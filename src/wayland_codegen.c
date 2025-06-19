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

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  ArenaTemp scratch = arena_get_scratch(0, 0);

  // NOTE: read and parse xml
  String8 wayland_protocol_contents = os_read_entire_file(scratch.arena, Str8Lit("/usr/share/wayland/wayland.xml"));
  printf("wayland protocol xml size: %lu\n", wayland_protocol_contents.count);
  ParsedXml wayland_protocol_xml = xml_parse(scratch.arena, wayland_protocol_contents);
  printf("wayland protocol xml node count: %lu\n", wayland_protocol_xml.node_count);
  printf("wayland protocol xml protocol node child count: %lu\n", wayland_protocol_xml.root->child_count);

  // NOTE: codegen
  ArenaTemp codegen = arena_begin_temp(scratch.arena);
  String8List message_opcode_list = {0};

  String8 interface_str = Str8Lit("interface");
  String8 request_str = Str8Lit("request");
  String8 event_str = Str8Lit("event");
  String8 arg_str = Str8Lit("arg");
  String8 name_str = Str8Lit("name");
  String8 type_str = Str8Lit("type");

  U64 depth = 0;
  XmlIterator iter = {0};
  for(XmlNode *node = wayland_protocol_xml.root;
      node;
      iter = xml_iterate_breadth_first_preorder(node), node = iter.next) {
    if(depth > 1) break;

    if(str8s_are_equal(node->label, interface_str)) {
      String8 interface_name = node->first_attribute->value;      

      U32 request_counter = 0;
      U32 event_counter = 0;
      for(XmlNode *message = node->first; message; message = message->next) {
	B32 is_request_node = str8s_are_equal(message->label, request_str);
	B32 is_event_node = str8s_are_equal(message->label, event_str);
	if(is_request_node || is_event_node) {
	  // NOTE: get event/request name
	  String8 message_name = message->first_attribute->value;
	  U32 index = 0;

	  if(is_request_node) {
	    index = request_counter++;	  	    	    
	  }
	  else if(is_event_node) {
	    index = event_counter++;
	  }

	  // NOTE: global opcode constants
	  str8_list_push_f(codegen.arena, &message_opcode_list,
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

  String8 message_opcodes = str8_join(codegen.arena, &message_opcode_list);

  os_write_entire_file(message_opcodes, Str8Lit("../src/wayland_generated.c"));

  /* S32 depth = 0; */
  /* XmlIterator iter = {0}; */
  /* for(XmlNode *node = wayland_protocol_xml.root; */
  /*     node; */
  /*     iter = xml_iterate_depth_first_preorder(node), node = iter.next) { */
  /*   depth += iter.push_count; */
  /*   depth -= iter.pop_count; */

  /*   for(S32 i = 0; i < depth; ++i) { */
  /*     printf("|"); */
  /*   } */
  /*   printf("label=%.*s", (int)node->label.count, node->label.string); */
  /*   for(XmlAttribute *attr = node->first_attribute; attr; attr = attr->next) { */
  /*     printf(", %.*s=%.*s", (int)attr->name.count, attr->name.string, (int)attr->value.count, attr->value.string); */
  /*   } */
  /*   /\* if(node->body.count) { *\/ */
  /*   /\*   printf("\n%.*s", (int)node->body.count, node->body.string); *\/ */
  /*   /\* } *\/ */
  /*   printf("\n"); */
  /* } */

  arena_end_temp(codegen);
  arena_release_scratch(scratch);

  return(0);
}
