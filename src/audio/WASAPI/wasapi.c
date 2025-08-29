proc B32
wasapi_init_port(Wasapi_PortKind kind, IMMDevice *device, Wasapi_Port *port, IAudioSessionControl **session)
{
  B32 result = 0;
  HRESULT err = 0;

  err = IMMDevice_Activate(device, &IID_IAudioClient, CLSCTX_ALL, 0, (void**)&port->client);
  if(err == S_OK && port->client != 0) {
    err = IAudioClient_GetMixFormat(port->client, &port->fmt);
    if(err == S_OK) {
      err = IAudioClient_Initialize(port->client, AUDCLNT_SHAREMODE_SHARED, 0, 100000, 0,
				      port->fmt, (LPCGUID)session);
      if(err == S_OK) {
	err = IAudioClient_GetBufferSize(port->client, &port->buffer_count);
	if(err == S_OK) {
	  switch(kind) {
	  case Wasapi_PortKind_source:
	    {
	      err = IAudioClient_GetService(port->client, &IID_IAudioCaptureClient,
					    (void**)&port->source);
	    }break;
	  case Wasapi_PortKind_sink:
	    {
	      err = IAudioClient_GetService(port->client, &IID_IAudioRenderClient,
					    (void**)&port->sink);
	    }break;
	  default: { result = 0; } break;
	  }
	  if(err == S_OK && port->service != 0) {
	    result = 1;
	  }
	}
      }
    }
  }

  return(result);
}

// TODO: Is the idea of a "port" a worthwhile generalization for WASAPI?
//       Source and sink are handled quite differently here...
proc Buffer
wasapi_port_get_buffer(Wasapi_Port *port, U32 request_frame_count, U64 *timestamp)
{
  HRESULT err = 0;

  U8 *mem = 0;
  U32 size = 0;

  DWORD flags = 0;
  switch(port->kind) {
  case Wasapi_PortKind_source:
    {
      // TODO: do we need the buffer position?
      U32 frames_to_read = 0;
      err = IAudioCaptureClient_GetBuffer(port->source, &mem, &frames_to_read, &flags, 0, timestamp);
      Unused(flags); // TODO: maybe do something with the flags if they are nonzero?
      if(err == S_OK) {
	size = frames_to_read * port->fmt->nBlockAlign;
      }else {
	mem = 0;
	size = 0;
      }
    }
  case Wasapi_PortKind_sink:
    {
      err = IAudioRenderClient_GetBuffer(port->sink, request_frame_count, &mem);
      if(err == S_OK) {
	size = request_frame_count * port->fmt->nBlockAlign;
      }else {
	mem = 0;
	size = 0;
      }
    }
  default: {} break;
  }

  Buffer result = {0};
  result.size = size;
  result.mem = mem;
  return(result);
}

proc B32
win32_audio_init(void)
{
#define Win32AudioInitCheckError() if(error != S_OK) { wasapi_state = 0; arena_release(arena); goto win32_audio_init_end; }
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
    /* error = wasapi_state->device_enumerator->lpVtbl->EnumAudioEndpoints(wasapi_state->device_enumerator, eCapture, DEVICE_STATEMASK_ALL, &wasapi_state->input_endpoints); */
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
#if 1
    B32 source_init_success = wasapi_init_port(Wasapi_PortKind_source, wasapi_state->input_device,
					       &wasapi_state->source_port, &wasapi_state->session);
    B32 sink_init_success =   wasapi_init_port(Wasapi_PortKind_sink, wasapi_state->output_device,
					       &wasapi_state->sink_port, &wasapi_state->session);
#else
    error = IMMDevice_Activate(wasapi_state->input_device, &IID_IAudioClient, CLSCTX_ALL, 0,
			       (void**)&wasapi_state->input_client);
    Win32AudioInitCheckErrorPtr(wasapi_state->input_client);
    error = IMMDevice_Activate(wasapi_state->input_device, &IID_IAudioClient, CLSCTX_ALL, 0,
			       (void**)&wasapi_state->output_client);
    Win32AudioInitCheckErrorPtr(wasapi_state->output_client);

    // NOTE: add clients to audio session
    U32 capture_buffer_count = 0;
    WAVEFORMATEX *capture_fmt = 0;
    error = IAudioClient_GetMixFormat(wasapi_state->input_client, &capture_fmt);
    Win32AudioInitCheckError();
    error = IAudioClient_Initialize(wasapi_state->input_client,
				    AUDCLNT_SHAREMODE_SHARED,
				    0,
				    100000,
				    0,
				    capture_fmt,
				    (LPCGUID)&wasapi_state->session);
    Win32AudioInitCheckErrorPtr(wasapi_state->session);
    error = IAudioClient_GetBufferSize(wasapi_state->input_client, &capture_buffer_count);
    Win32AudioInitCheckError();
#endif
    
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
