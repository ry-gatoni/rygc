/* #include  "os/core/Windows/win32_audio_defines.h" */

#if LANGUAGE_C
#  ifndef COBJMACROS
#    define COBJMACROS
#  endif
#endif

#include <initguid.h>
#include <combaseapi.h>

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <Audiopolicy.h>

#pragma comment(lib, "ole32.lib")

// NOTE: windows doesn't provide IIDs needed here in headers or libs, so manual definition is needed
DEFINE_GUID(IID_IMMDeviceEnumerator,
	    0xa95664d2, 0x9614, 0x4f35, 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6);
DEFINE_GUID(CLSID_MMDeviceEnumerator,
	    0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E);
DEFINE_GUID(IID_IAudioClient,
	    0x1cb9ad4c, 0xdbfa, 0x4c32, 0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2);
DEFINE_GUID(IID_IAudioRenderClient,
	    0xf294acfc, 0x3146, 0x4483, 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2);
DEFINE_GUID(IID_IAudioCaptureClient,
	    0xc8adbd64, 0xe71e, 0x48a0, 0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17);

// NOTE: wasapi helpers
// TODO: these could be generated and put in a separate file, included here
#if 0
static inline HRESULT
IMMDeviceEnumerator_EnumAudioEndpoints
(IMMDeviceEnumerator *this_, EDataFlow dataFlow, DWORD dwStateMask, IMMDeviceCollection **ppDevices)
{ return(this_->lpVtbl->EnumAudioEndpoints(this_, dataFlow, dwStateMask, ppDevices)); }

static inline HRESULT
IMMDeviceEnumerator_GetDefaultAudioEndpoint
(IMMDeviceEnumerator *this_, EDataFlow dataFlow, ERole role, IMMDevice **ppEndpoint)
{ return(this_->lpVtbl->GetDefaultAudioEndpoint(this_, dataFlow, role, ppEndpoint)); }

static inline HRESULT
IMMDevice_Activate
(IMMDevice *this_, REFIID iid, DWORD dwClsCtx, PROPVARIANT *pActivationParams, void **ppInterface)
{ return(this_->lpVtbl->Activate(this_, iid, dwClsCtx, pActivationParams, ppInterface)); }

static inline HRESULT
IAudioClient_Initialize
(IAudioClient *this_, AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags, REFERENCE_TIME hnsBufferDuration, REFERENCE_TIME hnsPeriodicity, const WAVEFORMATEX *pFormat, LPCGUID AudioSessionGuid)
{ return(this_->lpVtbl->Initialize(this_, ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid)); }

static inline HRESULT
IAudioClient_GetBufferSize
(IAudioClient *this_, UINT32 *pNumBufferFrames)
{ return(this_->lpVtbl->GetBufferSize(this_, pNumBufferFrames)); }

static inline HRESULT
IAudioClient_GetMixFormat
(IAudioClient *this_, WAVEFORMATEX **ppDeviceFormat)
{ return(this_->lpVtbl->GetMixFormat(this_, ppDeviceFormat)); }

static inline HRESULT
IAudioClient_GetService
(IAudioClient *this_, REFIID riid, void **ppv)
{ return(this_->lpVtbl->GetService(this_, riid, ppv)); }
#endif

// NOTE: types
typedef enum Wasapi_PortKind
{
  Wasapi_PortKind_source,
  Wasapi_PortKind_sink,
  Wasapi_PortKind_Count,
} Wasapi_PortKind;

typedef struct Wasapi_Port
{
  IMMDevice *device;
  IAudioClient *client;
  
  WAVEFORMATEX *fmt;

  U32 buffer_count;

  Wasapi_PortKind kind;
  union
  {
    IAudioCaptureClient *source;
    IAudioRenderClient  *sink;
    void                *service;
  };
} Wasapi_Port;

proc B32 wasapi_init_port(Wasapi_PortKind kind, IMMDevice *device, Wasapi_Port *port, IAudioSessionControl **session);
proc Buffer wasapi_port_get_buffer(Wasapi_Port *port, U32 request_frame_count, U64 *timestamp);

typedef struct Wasapi_State
{
  Arena *arena;

  IMMDeviceCollection *input_endpoints;
  IMMDeviceCollection *output_endpoints;

  IMMDeviceEnumerator *device_enumerator;
  IMMDevice *input_device;
  IMMDevice *output_device;
  //IMMDevice *midi_device;

  //U32 audio_client_version;
  IAudioClient *input_client;
  IAudioClient *output_client;

  WAVEFORMATEXTENSIBLE *source_fmt;
  IAudioCaptureClient  *source;

  WAVEFORMATEXTENSIBLE *sink_fmt;
  IAudioRenderClient   *sink;
  
  HANDLE callback_event_handle;

  Wasapi_Port source_port;
  Wasapi_Port sink_port;
  //Wasapi_Port midi_port;

  IAudioSessionControl *session;

  Os_Handle wasapi_process_thread_handle;
  Arena *wasapi_process_arena;

  B32 running;
  //U32 sample_rate;
  U32 output_buffer_size_in_frames;

  U64 input_latency_frames;
  U64 output_latency_frames;

  U64 output_buffer_minimum_frames_to_write;  
} Wasapi_State;

global Wasapi_State *wasapi_state = 0;
global Audio_ProcessData *global_process_data = 0;

proc B32 wasapi_init(String8 client_name);

//proc void wasapi_process(void *data);
proc OsThreadProc(wasapi_process);

