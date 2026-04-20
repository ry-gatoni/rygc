// -----------------------------------------------------------------------------
// vst3 types/interfaces

// -----------------------------------------------------------------------------
// interface forward declarations

typedef struct FUnknown FUnknown;
typedef struct IPluginFactory IPluginFactory;
typedef struct IPluginBase IPluginBase;
typedef struct IBStream IBStream;
typedef struct IComponentHandler IComponentHandler;
typedef struct IPlugView IPlugView;
typedef struct IPlugFrame IPlugFrame;
typedef struct IEditController IEditController;
typedef struct IComponent IComponent;
typedef struct IParameterChanges IParameterChanges;
typedef struct IEventList IEventList;
typedef struct IAudioProcessor IAudioProcessor;
typedef struct IHostApplication IHostApplication;

// -----------------------------------------------------------------------------
// other types

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

typedef enum IBStream_Flags
{
  KIBStream_seek_set = 0,
  KIBStream_seek_cur,
  KIBStream_seek_end,
} IBStream_Flags;

typedef char Tuid[16];
typedef const char* FidString;

typedef struct Fuid
{
  Tuid data;
} Fuid;

#if OS_WINDOWS
// TODO: test
#  define VST3_UID(dw0, dw1, dw2, dw3) (Tuid){ .dw = { dw0, dw1, dw2, dw3}, }.s;
#elif OS_MAC || OS_LINUX
#  define VST3_UID(dw0, dw1, dw2, dw3) (Fuid){ .data = {\
      (dw0 & 0xFF000000)>>3*8, (dw0 & 0xFF0000)>>2*8, (dw0 & 0xFF00)>>1*8, (dw0 & 0xFF)>>0*8, \
      (dw1 & 0xFF000000)>>3*8, (dw1 & 0xFF0000)>>2*8, (dw1 & 0xFF00)>>1*8, (dw1 & 0xFF)>>0*8,\
      (dw2 & 0xFF000000)>>3*8, (dw2 & 0xFF0000)>>2*8, (dw2 & 0xFF00)>>1*8, (dw2 & 0xFF)>>0*8,\
      (dw3 & 0xFF000000)>>3*8, (dw3 & 0xFF0000)>>2*8, (dw3 & 0xFF00)>>1*8, (dw3 & 0xFF)>>0*8,\
  }, };
#else
#  error unsupported os
#endif

typedef const char *FidString;
#include <uchar.h>
typedef char16_t TChar;
typedef TChar Vst3_String128[128];

typedef U8 TBool;

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

typedef U32 ParamId;
typedef R64 ParamValue;
typedef S32 UnitId;
typedef struct ParameterInfo
{
  ParamId id;
  Vst3_String128 title;
  Vst3_String128 short_title;
  Vst3_String128 units;
  S32 step_count;
  ParamValue default_normalized_value; // normalized 0-1
  UnitId unit_id;
  S32 flags;
} ParameterInfo;

typedef S32 MediaType;
typedef S32 BusDirection;
typedef S32 BusType;
typedef S32 IoMode;
typedef struct BusInfo
{
  MediaType media_type;
  BusDirection direction;
  S32 channel_count;
  Vst3_String128 name;
  BusType type;
  U32 flags;
} BusInfo;

typedef struct RoutingInfo
{
  MediaType media_type;
  S32 bus_index;
  S32 channel;
} RoutingInfo;

typedef U64 SpeakerArrangement;
typedef U64 Speaker;
typedef R64 SampleRate;
typedef struct ProcessSetup
{
  S32 process_mode;
  S32 symbolic_sample_size;
  S32 max_samples_per_block;
  SampleRate sample_rate;
} ProcessSetup;

typedef R32 Sample32;
typedef R64 Sample64;
typedef struct AudioBusBuffers
{
  S32 num_channels;
  U64 silence_flags;
  union{
    Sample32 **channel_buffer__32;
    Sample64 **channel_buffer__64;
  };
} AudioBusBuffers;

typedef struct Chord
{
  U8 key_note;
  U8 root_note;
  S16 chord_mask;
} Chord;

typedef struct FrameRate
{
  U32 frames_per_second;
  U32 flags;
} FrameRate;

