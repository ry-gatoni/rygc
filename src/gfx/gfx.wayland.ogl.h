#include <EGL/egl.h>
#include <EGL/eglext.h> // TODO: runtime link egl

#define GL_GLEXT_PROTOTYPES // TODO: runtime link opengl
#include <GL/gl.h>

// TODO: find a better place for this
#define GlTextureDefaultParams() \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); \
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); \

typedef struct Wayland_Id Wayland_Id;
typedef struct Wayland_Window Wayland_Window;

typedef struct WaylandGl_Framebuffer
{
  Wayland_Id *buffer;

  U32 color_texture;
  U32 depth_texture;
  U32 fbo;

  S32 width;
  S32 height;

  EGLImage egl_image;
  int num_planes;
  int fourcc;
  int fd;
  EGLint stride;
  EGLint offset;
  U32 modifier_hi;
  U32 modifier_lo;
} WaylandGl_Framebuffer;

typedef struct WaylandGl_Window WaylandGl_Window;
struct WaylandGl_Window
{
  WaylandGl_Window *next_free;

  Wayland_Window *wayland_window;

  Wayland_Id *zwp_linux_buffer_params_v1;
  WaylandGl_Framebuffer framebuffers[2];
  /* U32 backbuffer_index; */
};

typedef struct WaylandGL_State
{
  WaylandGl_Window *freelist;

  EGLDisplay egl_display;
  EGLContext egl_context;

  PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR;
  PFNEGLEXPORTDMABUFIMAGEQUERYMESAPROC eglExportDMABUFImageQueryMESA;
  PFNEGLEXPORTDMABUFIMAGEMESAPROC eglExportDMABUFImageMESA;
} WaylandGl_State;

global WaylandGl_State *wayland_gl_state = 0;

// -----------------------------------------------------------------------------
// helpers

proc WaylandGl_Window* wayland_gl__window_create(Arena *arena);
proc void wayland_gl__window_destroy(WaylandGl_Window *window);

proc WaylandGl_Window* wayland_gl__window_get(Wayland_Window *window);
proc WaylandGl_Framebuffer wayland_gl__window_get_framebuffer(WaylandGl_Window *window);

proc B32 wayland_gl__allocate_textures(WaylandGl_Window *window);
proc B32 wayland_gl__create_buffer(WaylandGl_Window *window);

// -----------------------------------------------------------------------------
// functions

proc B32 wayland_gl_init(void);
proc void wayland_gl_uninit(void);

proc WaylandGl_Window* wayland_gl_window_open(Wayland_Window *window);
proc void wayland_gl_window_close(WaylandGl_Window *window);

proc B32 wayland_gl_window_create_buffer(Wayland_Window *window);
proc B32 wayland_gl_window_begin(Wayland_Window *window);
proc B32 wayland_gl_window_end(Wayland_Window *window);

//proc B32 wayland_create_gl_buffer(WaylandWindow *window);
