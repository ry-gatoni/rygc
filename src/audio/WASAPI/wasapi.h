/* #include  "os/core/Windows/win32_audio_defines.h" */

#if LANGUAGE_C
#  ifndef COBJMACROS
#    define COBJMACROS
#  endif
#endif

#include <combaseapi.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <Audiopolicy.h>

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
  IMMDevice *midi_device;

  IAudioClient *input_client;
  IAudioClient *output_client;

  Wasapi_Port source_port;
  Wasapi_Port sink_port;
  //Wasapi_Port midi_port;

  IAudioSessionControl *session;

  U32 sample_rate;
} Wasapi_State;

global Wasapi_State *wasapi_state = 0;
global Audio_ProcessData *global_process_data = 0;

proc B32 win32_audio_init(void);

