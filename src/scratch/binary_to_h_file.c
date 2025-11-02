#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

int main(int argc, char **argv)
{
  int result = 1;

  ArenaTemp scratch = arena_get_scratch(0, 0);

#define PopArg(argc, argv) ((*argc != 0) ? ((*argc)--, *argv++) : 0)
  
  PopArg(&argc, argv);
    
  char *dest_path_cstr = PopArg(&argc, argv);
  if(dest_path_cstr != 0 )
  {
    String8 dest_path = Str8Cstr(dest_path_cstr);

    U32 src_path_count = 0;
    U32 max_src_paths = 16;
    String8 *src_paths = arena_push_array(scratch.arena, String8, max_src_paths);
    for(;src_path_count < max_src_paths;)
    {
      char *src_path_cstr = PopArg(&argc, argv);
      if(src_path_cstr == 0) break;
      
      String8 src_path = Str8Cstr(src_path_cstr);
      src_paths[src_path_count++] = src_path;
    }

    if(src_path_count)
    {
      String8List header_list = {0};

      for(U32 src_idx = 0; src_idx < src_path_count; ++src_idx)
      {
	String8 src_path = src_paths[src_idx];
	String8 src_contents = os_read_entire_file(scratch.arena, src_path);

	String8List path_components_list = str8_split(scratch.arena, src_path, (U8*)"/.", 2);
	String8Array path_components_array = str8_array_from_list(scratch.arena, &path_components_list);
	String8 variable_name = path_components_array.strings[path_components_array.count - 2];	
	str8_list_push_f(scratch.arena, &header_list,
			 "static U8 %.*s[] = {\n  ",
			 (int)variable_name.count, variable_name.string);

	U32 byte_counter = 0;
	U32 bytes_per_line = 16;	
	U8 *start = src_contents.string;
	U8 *opl = start + src_contents.count;
	for(U8 *at = start; at < opl; ++at)
	{
	  str8_list_push_f(scratch.arena, &header_list, "%u,", *at);

	  ++byte_counter;
	  if(byte_counter == bytes_per_line)
	  {
	    str8_list_push(scratch.arena, &header_list, Str8Lit("\n  "));
	    byte_counter = 0;
	  }
	  else
	  {
	    str8_list_push(scratch.arena, &header_list, Str8Lit(" "));
	  }
	}

	str8_list_push(scratch.arena, &header_list, Str8Lit("\n};\n"));
      }

      String8 dest_contents = str8_join(scratch.arena, &header_list);
      os_write_entire_file(dest_contents, dest_path);
      result = 0;
    }
  }
  
  arena_release_scratch(scratch);
  return(result);
}
