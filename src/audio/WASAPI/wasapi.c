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
wasapi_init(String8 client_name)
{
#define WasapiLogError(error) do { char* msg; FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, 0); OutputDebugStringA(msg); } while(0)
#define WasapiInitCheckError() if(error != S_OK) { wasapi_state = 0; arena_release(arena); WasapiLogError(error);  goto wasapi_init_end; }
#define WasapiInitCheckErrorPtr(ptr) if(error != S_OK || ptr == 0) { wasapi_state = 0; arena_release(arena); WasapiLogError(error); goto wasapi_init_end; }
  B32 result = 0;
  HRESULT error = 0;
  
  Arena *arena = arena_alloc();
  wasapi_state = arena_push_struct(arena, Wasapi_State);
  if(wasapi_state) {
    wasapi_state->arena = arena;

    // NOTE: create a device enumerator
    error = CoCreateInstance(&CLSID_MMDeviceEnumerator, 0, CLSCTX_ALL, &IID_IMMDeviceEnumerator,
			     (void**)&wasapi_state->device_enumerator);
    WasapiInitCheckErrorPtr(wasapi_state->device_enumerator);

    // NOTE: list endpoints
    error = IMMDeviceEnumerator_EnumAudioEndpoints(wasapi_state->device_enumerator, eCapture, DEVICE_STATEMASK_ALL, &wasapi_state->input_endpoints);
    WasapiInitCheckErrorPtr(wasapi_state->input_endpoints);
    error = IMMDeviceEnumerator_EnumAudioEndpoints(wasapi_state->device_enumerator, eRender, DEVICE_STATEMASK_ALL, &wasapi_state->output_endpoints);
    WasapiInitCheckErrorPtr(wasapi_state->output_endpoints);

    // NOTE: get default input/output endpoints
    error = IMMDeviceEnumerator_GetDefaultAudioEndpoint(wasapi_state->device_enumerator, eCapture, eMultimedia, &wasapi_state->input_device);
    WasapiInitCheckErrorPtr(wasapi_state->input_device);
    error = IMMDeviceEnumerator_GetDefaultAudioEndpoint(wasapi_state->device_enumerator, eRender, eMultimedia, &wasapi_state->output_device);
    WasapiInitCheckErrorPtr(wasapi_state->output_device);

    // TODO: get midi device

    // NOTE: get audio clients from input/output devices
#if 0
    B32 source_init_success = wasapi_init_port(Wasapi_PortKind_source, wasapi_state->input_device,
					       &wasapi_state->source_port, &wasapi_state->session);
    B32 sink_init_success =   wasapi_init_port(Wasapi_PortKind_sink, wasapi_state->output_device,
					       &wasapi_state->sink_port, &wasapi_state->session);
#else
    error = IMMDevice_Activate(wasapi_state->input_device, &IID_IAudioClient, CLSCTX_ALL, 0,
			       (void**)&wasapi_state->input_client);
    WasapiInitCheckErrorPtr(wasapi_state->input_client);
    error = IMMDevice_Activate(wasapi_state->output_device, &IID_IAudioClient, CLSCTX_ALL, 0,
			       (void**)&wasapi_state->output_client);
    WasapiInitCheckErrorPtr(wasapi_state->output_client);

    // NOTE: initialize input/output clients 
    U32 source_buffer_count = 0;
    WAVEFORMATEX *source_fmt = 0;
    error = IAudioClient_GetMixFormat(wasapi_state->input_client, &source_fmt);
    WasapiInitCheckError();
    error = IAudioClient_Initialize(wasapi_state->input_client,
				    AUDCLNT_SHAREMODE_SHARED,
				    0,
				    100000,
				    0,
				    source_fmt,
				    0);
				    //(LPCGUID)&wasapi_state->session);
    WasapiInitCheckError();
    error = IAudioClient_GetBufferSize(wasapi_state->input_client, &source_buffer_count);
    WasapiInitCheckError();
    error = IAudioClient_GetService(wasapi_state->input_client, &IID_IAudioCaptureClient,
				    (void**)&wasapi_state->source);
    WasapiInitCheckErrorPtr(wasapi_state->source);

    U32 sink_buffer_count = 0;
    WAVEFORMATEX *sink_fmt = 0;    
    error = IAudioClient_GetMixFormat(wasapi_state->output_client, &sink_fmt);
    WasapiInitCheckError();
    error = IAudioClient_Initialize(wasapi_state->output_client,
				    AUDCLNT_SHAREMODE_SHARED,
				    AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
				    100000,
				    0,
				    sink_fmt,
				    0);
				    //(LPCGUID)&wasapi_state->session);
    WasapiInitCheckError();
    error = IAudioClient_GetBufferSize(wasapi_state->output_client, &sink_buffer_count);
    WasapiInitCheckError();
    error = IAudioClient_GetService(wasapi_state->output_client, &IID_IAudioRenderClient,
				    (void**)&wasapi_state->sink);
    WasapiInitCheckErrorPtr(wasapi_state->sink);
    wasapi_state->output_buffer_size_in_frames = sink_buffer_count;

    // NOTE: set up callback event
    ArenaTemp scratch = arena_get_scratch(0, 0);
    String16 event_name = str16_from_str8(scratch.arena, client_name);
    wasapi_state->callback_event_handle = CreateEventW(0, 0, 0, (LPCWSTR)event_name.string);
    error = IAudioClient_SetEventHandle(wasapi_state->output_client,
					wasapi_state->callback_event_handle);    
    arena_release_scratch(scratch);
    WasapiInitCheckError();

    // TODO: make sure source and sink have compatible formats

    // NOTE: launch process thread
    global_process_data = arena_push_struct(arena, Audio_ProcessData);
#endif
    
    result = 1;
  }
  
 wasapi_init_end:
  return(result);
}

