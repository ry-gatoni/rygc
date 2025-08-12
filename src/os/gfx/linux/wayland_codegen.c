#include "base/base.h"
#include "os/os.h"

#include "xml.h"

#include "base/base.c"
#include "os/os.c"

proc B32
is_whitespace(U8 c)
{
  B32 result = (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\r' || c == '\f');
  return(result);
}

proc B32
is_letter(U8 c)
{
  B32 result = ((c >= 'a' && c <= 'z') || (c >= 'A' || c <= 'Z'));
  return(result);
}

proc B32
is_number(U8 c)
{
  B32 result = ((c >= '0') && (c <= '9'));
  return(result);
}

#include <stdio.h>

#include "xml.c"

// TODO: hash table?
proc String8
rygc_type_from_wayland_type(Arena *arena, String8 wayland_type)
{
  String8 result = {0};
  if(str8s_are_equal(wayland_type, Str8Lit("uint")) ||
     str8s_are_equal(wayland_type, Str8Lit("new_id")) ||
     str8s_are_equal(wayland_type, Str8Lit("object"))) {
    result = arena_push_str8_copy(arena, Str8Lit("U32"));
  }
  else if(str8s_are_equal(wayland_type, Str8Lit("string"))) {
    result = arena_push_str8_copy(arena, Str8Lit("String8"));
  }
  else if(str8s_are_equal(wayland_type, Str8Lit("int"))) {
    result = arena_push_str8_copy(arena, Str8Lit("S32"));
  }
  else if(str8s_are_equal(wayland_type, Str8Lit("fd"))) {
    // NOTE: mark file descriptors to be passed as ancillary data, unlike other args
    result = arena_push_str8_copy(arena, Str8Lit("int"));
  }
  else {
    Assert(!"ERROR: unknown wayland type");
  }

  return(result);
}

proc String8
prefix_from_string(String8 string, U8 delimiter)
{
  U32 delimiter_pos = 0;
  for(U32 i = 0; i < string.count; ++i) {
    if(string.string[i] == delimiter) {
      delimiter_pos = i;
      break;
    }
  }
      
  String8 result = {.string = string.string, .count = delimiter_pos};
  return(result);
}

global String8List ignored_prefixes = {0};

global String8List message_opcode_list = {0};
global String8List request_function_list = {0};

proc ParsedXml
load_and_parse_xml(Arena *parse_arena, String8 xml_path)
{
  ArenaTemp scratch = arena_get_scratch(&parse_arena, 1);

  String8 contents = os_read_entire_file(scratch.arena, xml_path);
  ParsedXml result = xml_parse(parse_arena, contents);

  arena_release_scratch(scratch);
  return(result);
}

proc void
generate_code_from_wayland_xml(Arena *codegen_arena, ParsedXml protocol)
{  
  U64 depth = 0;
  XmlIterator iter = {0};
  for(XmlNode *node = protocol.root;
      node;
      iter = xml_iterate_breadth_first_preorder(node), node = iter.next) {
    if(depth > 1) break;

    if(str8s_are_equal(node->label, Str8Lit("interface"))) {
      String8 interface_name = node->first_attribute->value;      

      String8 prefix = prefix_from_string(interface_name, '_');
      B32 prefix_is_in_list = 0;
      for(String8Node *prefix_node = ignored_prefixes.first; prefix_node; prefix_node = prefix_node->next) {
	if(str8s_are_equal(prefix_node->string, prefix)) {
	  prefix_is_in_list = 1;
	  break;
	}
      }

      if(!prefix_is_in_list) {
	str8_list_push(codegen_arena, &ignored_prefixes, prefix);
      }

      U32 request_counter = 0;
      U32 event_counter = 0;
      for(XmlNode *message = node->first; message; message = message->next) {
	B32 is_request_node = str8s_are_equal(message->label, Str8Lit("request"));
	B32 is_event_node = str8s_are_equal(message->label, Str8Lit("event"));
	if(is_request_node || is_event_node) {
	  // NOTE: get event/request name
	  String8 message_name = message->first_attribute->value;
	  U32 index = 0;

	  if(is_request_node) {
	    index = request_counter++;
	    B32 passes_fd = 0;
	    
	    // NOTE: request message sending functions
	    String8List arg_name_list = {0};
	    String8List arg_type_list = {0};
	    ArenaTemp arg_temp = arena_get_scratch(&codegen_arena, 1); // TODO: investigate

	    str8_list_push_f(arg_temp.arena, &arg_name_list,
			     "%.*s_id",
			     (int)interface_name.count, interface_name.string);
	    str8_list_push(arg_temp.arena, &arg_type_list, Str8Lit("uint"));

	    for(XmlNode *arg = message->first; arg; arg = arg->next) {
	      if(str8s_are_equal(arg->label, Str8Lit("arg"))) {
		String8 arg_name = arg->first_attribute->value;
		String8 arg_type = arg->first_attribute->next->value;

		str8_list_push(arg_temp.arena, &arg_name_list, arg_name);
		str8_list_push(arg_temp.arena, &arg_type_list, arg_type);
	      }
	    }
	    
	    // NOTE: function name and return value
	    str8_list_push_f(codegen_arena, &request_function_list,
			     "proc B32\n%.*s_%.*s(",
			     (int)interface_name.count, interface_name.string,
			     (int)message_name.count, message_name.string);

	    // NOTE: function signature
	    for(String8Node *arg_name_node = arg_name_list.first, *arg_type_node = arg_type_list.first;
		arg_name_node && arg_type_node;
		arg_name_node = arg_name_node->next, arg_type_node = arg_type_node->next) {
	      String8 arg_name = arg_name_node->string;
	      String8 arg_type = rygc_type_from_wayland_type(arg_temp.arena, arg_type_node->string);
	      
	      str8_list_push_f(codegen_arena, &request_function_list, "%s%.*s %.*s",
			       arg_name_node == arg_name_list.first ? "" : ", ",
			       (int)arg_type.count, arg_type.string,
			       (int)arg_name.count, arg_name.string);
	    }
	    str8_list_push(codegen_arena, &request_function_list, Str8Lit(")\n{\n"));

	    // NOTE: message_header
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  B32 result = 1;\n"));
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  ArenaTemp scratch = arena_get_scratch(0, 0);\n\n"));
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  PushBuffer buf = push_buffer_alloc(scratch.arena, 1024);\n"));
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  WaylandMessageHeader *message_header = buf_push_struct(&buf, WaylandMessageHeader);\n"));
	    str8_list_push_f(codegen_arena, &request_function_list,
			     "  message_header->object_id = %.*s_id;\n",
			     (int)interface_name.count, interface_name.string);
	    str8_list_push_f(codegen_arena, &request_function_list,
			     "  message_header->opcode = %.*s_%.*s_opcode;\n\n",
			     (int)interface_name.count, interface_name.string,
			     (int)message_name.count, message_name.string);

	    // NOTE: message body
	    for(String8Node *arg_name_node = arg_name_list.first, *arg_type_node = arg_type_list.first;
		arg_name_node && arg_type_node;
		arg_name_node = arg_name_node->next, arg_type_node = arg_type_node->next) {
	      String8 arg_name = arg_name_node->string;
	      String8 arg_type = rygc_type_from_wayland_type(arg_temp.arena, arg_type_node->string);

	      String8 arg_name_prefix = prefix_from_string(arg_name, '_');
	      B32 prefix_is_ignored = 0;
	      for(String8Node *prefix_node = ignored_prefixes.first; prefix_node; prefix_node = prefix_node->next) {
		if(str8s_are_equal(prefix_node->string, arg_name_prefix)) {
		  prefix_is_ignored = 1;
		  break;
		}
	      }

	      if(prefix_is_ignored) continue;
	      
	      if(str8s_are_equal(arg_type, Str8Lit("U32"))) {
		str8_list_push_f(codegen_arena, &request_function_list,
				 "  *buf_push_struct(&buf, U32) = %.*s;\n",
				 (int)arg_name.count, arg_name.string);
	      }
	      else if(str8s_are_equal(arg_type, Str8Lit("S32"))) {
		str8_list_push_f(codegen_arena, &request_function_list,
				 "  *buf_push_struct(&buf, S32) = %.*s;\n",
				 (int)arg_name.count, arg_name.string);
	      }
	      else if(str8s_are_equal(arg_type, Str8Lit("String8"))) {
		str8_list_push_f(codegen_arena, &request_function_list,
				 "  *buf_push_struct(&buf, U32) = %.*s.count + 1;\n",
				 (int)arg_name.count, arg_name.string);
		str8_list_push_f(codegen_arena, &request_function_list,
				 "  buf_push_str8_copy(&buf, %.*s);\n",
				 (int)arg_name.count, arg_name.string);
	      }
	      else if(str8s_are_equal(arg_type, Str8Lit("int"))) {
		passes_fd = 1;
	      }
	    }

	    // NOTE: send message and return
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  U32 message_size = AlignPow2(buf.pos, 4);\n"));
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  message_header->message_size = message_size;\n\n"));
	    if(passes_fd) {	      
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  U64 buffer_len = CMSG_SPACE(sizeof(fd));\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  U8 *buffer = arena_push_array(scratch.arena, U8, buffer_len);\n\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  struct iovec io = {.iov_base = message_header, .iov_len = message_size};\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  struct msghdr socket_msg = {.msg_iov = &io, .msg_iovlen = 1, .msg_control = buffer, .msg_controllen = buffer_len};\n\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&socket_msg);\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  cmsg->cmsg_level = SOL_SOCKET;\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  cmsg->cmsg_type = SCM_RIGHTS;\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  cmsg->cmsg_len = CMSG_LEN(sizeof(fd));\n\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  *((int *)CMSG_DATA(cmsg)) = fd;\n"));
	      /* str8_list_push(codegen_arena, &request_function_list, */
	      /* 		     Str8Lit("  socket_msg.msg_controllen = CMSG_SPACE(sizeof(fd));\n\n")); */
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  int send_size = sendmsg(wayland_state.display_socket_handle, &socket_msg, 0);\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  if(send_size == -1) {\n    result = 0;\n  }\n\n"));
	    }
	    else {
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  int send_size = send(wayland_state.display_socket_handle, message_header, message_size, 0);\n"));
	      str8_list_push(codegen_arena, &request_function_list,
			     Str8Lit("  if(send_size == -1) {\n    result = 0;\n  }\n\n"));
	    }
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  arena_release_scratch(scratch);\n"));	    
	    str8_list_push(codegen_arena, &request_function_list,
			   Str8Lit("  return(result);\n}\n\n"));

	    arena_release_scratch(arg_temp);
	  }
	  else if(is_event_node) {
	    index = event_counter++;
	  }

	  // NOTE: global opcode constants
	  str8_list_push_f(codegen_arena, &message_opcode_list,
			   "global U32 %.*s_%.*s_%-*s = %2u; // %s\n",
			   (int)interface_name.count, interface_name.string,
			   (int)message_name.count, message_name.string,
			   (int)(50 - interface_name.count - message_name.count), "opcode",
			   index,
			   is_request_node ? "request" : "event");
	}
      }
    }
    
    depth += iter.push_count;
    depth -= iter.pop_count;
  }
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  // NOTE: read and parse xml
  Arena *parse_arena = arena_alloc();
  ParsedXml wayland_protocol = load_and_parse_xml(parse_arena, Str8Lit("../data/xml/wayland.xml"));
  ParsedXml xdg_shell_protocol = load_and_parse_xml(parse_arena, Str8Lit("../data/xml/xdg-shell.xml"));
  ParsedXml linux_dmabuf_v1_protocol = load_and_parse_xml(parse_arena, Str8Lit("../data/xml/linux-dmabuf-v1.xml"));

  //print_parsed_xml(wayland_protocol);
  //print_parsed_xml(xdg_shell_protocol);
  
  // NOTE: codegen
  Arena *codegen = arena_alloc();
  generate_code_from_wayland_xml(codegen, wayland_protocol);
  generate_code_from_wayland_xml(codegen, xdg_shell_protocol);
  generate_code_from_wayland_xml(codegen, linux_dmabuf_v1_protocol);
  printf("generated code\n");
  
  String8 message_opcodes = str8_join(codegen, &message_opcode_list);
  String8 request_functions = str8_join(codegen, &request_function_list);

  String8List code_list = {0};
  str8_list_push(codegen, &code_list, message_opcodes);
  str8_list_push(codegen, &code_list, Str8Lit("\n// NOTE: message request functions\n"));
  str8_list_push(codegen, &code_list, request_functions);
  String8 code = str8_join(codegen, &code_list);

  printf("concatenated code\n");

  os_write_entire_file(code, Str8Lit("../src/os/gfx/linux/wayland_generated.c"));
  printf("wrote code to file\n");

  return(0);
}
