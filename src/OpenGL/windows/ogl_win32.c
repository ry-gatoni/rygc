proc B32
win32_ogl_init(void)
{
  B32 result = 1;
  // TODO: handle errors/failures

  // NOTE: load wgl functions
  HMODULE ogl_module = LoadLibraryA("opengl32.dll");
/*   { */
/* #define X(name, ret, args) name = (void*)GetProcAddress(ogl_module, #name); Assert(name); */
/*     WGL_FUNCTION_XLIST() */
/* #undef X */
/*   } */
  
  // NOTE: create bootstrap window and get device context
  LPCSTR bootstrap_class_name = (LPCSTR)L"win32-ogl-bootstrap";
  WNDCLASS wnd_class = {0};
  HWND bootstrap_window = 0;
  HDC dc = 0;
  {
    wnd_class.lpfnWndProc = DefWindowProcW;
    wnd_class.hInstance = GetModuleHandle(0);
    wnd_class.lpszClassName = bootstrap_class_name;
    RegisterClass(&wnd_class);
    bootstrap_window =
      CreateWindowEx(0, bootstrap_class_name, (LPCSTR)L"", 0, 0, 0, 0, 0, 0, 0, wnd_class.hInstance, 0);
    dc = GetDC(bootstrap_window);
  }

  // NOTE: request pixel format
  int pf = 0;
  {
    PIXELFORMATDESCRIPTOR pfd = {0};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pf = ChoosePixelFormat(dc, &pfd);
    int max_pf = DescribePixelFormat(dc, pf, sizeof(pfd), &pfd);
    BOOL success = SetPixelFormat(dc, pf, &pfd);
    Unused(max_pf);
    Unused(success);
  }

  // NOTE: make boostrap context
  HGLRC glrc = wglCreateContext(dc);
  BOOL make_current_success = wglMakeCurrent(dc, glrc);
  Unused(make_current_success);
  
  // NOTE: load wgl extensions
  {
#define X(name, ret, args) name = (void*)wglGetProcAddress(#name); Assert(name);
    WGL_EXTENSION_XLIST()
#undef X    
  }

  // NOTE: set up real pixel format
  {
    int pf_attribs[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB, 32,
      WGL_DEPTH_BITS_ARB, 24,
      WGL_STENCIL_BITS_ARB, 8,
      0,
    };

    UINT fmt_count = 0;
    BOOL success = wglChoosePixelFormatARB(dc, pf_attribs, 0, 1, &pf, &fmt_count);
    Assert(success);
    w32_state->pf = pf;
  }

  // NOTE: make real context
  {
    int ctxt_attribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
      WGL_CONTEXT_MINOR_VERSION_ARB, 3,
      0,
    };
    w32_state->gfx_ctxt = wglCreateContextAttribsARB(dc, glrc, ctxt_attribs);
  }

  // NOTE: load opengl functions
  {
#define X(name, ret, args) name = (void*)GetProcAddress(ogl_module, #name); if(!name) name = (void*)wglGetProcAddress(#name);
    OGL_FUNCTION_XLIST
#undef X
  }

  // NOTE: clean up boostrap
  {
    Assert(wglMakeCurrent(dc, 0));
    Assert(wglDeleteContext(glrc));
    ReleaseDC(bootstrap_window, dc);
    DestroyWindow(bootstrap_window);
    UnregisterClass(bootstrap_class_name, wnd_class.hInstance);
    //wglMakeCurrent(dc, w32_state->gfx_ctxt);
  }

  return(result);
}

// NOTE: public api
proc B32
ogl_init(void)
{
  B32 result = win32_ogl_init();
  return(result);
}