typedef S64 TSamples;
typedef R64 TQuarterNotes;
typedef struct ProcessContext
{
  U32 state;
  R64 sample_rate;
  TSamples project_time_samples;
  S64 system_time;
  TSamples continuous_time_samples;
  TQuarterNotes project_time_music;
  TQuarterNotes bar_position_music;
  TQuarterNotes cycle_start_music;
  TQuarterNotes cycle_end_music;
  R64 tempo;
  S32 time_sig_numerator;
  S32 time_sig_denominator;
  Chord chord;
  S32 smpte_offset_subframes;
  FrameRate frame_rate;
  S32 samples_to_next_clock;
} ProcessContext;

typedef struct ProcessData
{
  S32 process_mode;
  S32 symbolic_sample_size;
  S32 num_samples;
  S32 num_inputs;
  S32 num_outputs;
  AudioBusBuffers *inputs;
  AudioBusBuffers *outputs;
  IParameterChanges *input_parameter_changes;
  IParameterChanges *output_parameter_changes;
  IEventList *input_events;
  IEventList *output_events;
  ProcessContext *process_context;
} ProcessData;

typedef struct ViewRect
{
  // vtable?
  S32 left;
  S32 top;
  S32 right;
  S32 bottom;
} ViewRect;

// -----------------------------------------------------------------------------
// interface definitions

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

struct IPluginBase
{
  union{
    struct{
      TResult (*query_interface)(IPluginBase *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IPluginBase *_this);
      U32 (*release)(IPluginBase *_this);

      TResult (*initialize)(IPluginBase *_this, FUnknown *context);
      TResult (*terminate)(IPluginBase *_this);
    };
    void *table[5];
  } *v;
};

struct IBStream
{
  union{
    struct{
      TResult (*query_interface)(IBStream *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IBStream *_this);
      U32 (*release)(IBStream *_this);

      TResult (*read)(IBStream *_this, void *buffer, S32 num_bytes, S32 *num_bytes_read);
      TResult (*write)(IBStream *_this, void *buffer, S32 num_bytes, S32 *num_bytes_written);
      TResult (*seek)(IBStream *_this, S64 pos, S32 mode, S64 *result);
      TResult (*tell)(IBStream *_this, S64 *pos);
    };
    void *table[7];
  } *v;
};

struct IComponentHandler
{
  union{
    struct{
      TResult (*query_interface)(IComponentHandler *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IComponentHandler *_this);
      U32 (*release)(IComponentHandler *_this);

      TResult (*begin_edit)(IComponentHandler *_this, ParamId id);
      TResult (*perform_edit)(IComponentHandler *_this, ParamId id, ParamValue value_normalized);
      TResult (*end_edit)(IComponentHandler *_this, ParamId id);
      TResult (*restart_component)(IComponentHandler *_this, S32 flags);
    };
    void *table[7];
  } *v;
};

struct IPlugFrame
{
  union{
    struct{
      TResult (*query_interface)(IPlugFrame *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IPlugFrame *_this);
      U32 (*release)(IPlugFrame *_this);

      TResult (*resize_view)(IPlugFrame *_this, IPlugView *view, ViewRect *new_size);
    };
    void *table[14];
  } *v;
};

struct IPlugView
{
  union{
    struct{
      TResult (*query_interface)(IPlugView *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IPlugView *_this);
      U32 (*release)(IPlugView *_this);

      TResult (*attached)(IPlugView *_this, void *parent, FidString type);
      TResult (*removed)(IPlugView *_this);
      TResult (*on_wheel)(IPlugView *_this, R32 distance);
      TResult (*on_key_down)(IPlugView *_this, U16 key, S16 key_code, S16 modifiers);
      TResult (*on_key_up)(IPlugView *_this, U16 key, S16 key_code, S16 modifiers);
      TResult (*get_size)(IPlugView *_this, ViewRect *size);
      TResult (*on_size)(IPlugView *_this, ViewRect *new_size);
      TResult (*on_focus)(IPlugView *_this, TBool state);
      TResult (*set_frame)(IPlugView *_this, IPlugFrame *frame);
      TResult (*can_resize)(IPlugView *_this);
      TResult (*check_size_constraint)(IPlugView *_this, ViewRect *rect);
    };
    void *table[14];
  } *v;
};

struct IEditController
{
  union{
    struct{
      TResult (*query_interface)(IEditController *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IEditController *_this);
      U32 (*release)(IEditController *_this);

      TResult (*initialize)(IEditController *_this, FUnknown *context);
      TResult (*terminate)(IEditController *_this);

