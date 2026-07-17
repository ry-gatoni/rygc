#include "ogl_api_defs.h"

typedef struct Ogl_MacContext
{
  NSOpenGLContext *ns_ctxt;
} Ogl_MacContext;

global Ogl_MacContext mac_ogl_ctxt = {0};

proc B32 ogl_mac_init(void);

proc Ogl_MacContext ogl_mac_create_context(void);
proc void ogl_mac_make_context_current(Ogl_MacContext *ctxt);
proc void ogl_mac_swap_buffers(Ogl_MacContext *ctxt);
