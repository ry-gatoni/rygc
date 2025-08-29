/* #include  "os/core/Windows/win32_audio_defines.h" */

#include <combaseapi.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>

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

  IAudioSessionControl *session;
} Wasapi_State;

global Wasapi_State *wasapi_state = 0;
global Audio_ProcessData *global_process_data = 0;

proc B32 win32_audio_init(void);