proc void
wasapi_process(void)
{
  HRESULT result = 0;
  for(;;) {
    ArenaTemp scratch = arena_get_scratch(0, 0);
    if(wasapi_state->running) {

#if 1
      WaitForSingleObject(wasapi_state->callback_event_handle, INFINITE);

      U32 padding_in_frames = 0;
      result = IAudioClient_GetCurrentPadding(wasapi_state->output_client, &padding_in_frames);
      U32 frames_available = wasapi_state->output_buffer_size_in_frames - padding_in_frames;
      U8 *dest_samples = 0;
      result = IAudioRenderClient_GetBuffer(wasapi_state->sink, frames_available, &dest_samples);

      U32 next_packet_frames = 0;
      result = IAudioCaptureClient_GetNextPacketSize(wasapi_state->source, &next_packet_frames);
      U8 *src_samples = 0;
      U32 frames_to_read = 0;
      DWORD flags = 0;
      U64 timestamp = 0;
      result = IAudioCaptureClient_GetBuffer(wasapi_state->source,
					     &src_samples, &frames_to_read, &flags, 0, &timestamp);
#else 

      // NOTE: read from source
      U32 in_cap = 4096;
      R32 *in_l = arena_push_array(scratch.arena, R32, in_cap);
      R32 *in_r = arena_push_array(scratch.arena, R32, in_cap);
      U32 in_count = 0;

      // TODO: we need to be sure we receive input in float format, and don't have to convert
      U8 *samples = 0;
      U32 frame_count = 0;
      DWORD flags = 0;
      U64 timestamp = 0;
      U64 *ts_ptr = &timestamp;
      for(result = IAudioCaptureClient_GetBuffer(wasapi_state->source,
						 &samples, &frame_count, &flags, 0, ts_ptr);
	  result != AUDCNT_S_EMPTY;
	  result = IAudioCaptureClient_ReleaseBuffer(wasapi_state->source, frames_count))
	{
	  ts_ptr = 0;
	  Assert(in_count < in_cap);
	  R32 *src = (R32*)samples;
	  R32 *dest_l = in_l + in_count;
	  R32 *dest_r = in_r + in_count;
	  for(U32 frame_idx = 0; frame_idx < frame_count; ++frame_idx) {
	    *dest_l++ = *src++;
	    *dest_r++ = *src++;
	  }
	  in_count += frame_count;
	}

      // NOTE: write to dest
      U32 padding_in_frames = 0;
      error = IAudioClient_GetCurrentPadding(wasapi_state->output_client, padding);
      U32 frames_available = wasapi_state->output_buffer_size_in_frames - padding_in_frames;
      U32 frames_to_write = Min(in_count, frames_available);
      U8 *dest_samples = 0;
      error = IAudioRenderClient_GetBuffer(wasapi_state->sink, frames_to_write, &dest_samples);
#endif
    }else {
      // TODO: sleep
    }
    arena_release_scratch(scratch);
  }
}

// TODO: implement
proc B32
audio_init(String8 client_name)
{
  B32 result = wasapi_init(client_name);
  return(result);
}

proc void
audio_uninit(void)
{
  // TODO: implement
}

proc void
audio_set_user_data(void *data)
{
  //Unused(data);
  Assert(global_process_data != 0);
  global_process_data->user_data = data;
}

proc U32 audio_get_sample_rate(void)
{
  // TODO: implement
  return(0);
}

proc B32
audio_start(void)
{
  wasapi_state->running = 1;
  // TODO: start source/sink
  return(0);
}

proc B32
audio_stop(void)
{
  wasapi_state->running = 0;
  // TODO: stop source/sink
  return(0);
}
