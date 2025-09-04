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

    // NOTE: get input/output format; try to get formats to match
    error = IAudioClient_GetMixFormat(wasapi_state->output_client,
				      &(WAVEFORMATEX*)wasapi_state->sink_fmt);
    WasapiInitCheckError();
    error = IAudioClient_IsFormatSupported(wasapi_state->input_client,
					   AUDCLNT_SHAREMODE_SHARED,
					   (WAVEFORMATEX*)wasapi_state->sink_fmt,
					   &(WAVEFORMATEX*)wasapi_state->source_fmt);
    if(error == S_FALSE) {
      error = IAudioClient_IsFormatSupported(wasapi_state->output_client,
					   AUDCLNT_SHAREMODE_SHARED,
					   (WAVEFORMATEX*)wasapi_state->source_fmt,
					   &(WAVEFORMATEX*)wasapi_state->sink_fmt);
      if(error == S_FALSE) {
	error = S_OK;
      }
    }
    WasapiInitCheckError();

    // NOTE: Initialize input/output streams
    error = IAudioClient_Initialize(wasapi_state->input_client,
				    AUDCLNT_SHAREMODE_SHARED,
				    0,
				    100000,
				    0,
				    (WAVEFORMATEX*)wasapi_state->source_fmt,
				    0);
    WasapiInitCheckError();
    error = IAudioClient_Initialize(wasapi_state->output_client,
				    AUDCLNT_SHAREMODE_SHARED,
				    AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
				    100000,
				    0,
				    (WAVEFORMATEX*)wasapi_state->sink_fmt,
				    0);
    WasapiInitCheckError();

    // NOTE: get input/output buffer counts:
    U32 source_buffer_count = 0;
    error = IAudioClient_GetBufferSize(wasapi_state->input_client, &source_buffer_count);
    WasapiInitCheckError();            
    U32 sink_buffer_count = 0;
    error = IAudioClient_GetBufferSize(wasapi_state->output_client, &sink_buffer_count);
    WasapiInitCheckError();
    wasapi_state->output_buffer_size_in_frames = sink_buffer_count;

    // NOTE: get source/sink
    error = IAudioClient_GetService(wasapi_state->input_client, &IID_IAudioCaptureClient,
				    (void**)&wasapi_state->source);
    WasapiInitCheckErrorPtr(wasapi_state->source);
    error = IAudioClient_GetService(wasapi_state->output_client, &IID_IAudioRenderClient,
				    (void**)&wasapi_state->sink);
    WasapiInitCheckErrorPtr(wasapi_state->sink);    

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
    error = IAudioClient_Start(wasapi_state->input_client);
    WasapiInitCheckError();
    error = IAudioClient_Start(wasapi_state->output_client);
    WasapiInitCheckError();
    wasapi_state->wasapi_process_thread_handle = os_thread_launch(wasapi_process, 0);
    wasapi_state->running = 1;
#endif
    
    result = 1;
  }
  
 wasapi_init_end:
  return(result);
}