      TResult (*set_component_state)(IEditController *_this, IBStream *state);
      TResult (*set_state)(IEditController *_this, IBStream *state);
      TResult (*get_state)(IEditController *_this, IBStream *state);
      S32 (*get_parameter_count)(IEditController *_this);
      TResult (*get_parameter_info)(IEditController *_this, S32 param_index, ParameterInfo *info);
      TResult (*get_param_string_by_value)(IEditController *_this, ParamId id, ParamValue value_normalized, Vst3_String128 string);
      TResult (*get_param_value_by_string)(IEditController *_this, ParamId id, TChar *string, ParamValue *value_normalized);
      ParamValue (*normalized_param_to_plain)(IEditController *_this, ParamId id, ParamValue value_normalized);
      ParamValue (*plain_param_to_normalized)(IEditController *_this, ParamId id, ParamValue plain_value);
      ParamValue (*get_param_normalized)(IEditController *_this, ParamId id);
      TResult (*set_param_normalized)(IEditController *_this, ParamId id, ParamValue value);
      TResult (*set_component_handler)(IEditController *_this, IComponentHandler *handler);
      IPlugView* (*create_view)(IEditController *_this, FidString name);
    };
    void *table[18];
  } *v;
};

struct IComponent
{
  union{
    struct{
      TResult (*query_interface)(IComponent *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IComponent *_this);
      U32 (*release)(IComponent *_this);

      TResult (*initialize)(IComponent *_this, FUnknown *context);
      TResult (*terminate)(IComponent *_this);

      TResult (*get_controller_class_id)(IComponent *_this, Tuid class_id);
      TResult (*set_io_mode)(IComponent *_this, IoMode mode);
      S32 (*get_bus_count)(IComponent *_this, MediaType type, BusDirection dir);
      TResult (*get_bus_info)(IComponent *_this, MediaType type, BusDirection dir, S32 index, BusInfo *bus);
      TResult (*get_routing_info)(IComponent *_this, RoutingInfo *in_info, RoutingInfo *out_info);
      TResult (*activate_bus)(IComponent *_this, MediaType type, BusDirection dir, S32 index, TBool state);
      TResult (*set_active)(IComponent *_this, TBool state);
      TResult (*set_state)(IComponent *_this, IBStream *state);
      TResult (*get_state)(IComponent *_this, IBStream *state);
    };
    void *table[14];
  } *v;
};

struct IAudioProcessor
{
  union{
    struct{
      TResult (*query_interface)(IAudioProcessor *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IAudioProcessor *_this);
      U32 (*release)(IAudioProcessor *_this);

      TResult (*set_bus_arrangements)(IAudioProcessor *_this, SpeakerArrangement *inputs, S32 num_ins, SpeakerArrangement *outputs, S32 num_outs);
      TResult (*get_bus_arrangements)(IAudioProcessor *_this, BusDirection dir, S32 index, SpeakerArrangement *arr);
      TResult (*can_process_sample_size)(IAudioProcessor *_this, S32 symbolic_sample_size);
      U32 (*get_latency_samples)(IAudioProcessor *_this);
      TResult (*setup_processing)(IAudioProcessor *_this, ProcessSetup *setup);
      TResult (*set_processing)(IAudioProcessor *_this, TBool state);
      TResult (*process)(IAudioProcessor *_this, ProcessData *data);
      U32 (*get_tail_samples)(IAudioProcessor *_this);
    };
    void *table[11];
  } *v;
};

struct IHostApplication
{
  union{
    struct{
      TResult (*query_interface)(IHostApplication *_this, const Tuid iid, void **obj);
      U32 (*add_ref)(IHostApplication *_this);
      U32 (*release)(IHostApplication *_this);

      TResult (*get_name)(IHostApplication *_this, Vst3_String128 name);
      TResult (*create_instance)(IHostApplication *_this, Tuid cid, Tuid iid, void **obj);
    };
    void *table[5];
  } *v;
};

typedef IPluginFactory* (Vst3_GetFactoryProc)(void);
#if OS_WINDOWS
typedef TBool (Vst3_InitModuleProc)(void);
typedef TBool (Vst3_ExitModuleProc)(void);
#elif OS_LINUX
typedef TBool (Vst3_InitModuleProc)(void *dlopen_handle);
typedef TBool (Vst3_ExitModuleProc)(void);
#elif OS_MAC
typedef TBool (Vst3_InitModuleProc)(CFBundleRef bundle_ref);
typedef TBool (Vst3_ExitModuleProc)(void);
#else
#  error unsupported os
#endif

