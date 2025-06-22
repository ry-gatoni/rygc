#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

#include <sys/socket.h>
#include <sys/un.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "wayland.h"
#include "wayland.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  //wayland_state_initialize();

  B32 connected = wayland_display_connect();
  if(connected) {
    if(wayland_display_get_registry()) {
      //U32 registry_id = *wayland_interface_object_id_from_name(Str8Lit("wl_registry"));
      printf("registry id: %u\n", wayland_state.wl_registry_id);
      
      U64 shared_memory_size = MB(32);
      if(wayland_allocate_shared_memory(shared_memory_size)) {
	Arena *framebuffer_arena = arena_alloc_ex(shared_memory_size, shared_memory_size,
						  wayland_state.shared_memory, 0);
	printf("framebuffer success!\n");

	Arena *frame_arena = arena_alloc();
	B32 running = 1;
	while(running) {
	  U64 message_buffer_size = KB(4);
	  U8 *message_buffer = arena_push_array_z(frame_arena, U8, message_buffer_size);
	  S64 recv_size = recv(wayland_state.display_socket_handle, message_buffer, message_buffer_size, 0);
	  if(recv_size == -1) {
	    running = 0;
	    // TODO: log error: message receive failure
	  }

	  U8 *message = message_buffer;
	  S64 message_len = recv_size;
	  printf("message len: %ld\n", message_len);
	  while(message_len > 0) {
	    wayland_handle_message(&message, &message_len);
	  }
	  
	  if(wayland_state.wl_shm_id && wayland_state.wl_compositor_id && wayland_state.xdg_wm_base_id &&
	     !wayland_state.wl_surface_id) {
	    printf("shm id: %u\n", wayland_state.wl_shm_id);
	    printf("compositor id: %u\n", wayland_state.wl_compositor_id);
	    printf("xdg wm base id: %u\n", wayland_state.xdg_wm_base_id);

	    if(wl_compositor_create_surface(++wayland_current_id)) {
	      wayland_state.wl_surface_id = wayland_current_id;
	      printf("surface id: %u\n", wayland_state.wl_surface_id);
	      
	      if(xdg_wm_base_get_xdg_surface(++wayland_current_id, wayland_state.wl_surface_id)) {
		wayland_state.xdg_surface_id = wayland_current_id;
		printf("xdg surface id: %u\n", wayland_state.xdg_surface_id);

		if(xdg_surface_get_toplevel(++wayland_current_id)) {
		  wayland_state.xdg_toplevel_id = wayland_current_id;
		  printf("xdg toplevel id: %u\n", wayland_state.xdg_toplevel_id);

		  if(wl_surface_commit()) {
		    printf("surface committed\n");
		  }
		}
	      }
	    }	    
	  }

	  if(wayland_state.init_state == WaylandInit_acked) {
	    Assert(wayland_state.wl_surface_id);
	    Assert(wayland_state.xdg_surface_id);
	    Assert(wayland_state.xdg_toplevel_id);

	    if(!wayland_state.wl_shm_pool_id) {
	      if(wl_shm_create_pool(++wayland_current_id,
				    shared_memory_size)) {
		wayland_state.wl_shm_pool_id = wayland_current_id;
		printf("shm pool id: %u\n", wayland_state.wl_shm_pool_id);

		if(!wayland_state.wl_buffer_id) {
		  S32 offset = 0;
		  S32 width = 640;
		  S32 height = 480;
		  S32 stride = width*sizeof(U32);
		  U32 format = 0x34324241; // NOTE: bgra 32-bit little-endian // TODO: generate enums
		  if(wl_shm_pool_create_buffer(++wayland_current_id,
					       offset, width, height, stride, format)) {
		    wayland_state.wl_buffer_id = wayland_current_id;
		    printf("buffer id: %u\n", wayland_state.wl_buffer_id);

		    if(wl_surface_attach(wayland_state.wl_buffer_id, 0, 0)) {
		      printf("surface attached\n");
		      if(wl_surface_commit()) {
			printf("surface commited. initialization complete\n");
			wayland_state.init_state = WaylandInit_attached;
		      }
		    }		    		    
		  }
		}
	      }
	    }	    
	  }
	  
	  arena_clear(frame_arena);
	}
      }
    }
  }

  return(0);
}
