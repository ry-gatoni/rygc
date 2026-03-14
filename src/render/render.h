#include "render.common.h"

#define RENDER_BACKEND_SOFTWARE 0
#define RENDER_BACKEND_OPENGL 1
#define RENDER_BACKEND_VULKAN 2

#if !defined(RENDER_BACKEND)
#  define RENDER_BACKEND RENDER_BACKEND_OPENGL
#endif

#if RENDER_BACKEND == RENDER_BACKEND_SOFTWARE
#  include "render.software.h"
#elif RENDER_BACKEND == RENDER_BACKEND_OPENGL
#  include "render.ogl.h"
#elif RENDER_BACKEND == RENDER_BACKEND_VULKAN
#  error TODO
#else
#  error ERROR: unrecognized value for `RENDER_BACKEND`
#endif