typedef enum Vst3_Interface
{
  Vst3_Interface_FUnknown,
  Vst3_Interface_IPluginFactory,
  Vst3_Interface_IPluginBase,
  Vst3_Interface_IEditController,
  Vst3_Interface_IComponent,
  Vst3_Interface_IAudioProcessor,
  Vst3_Interface_IHostApplication,
  Vst3_Interface_COUNT,
} Vst3_Interface;

typedef struct Vst3_InterfaceSet
{
  U64 count;
  Vst3_Interface const *is;
} Vst3_InterfaceSet;

typedef enum Vst3_Category
{
  Vst3_Category_audio_processor,
  Vst3_Category_component_controller,
  Vst3_Category_Count,
} Vst3_Category;

// -----------------------------------------------------------------------------
// vst3 interface member functions

// IPluginFactory
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

// IComponent
proc inline TResult
IComponent_Initialize(IComponent *_this, FUnknown *context)
{ return(_this->v->initialize(_this, context)); }

proc inline TResult
IComponent_Terminate(IComponent *_this)
{ return(_this->v->terminate(_this)); }

proc inline TResult
IComponent_GetControllerClassID(IComponent *_this, Tuid class_id)
{ return(_this->v->get_controller_class_id(_this, class_id)); }

proc inline TResult
IComponent_QueryInterface(IComponent *_this, Tuid iid, void **obj)
{ return(_this->v->query_interface(_this, iid, obj)); }

proc inline U32
IComponent_Release(IComponent *_this)
{ return(_this->v->release(_this)); }

proc inline TResult
IComponent_GetState(IComponent *_this, IBStream *state)
{ return(_this->v->get_state(_this, state)); }

// IEditController
proc inline TResult
IEditController_Initialize(IEditController *_this, FUnknown *context)
{ return(_this->v->initialize(_this, context)); }

proc inline TResult
IEditController_Terminate(IEditController *_this)
{ return(_this->v->terminate(_this)); }

proc inline U32
IEditController_Release(IEditController *_this)
{ return(_this->v->release(_this)); }

proc inline TResult
IEditController_SetComponentHandler(IEditController *_this, IComponentHandler *handler)
{ return(_this->v->set_component_handler(_this, handler)); }

proc inline TResult
IEditController_SetComponentState(IEditController *_this, IBStream *state)
{ return(_this->v->set_component_state(_this, state)); }

proc inline S32
IEditController_GetParameterCount(IEditController *_this)
{ return(_this->v->get_parameter_count(_this)); }

proc inline TResult
IEditController_GetParameterInfo(IEditController *_this, S32 param_index, ParameterInfo *info)
{ return(_this->v->get_parameter_info(_this, param_index, info)); }

// IAudioProcessor
proc inline U32
IAudioProcessor_Release(IAudioProcessor *_this)
{ return(_this->v->release(_this)); }

// -----------------------------------------------------------------------------
// vst3 constants

// fuids/iids
#define VST3_CLASS_IID(c, dw0, dw1, dw2, dw3) global Tuid const Glue(Glue(Vst3_Interface_, c), _tuid) VST3_UID(dw0, dw1, dw2, dw3)
global Fuid const Vst3_Interface_FUnknown_fuid = VST3_UID(0x00000000, 0x00000000, 0xC0000000, 0x00000046);
//VST3_CLASS_IID(FUnknown, 0x00000000, 0x00000000, 0xC0000000, 0x00000046);
global Fuid const Vst3_Interface_IPluginFactory_fuid = VST3_UID(0x7A4D811C, 0x52114A1F, 0xAED9D2EE, 0x0B43BF9F);
//VST3_CLASS_IID(IPluginFactory, 0x7A4D811C, 0x52114A1F, 0xAED9D2EE, 0x0B43BF9F);
global Fuid const Vst3_Interface_IPluginBase_fuid = VST3_UID(0x22888DDB, 0x156E45AE, 0x8358B348, 0x08190625);
//VST3_CLASS_IID(IPluginBase, 0x22888DDB, 0x156E45AE, 0x8358B348, 0x08190625);
global Fuid const Vst3_Interface_IEditController_fuid = VST3_UID(0xDCD7BBE3, 0x7742448D, 0xA874AACC, 0x979C759E);
//VST3_CLASS_IID(IEditController, 0xDCD7BBE3, 0x7742448D, 0xA874AACC, 0x979C759E);
global Fuid const Vst3_Interface_IComponent_fuid = VST3_UID(0xE831FF31, 0xF2D54301, 0x928EBBEE, 0x25697802);
//VST3_CLASS_IID(IComponent, 0xE831FF31, 0xF2D54301, 0x928EBBEE, 0x25697802);
global Fuid const Vst3_Interface_IAudioProcessor_fuid = VST3_UID(0x42043F99, 0xB7DA453C, 0xA569E79D, 0x9AAEC33D);
//VST3_CLASS_IID(IAudioProcessor, 0x42043F99, 0xB7DA453C, 0xA569E79D, 0x9AAEC33D);
global Fuid const Vst3_Interface_IHostApplication_fuid = VST3_UID(0x58E595CC, 0xDB2D4969, 0x8B6AAF8C, 0x36A664E5);

