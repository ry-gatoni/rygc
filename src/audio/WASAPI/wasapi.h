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

#include <mmeapi.h>

#pragma comment(lib, "Winmm.lib")

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

/* typedef struct Win32_MidiData */
/* { */
/*   U32 timestamp; */
/*   union { */
/*     struct { */
/*       U8 opcode_channel; */
/*       U8 data_1; */
/*       U8 data_2; */
/*       U8 _reserved_; */
/*     }; */
/*     U32 data; */
/*   }; */
/* } Win32_MidiData; */

typedef struct Wasapi_State
{
  Arena *arena;

  IMMDeviceCollection *input_endpoints;
  IMMDeviceCollection *output_endpoints;

  IMMDeviceEnumerator *device_enumerator;
  IMMDevice *input_device;
  IMMDevice *output_device;

  //U32 audio_client_version;
  IAudioClient *input_client;
  IAudioClient *output_client;

  WAVEFORMATEXTENSIBLE *source_fmt;
  IAudioCaptureClient  *source;

  WAVEFORMATEXTENSIBLE *sink_fmt;
  IAudioRenderClient   *sink;

  union {
    struct {
      HANDLE render_callback_event_handle;
      HANDLE capture_callback_event_handle;
      //HANDLE midi_callback_event_handle;
    };
    HANDLE callback_event_handles[2];
  };
  
  /* IAudioSessionControl *session; */

  String8List midi_input_device_names;
  HMIDIIN midi_input_handle;  

  Audio_MidiMessage *first_midi_msg;
  Audio_MidiMessage *last_midi_msg;
  U64 midi_msg_count;

  Os_Handle wasapi_process_thread_handle;
  Arena *wasapi_process_arena;

  B32 running;
  //U32 sample_rate;
  U32 input_buffer_frame_count;
  U32 output_buffer_frame_count;

  R32 samplerate_conversion_factor__dest_from_src;
  R32 samplerate_conversion_factor__src_from_dest;

  U64 input_latency_frames;
  U64 output_latency_frames;

  U64 output_buffer_minimum_frames_to_write;

  Os_RingBuffer shared_buffer;
  R32 *shared_buffer_samples;
  U32 shared_buffer_sample_capacity;
  U32 shared_buffer_read_idx;
  U32 shared_buffer_write_idx;
  U32 shared_buffer_last_write_size_in_frames;

} Wasapi_State;

global Wasapi_State *wasapi_state = 0;
global Audio_ProcessData *global_process_data = 0;

proc B32 wasapi_midi_init(void);
proc B32 wasapi_init(String8 client_name);

//#define WinMidiInProc(name) void CALLBACK (name)(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

//proc void wasapi_process(void *data);
proc OsThreadProc(wasapi_process);
//proc WinMidiInProc(wasapi_midi_in_process);


