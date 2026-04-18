// -----------------------------------------------------------------------------
// vst3 types/interfaces

typedef enum TResult
{
  KResult_ok			= 0,
  // who tf would define true == 0 and false == 1????
  KResult_true			= KResult_ok, // S_OK
  KResult_false			= 1,          // S_FALSE
  KResult_no_interface		= 0x80004002, // E_NOINTERFACE
  KResult_invalid_argument	= 0x80070057, // E_INVALIDARG
  KResult_not_implemented	= 0x80004001, // E_NOTIMPL
  KResult_iternal_error		= 0x80004005, // E_FAIL
  KResult_not_initialized	= 0x8000FFFF, // E_UNEXPECTED
  KResult_out_of_memory		= 0x8007000E, // E_OUTOFMEMORY
} TResult;

typedef union Tuid
{
  U8 b[16];
  U32 dw[4];
  const char s[16];
} Tuid;
#if OS_WINDOWS
// TODO: test
#  define VST3_UID(dw0, dw1, dw2, dw3) (Tuid){ .dw = { dw0, dw1, dw2, dw3}, }.s;
#elif OS_MAC || OS_LINUX
#  define VST3_UID(dw0, dw1, dw2, dw3) (Tuid){ .b = {\
      (dw0 & 0xFF000000)>>3*8, (dw0 & 0xFF0000)>>2*8, (dw0 & 0xFF00)>>1*8, (dw0 & 0xFF)>>0*8,\
      (dw1 & 0xFF000000)>>3*8, (dw1 & 0xFF0000)>>2*8, (dw1 & 0xFF00)>>1*8, (dw1 & 0xFF)>>0*8,\
      (dw2 & 0xFF000000)>>3*8, (dw2 & 0xFF0000)>>2*8, (dw2 & 0xFF00)>>1*8, (dw2 & 0xFF)>>0*8,\
      (dw3 & 0xFF000000)>>3*8, (dw3 & 0xFF0000)>>2*8, (dw3 & 0xFF00)>>1*8, (dw3 & 0xFF)>>0*8,\
    }, }.s;
#else
#  error unsupported os
#endif

typedef const char *FidString;

typedef struct PFactoryInfo
{
  //struct{ void *table[]; } *v;
  U8 vendor[64];
  U8 url[256];
  U8 email[128];
  S32 flags;
} PFactoryInfo;

typedef struct PClassInfo
{
  //struct{ void *table[]; } *v;
  Tuid cid;
  S32 cardinality;
  U8 category[32];
  U8 name[64];
} PClassInfo;

typedef struct FUnknown FUnknown;
struct FUnknown
{
  union{
    struct{
      TResult (*query_interface)(FUnknown *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(FUnknown *_this);
      U32 (*release)(FUnknown *_this);
    };
    void *table[3];
  } *v;
};

typedef struct IPluginFactory IPluginFactory;
struct IPluginFactory
{
  union{
    struct{
      TResult (*query_interface)(IPluginFactory *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IPluginFactory *_this);
      U32 (*release)(IPluginFactory *_this);
      TResult (*get_factory_info)(IPluginFactory *_this, PFactoryInfo *info);
      S32 (*count_classes)(IPluginFactory *_this);
      TResult (*get_class_info)(IPluginFactory *_this, S32 index, PClassInfo *info);
      TResult (*create_instance)(IPluginFactory *_this, FidString cid, FidString iid, void **obj);
    };
    void *table[7];
  } *v;
};

typedef IPluginFactory* (Vst3_GetFactoryProc)(void);

// -----------------------------------------------------------------------------
// vst3 interface member functions

proc inline S32
IPluginFactory_CountClasses(IPluginFactory *_this)
{ return(_this->v->count_classes(_this)); }

proc inline TResult
IPluginFactory_GetClassInfo(IPluginFactory *_this, S32 index, PClassInfo *ci)
{ return(_this->v->get_class_info(_this, index, ci)); }

proc inline TResult
IPluginFactory_CreateInstance(IPluginFactory *_this, FidString cid, FidString iid, void **obj)
{ return(_this->v->create_instance(_this, cid, iid, obj)); }

proc inline U32
IPluginFactory_Release(IPluginFactory *_this)
{ return(_this->v->release(_this)); }

// -----------------------------------------------------------------------------
// vst3 constants

global FidString const vst3_funknown_iid = VST3_UID(0x00000000, 0x00000000, 0xC0000000, 0x00000046);

// -----------------------------------------------------------------------------
// internal types/state

typedef struct Vst3_HostState
{
  Arena *arena;

  // TODO: support having multiple plugins loaded
  Os_Handle plugin_handle;
  Vst3_GetFactoryProc *get_factory_proc;
  void *init_proc;
  void *exit_proc;

  U64 plugin_interface_count;
  FUnknown **plugin_interfaces;
} Vst3_HostState;

global Vst3_HostState *vst3_host_state = 0;

#if OS_LINUX
global String8 const vst3_init_proc_name_for_os = Str8Lit("ModuleEntry");
global String8 const vst3_exit_proc_name_for_os = Str8Lit("ModuleExit");
#elif OS_WINDOWS
global String8 const vst3_init_proc_name_for_os = Str8Lit("InitDll");
global String8 const vst3_exit_proc_name_for_os = Str8Lit("ExitDll");
#elif OS_MAC
global String8 const vst3_init_proc_name_for_os = Str8Lit("bundleEntry");
global String8 const vst3_exit_proc_name_for_os = Str8Lit("bundleExit");
#else
#  error unsupported os
#endif

// -----------------------------------------------------------------------------
// init/uninit layer

proc void vst3_host_init(void);
proc void vst3_host_uninit(void);

// -----------------------------------------------------------------------------
// load/unload plugin

proc Os_Handle vst3_host_load_plugin(String8 path_to_plugin_binary);
proc void vst3_host_unload_plugin(Os_Handle plugin_handle);

/// -----------------------------------------------------------------------------
// init/uninit plugin

proc void vst3_host_init_plugin(void);
proc void vst3_host_uninit_plugin(void);