global Fuid const vst3_fuids[] = {
  [Vst3_Interface_FUnknown]		= Vst3_Interface_FUnknown_fuid,
  [Vst3_Interface_IPluginFactory]	= Vst3_Interface_IPluginFactory_fuid,
  [Vst3_Interface_IPluginBase]		= Vst3_Interface_IPluginBase_fuid,
  [Vst3_Interface_IEditController]	= Vst3_Interface_IEditController_fuid,
  [Vst3_Interface_IComponent]		= Vst3_Interface_IComponent_fuid,
  [Vst3_Interface_IAudioProcessor]	= Vst3_Interface_IAudioProcessor_fuid,
  [Vst3_Interface_IHostApplication]	= Vst3_Interface_IHostApplication_fuid,
};

// categories/interfaces
global String8 const Vst3_Category_audio_processor_string = Str8Lit("Audio Module Class");
global String8 const Vst3_Category_component_controller_string = Str8Lit("Component Controller Class");

global Vst3_Interface const Vst3_Category_audio_processor_interfaces[] = {
  Vst3_Interface_IComponent,
  Vst3_Interface_IAudioProcessor,
};

global Vst3_Interface const Vst3_Category_component_controller_interfaces[] = {
  Vst3_Interface_IEditController,
};

/* #define Vst3_Category_audio_processor_interface_set (Vst3_InterfaceSet){\ */
/*   .count = ArrayCount(Vst3_Category_audio_processor_interfaces),\ */
/*   .is = Vst3_Category_audio_processor_interfaces,\ */
/* } */

/* #define Vst3_Category_component_controller_interface_set = (Vst3_InterfaceSet){\ */
/*   .count = ArrayCount(Vst3_Category_component_controller_interfaces),\ */
/*   .Vst3_Category_component_controller_interfaces,\ */
/* } */

global Vst3_InterfaceSet vst3_category_interfaces[] = {
  [Vst3_Category_audio_processor] = (Vst3_InterfaceSet){
    .count = ArrayCount(Vst3_Category_audio_processor_interfaces),
    .is = Vst3_Category_audio_processor_interfaces,
  },
  [Vst3_Category_component_controller] = (Vst3_InterfaceSet){
    .count = ArrayCount(Vst3_Category_component_controller_interfaces),
    .is = Vst3_Category_component_controller_interfaces,
  },
  // NOTE: nil set
  [Vst3_Category_Count] = (Vst3_InterfaceSet){
    .count = 0,
    .is = 0,
  },
};

// -----------------------------------------------------------------------------
// internal types/state

typedef struct Vst3_PluginImage
{
  Os_Handle handle;

  Vst3_GetFactoryProc *get_factory_proc;
  void *init_proc;
  void *exit_proc;

  IPluginFactory *factory;

  U64 class_count;
  PClassInfo *class_infos;
} Vst3_PluginImage;

typedef struct Vst3_PluginInstance
{
  Vst3_PluginImage *image;

  IComponent *component;
  IEditController *edit_controller;
  IAudioProcessor *audio_processor;

  U64 param_count;
  ParameterInfo *param_infos;
} Vst3_PluginInstance;

typedef struct Vst3_HostApplicationImpl
{
  U32 ref_count;
} Vst3_HostApplicationImpl;

typedef struct Vst3_HostStream
{
  IBStream i;

  U64 size;
  U64 pos;
  U8 *data;

  U32 ref_count;
} Vst3_HostStream;
#define VST3_HOST_STREAM_SIZE_DEFAULT KB(64)

