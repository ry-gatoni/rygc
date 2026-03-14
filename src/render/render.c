#include "render.common.c"

#if RENDER_BACKEND == RENDER_BACKEND_SOFTWARE
#  include "render.software.c"
#elif RENDER_BACKEND == RENDER_BACKEND_OPENGL
#  include "render.ogl.c"
#elif RENDER_BACKEND == RENDER_BACKEND_VULKAN
#  error TODO
#else
#  error ERROR: unrecognized value for `RENDER_BACKEND`
#endif
