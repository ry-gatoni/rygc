typedef struct WaylandSoftware_Framebuffer
{
  /* Wayland_Id *buffer; */
} WaylandSoftware_Framebuffer;

typedef struct WaylandSoftware_Window WaylandSoftware_Window;
struct WaylandSoftware_Window
{
  WaylandSoftware_Window *next_free;

  Wayland_Window *wayland_window;

  Wayland_Id *wl_shm_pool;

  void *shared_memory;
  U64 shared_memory_size;

  /* U32 backbuffer_index; */
  WaylandSoftware_Framebuffer framebuffers[2];
};

typedef struct WaylandSoftware_State
{
  WaylandSoftware_Window *freelist;
} WaylandSoftware_State;

global WaylandSoftware_State *wayland_software_state = 0;

// -----------------------------------------------------------------------------
// helpers

proc B32 wayland_software__allocate_shared_memory(WaylandSoftware_Window *window, U64 size);
proc B32 wayland_software__create_shm_buffer(WaylandSoftware_Window *window);

proc WaylandSoftware_Window* wayland_software__window_create(Arena *arena);
proc void wayland_software__window_destroy(WaylandSoftware_Window *window);
proc WaylandSoftware_Window* wayland_software__window_get(Wayland_Window *window);

// -----------------------------------------------------------------------------
// functions

proc B32 wayland_software_init(void);
proc void wayland_software_uninit(void);

proc WaylandSoftware_Window* wayland_software_window_open(Wayland_Window *window);
proc void wayland_software_window_close(WaylandSoftware_Window *window);

proc B32 wayland_software_window_create_buffer(Wayland_Window *window);
proc B32 wayland_software_window_begin(Wayland_Window *window);
proc B32 wayland_software_window_end(Wayland_Window *window);
