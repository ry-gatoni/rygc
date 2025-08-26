// NOTE: copied with modifications from https://github.com/KhronosGroup/OpenGL-Registry/blob/main/api/GL/wgl.h

#define WGL_DRAW_TO_WINDOW_ARB				0x2001
#define WGL_SUPPORT_OPENGL_ARB				0x2010
#define WGL_DOUBLE_BUFFER_ARB				0x2011
#define WGL_PIXEL_TYPE_ARB				0x2013
#define WGL_COLOR_BITS_ARB				0x2014
#define WGL_DEPTH_BITS_ARB				0x2022
#define WGL_STENCIL_BITS_ARB				0x2023
#define WGL_TYPE_RGBA_ARB				0x202B
#define WGL_CONTEXT_MAJOR_VERSION_ARB			0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB			0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB			0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB		0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB	0x00000002

#define WGL_FUNCTION_XLIST()			\
  X(wglCreateContext, HGLRC, (HDC dc))\
    X(wglMakeCurrent, BOOL, (HDC dc, HGLRC glrc))\
    X(wglGetProcAddress, PROC, (LPSCTR name))\
    X(wglDeleteContext, BOOL, (HGLRC glrc))

#define WGL_EXTENSION_XLIST()						\
  X(wglChoosePixelFormatARB, BOOL, (HDC hdc, const int *piAttribList, const FLOAT *pfAttribList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats))\
    X(wglCreateContextAttribsARB, HGLRC, (HDC hdc, HGLRC hShareContext, const int *attribList))

#define X(name, ret, args) typedef ret Glue(Win32_, name)args;
//WGL_FUNCTION_XLIST()
WGL_EXTENSION_XLIST()
#undef X

#define X(name, ret, args) global Glue(Win32_, name) *name = 0;
//WGL_FUNCTION_XLIST()
WGL_EXTENSION_XLIST()
#undef X