proc void
wasapi_process(void *data)
{
  Unused(data);
  HRESULT result = 0;
#if 0
  R32 sample_period = 1.f/(R32)wasapi_state->sink_fmt->Format.nSamplesPerSec;//48000.f;
  R32 samples[109] = {0};
  for(U32 sample_idx = 0; sample_idx < ArrayCount(samples); ++sample_idx) {
    samples[sample_idx] = 0.1f*sinf(440.f * (R32)sample_idx * TAU32 * sample_period);
  }

  U32 sample_idx = 0;
#endif

  for(;;) {
    ArenaTemp scratch = arena_get_scratch(0, 0);
    if(wasapi_state->running) {

      WaitForSingleObject(wasapi_state->callback_event_handle, INFINITE);

      U32 padding_in_frames = 0;
      result = IAudioClient_GetCurrentPadding(wasapi_state->output_client, &padding_in_frames);
      U32 frames_available = wasapi_state->output_buffer_size_in_frames - padding_in_frames;
      U8 *dest_samples = 0;
      result = IAudioRenderClient_GetBuffer(wasapi_state->sink, frames_available, &dest_samples);
      if(result != S_OK) {
	Win32LogError(result);
      }

      U32 next_packet_frames = 0;
      result = IAudioCaptureClient_GetNextPacketSize(wasapi_state->source, &next_packet_frames);
      U8 *src_samples = 0;
      U32 frames_to_read = 0;
      DWORD flags = 0;
      U64 timestamp = 0;
      result = IAudioCaptureClient_GetBuffer(wasapi_state->source,
					     &src_samples, &frames_to_read, &flags, 0, &timestamp);
      if(result != S_OK) {
	Win32LogError(result);
      }

      if(frames_available) {	
	Assert(frames_to_read <= frames_available);

	R32 *in    = arena_push_array_z(scratch.arena, R32, frames_available);
	R32 *out_l = arena_push_array_z(scratch.arena, R32, frames_available);
	R32 *out_r = arena_push_array_z(scratch.arena, R32, frames_available);

	U32 frames_to_read_at_samplerate = frames_to_read;
	{
	  R32 sample_rate_ratio__src_to_dest = ((R32)wasapi_state->sink_fmt->Format.nSamplesPerSec /
						(R32)wasapi_state->source_fmt->Format.nSamplesPerSec);
	  R32 sample_rate_ratio__dest_to_src = 1.f/sample_rate_ratio__src_to_dest;
	  frames_to_read_at_samplerate = (U32)(sample_rate_ratio__src_to_dest * (R32)(frames_to_read - 1) + 0.5f);
	  Assert(frames_to_read_at_samplerate <= frames_available);
	  R32 *src = (R32*)src_samples;
	  R32 *dest = in;
	  // TODO: this is kinda working, but where is the noise coming from?? (bad SR conversion)??
#if 1
	  for(U32 dest_idx = 0; dest_idx < frames_to_read_at_samplerate; ++dest_idx) {

	    R32 src_pos = sample_rate_ratio__dest_to_src * (R32)dest_idx;
	    U32 src_idx = (U32)src_pos;
	    R32 src_frac = (R32)src_pos - src_idx;

	    Assert(src_idx + 1 < frames_to_read);
	    R32 dest_sample = lerp(src[src_idx], src[src_idx + 1], src_frac);
	    dest[dest_idx] = dest_sample;
	  }
#else
	  CopyArray(dest, src, R32, frames_to_read);
#endif
	}      

	global_process_data->input[0] = in;
	global_process_data->input[1] = in;
	global_process_data->output[0] = out_l;
	global_process_data->output[1] = out_r;
	global_process_data->first_midi_msg = 0;
	global_process_data->last_midi_msg = 0;
	global_process_data->midi_msg_count = 0;
	global_process_data->sample_period = 1.f/(R32)wasapi_state->sink_fmt->Format.nSamplesPerSec;
	global_process_data->sample_count = frames_to_read_at_samplerate;

	audio_process(global_process_data);

	IAudioCaptureClient_ReleaseBuffer(wasapi_state->source, frames_to_read);

	{
#if 1
	  R32 *src_l = out_l;
	  R32 *src_r = out_r;
	  R32 *dest = (R32*)dest_samples;
	  for(U32 i = 0; i < frames_to_read_at_samplerate; ++i) {
	    *dest++ = *src_l++;
	    *dest++ = *src_r++;
	  }
#else
	  R32 *dest = (R32*)dest_samples;
	  for(U32 i = 0; i < frames_available; ++i) {
	    *dest++ = samples[sample_idx];
	    *dest++ = samples[sample_idx];
	    ++sample_idx;
	    sample_idx %= ArrayCount(samples);
	  }
#endif
	}

	IAudioRenderClient_ReleaseBuffer(wasapi_state->sink, frames_to_read_at_samplerate, 0);
      }
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
  Assert(global_process_data != 0);
  global_process_data->user_data = data;
}

proc U32
audio_get_sample_rate(void)
{
  U32 result = wasapi_state->sink_fmt->Format.nSamplesPerSec;
  return(result);
}

proc B32
audio_start(void)
{
  // TODO: handle errors
  B32 result = 0;
  HRESULT error = 0;
  Assert(wasapi_state != 0);
  error = IAudioClient_Start(wasapi_state->input_client);
  if(err == S_OK) {
    error = IAudioClient_Start(wasapi_state->output_client);
    if(error == S_OK) {
      wasapi_state->running = 1;
      result = 1;
    }
  }
  return(result);
}

proc B32
audio_stop(void)
{
  B32 result = 0;
  HRESULT error = 0;
  Assert(wasapi_state != 0);
  error = IAudioClient_Stop(wasapi_state->input_client);
  if(err == S_OK) {
    error = IAudioClient_Stop(wasapi_state->output_client);
    if(error == S_OK) {
      wasapi_state->running = 0;
      result = 1;
    }
  }
  return(result);
}
