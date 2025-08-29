proc B32
win32_audio_init(void)
{
#define Win32AudioInitCheckErrorPtr(ptr) if(error != S_OK || ptr == 0) { wasapi_state = 0; arena_release(arena); goto win32_audio_init_end; }
  B32 result = 0;
  HRESULT error = 0;
  
  Arena *arena = arena_alloc();
  wasapi_state = arena_push_struct(arena, Wasapi_State);
  if(wasapi_state) {
    wasapi_state->arena = arena;

    // NOTE: create a device enumerator
    error = CoCreateInstance(&CLSID_MMDeviceEnumerator, 0, CLSCTX_ALL, &IID_IMMDeviceEnumerator,
			     (void**)&wasapi_state->device_enumerator);
    Win32AudioInitCheckErrorPtr(wasapi_state->device_enumerator);

    // NOTE: list endpoints
    error = IMMDeviceEnumerator_EnumAudioEndpoints(wasapi_state->device_enumerator, eCapture, DEVICE_STATEMASK_ALL, &wasapi_state->input_endpoints);
    Win32AudioInitCheckErrorPtr(wasapi_state->input_endpoints);
    error = IMMDeviceEnumerator_EnumAudioEndpoints(wasapi_state->device_enumerator, eRender, DEVICE_STATEMASK_ALL, &wasapi_state->output_endpoints);
    Win32AudioInitCheckErrorPtr(wasapi_state->output_endpoints);

    // NOTE: get default input/output endpoints
    error = IMMDeviceEnumerator_GetDefaultAudioEndpoint(wasapi_state->device_enumerator, eCapture, eMultimedia, &wasapi_state->input_device);
    Win32AudioInitCheckErrorPtr(wasapi_state->input_device);
    error = IMMDeviceEnumerator_GetDefaultAudioEndpoint(wasapi_state->device_enumerator, eRender, eMultimedia, &wasapi_state->output_device);
    Win32AudioInitCheckErrorPtr(wasapi_state->output_device);

    // TODO: get midi device

    // NOTE: get audio clients from input/output devices
    error = IMMDevice_Activate(wasapi_state->input_device, IID_IAudioClient, CLSCTX_ALL, 0,
			       (void**)&wasapi_state->input_client);
    Win32AudioINitCheckErrorPtr(wasapi_state->input_client);
    error = IMMDevice_Activate(wasapi_state->input_device, IID_IAudioClient, CLSCTX_ALL, 0,
			       (void**)&wasapi_state->output_client);
    Win32AudioInitCheckErrorPtr(wasapi_state->output_client);

    // NOTE: add clients to audio session
    /* error = IAudioClient_Initialize(wasapi_state->input_client, */
    /* 				    AUDCLNT_SHAREMODE_SHARED, */
    /* 				    AUDCLNT_STREAMFLAGS_LOOPBACK|AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM|) */
    
    result = 1;
  }
  
 win32_audio_init_end:
  return(result);
}

// TODO: implement
proc B32
audio_init(String8 client_name)
{
  Unused(client_name);
  return(0);
}

proc void
audio_uninit(void)
{
}

proc void
audio_set_user_data(void *data)
{
  Unused(data);  
}

proc U32 audio_get_sample_rate(void)
{
  return(0);
}