typedef struct Vst3_ComponentHandler
{
  IComponentHandler i;

  U32 ref_count;
} Vst3_ComponentHandler;

typedef struct Vst3_HostState
{
  Arena *arena;

  // TODO: support having multiple plugins loaded
  Os_Handle plugin_handle;
  Vst3_GetFactoryProc *get_factory_proc;
  void *init_proc;
  void *exit_proc;

  U64 plugin_interface_count;
  FUnknown **plugin_interfaces_unknown;
  union {
    struct{
      FUnknown *funknown;
      IPluginFactory *plugin_factory;
      IPluginBase *plugin_base;
      IEditController *edit_controller;
      IComponent *component;
      IAudioProcessor *audio_processor;
    };
    void *unknown[Vst3_Interface_COUNT];
  } plugin_interfaces;

  String8 name_str8;
  String16 name_str16;

  IHostApplication ihost_application;
  Vst3_HostApplicationImpl host_application_impl;

  Vst3_ComponentHandler *component_handler;

  // TODO: multiple images
  Vst3_PluginImage *image;

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

proc Vst3_PluginImage* vst3_host_plugin_load(String8 path_to_plugin_binary);
proc void vst3_host_plugin_unload(Vst3_PluginImage *plugin);

// -----------------------------------------------------------------------------
// init/uninit plugin

proc void vst3_host_plugin_init(Vst3_PluginImage *plugin);
proc void vst3_host_plugin_uninit(Vst3_PluginImage *plugin);

// -----------------------------------------------------------------------------
// instantiate/uninstantiate plugin

proc Vst3_PluginInstance* vst3_host_plugin_instantiate(Vst3_PluginImage *plugin);
proc void vst3_host_plugin_uninstantiate(Vst3_PluginInstance *plugin);

// -----------------------------------------------------------------------------
// helpers

proc Vst3_Category vst3_category_from_string(U8 *category_cstr);
proc Vst3_InterfaceSet vst3_interfaces_for_category(U8 *category_cstr);
proc Fuid vst3_iid_for_interface(Vst3_Interface i);
proc FidString vst3_cid_for_interface(Vst3_Interface i);

Vst3_HostApplicationImpl* vst3_get_host_application_impl(IHostApplication *_this);

// -----------------------------------------------------------------------------
// IHostApplication implementations

proc TResult IHostApplication_QueryInterface(IHostApplication *_this, const Tuid iid, void **obj);
proc U32 IHostApplication_AddRef(IHostApplication *_this);
proc U32 IHostApplication_Release(IHostApplication *_this);

proc TResult IHostApplication_GetName(IHostApplication *_this, Vst3_String128 name);
proc TResult IHostApplication_CreateInstance(IHostApplication *_this, Tuid cid, Tuid iid, void **obj);

// -----------------------------------------------------------------------------
// IBStream implementations

proc Vst3_HostStream* vst3_host_stream_alloc(Arena *arena, U64 size);

proc TResult vst3_host_stream_query_interface(IBStream *_this, const Tuid iid, void **obj);
proc U32 vst3_host_stream_add_ref(IBStream *_this);
proc U32 vst3_host_stream_release(IBStream *_this);

proc TResult vst3_host_stream_read(IBStream *_this, void *buffer, S32 num_bytes, S32 *num_bytes_read);
proc TResult vst3_host_stream_write(IBStream *_this, void *buffer, S32 num_bytes, S32 *num_bytes_written);
proc TResult vst3_host_stream_seek(IBStream *_this, S64 pos, S32 mode, S64 *result);
proc TResult vst3_host_stream_tell(IBStream *_this, S64 *pos);

// -----------------------------------------------------------------------------
// IComponentHandler implementations

proc Vst3_ComponentHandler *vst3_component_handler_alloc(Arena *arena);
proc TResult vst3_component_handler_query_interface(IComponentHandler *_this, const Tuid iid, void **obj);
proc U32 vst3_component_handler_add_ref(IComponentHandler *_this);
proc U32 vst3_component_handler_release(IComponentHandler *_this);
proc TResult vst3_component_handler_begin_edit(IComponentHandler *_this, ParamId id);
proc TResult vst3_component_handler_perform_edit(IComponentHandler *_this, ParamId id, ParamValue value_normalized);
proc TResult vst3_component_handler_end_edit(IComponentHandler *_this, ParamId id);
proc TResult vst3_component_handler_restart_component(IComponentHandler *_this, S32 flags);
