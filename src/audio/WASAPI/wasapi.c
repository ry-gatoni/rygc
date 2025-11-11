/** TODO:
 - get midi message sample index from timestamp
 - after using the pitch bend wheel, the sound fluctuates in pitch again
   a second or two later even though no new interaction happened, as if not all
   the messages are being cleared from the queue, or are somehow getting back on
   without new messages being sent.
 */

proc void
wasapi_midi_in_process(HMIDIIN midi, UINT msg, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2)
{
  Arena *arena = wasapi_state->arena;
  switch(msg)
  {
    case MIM_DATA:
    {
      while(AtomicCompareAndSwap(&wasapi_state->midi_lock, 0, 1)) {}
      
      DWORD timestamp = (DWORD)param2;

      DWORD data = (DWORD)param1;
      U8 opcode  = (U8)((data >> 0*8) & 0xF0);
      U8 channel = (U8)((data >> 0*8) & 0x0F);
      U8 data1   = (U8)((data >> 1*8) & 0xFF);
      U8 data2   = (U8)((data >> 2*8) & 0xFF);

      PushBuffer data_buffer = push_buffer_alloc(arena, 2);
      *buf_push_struct(&data_buffer, U8) = data1;
      *buf_push_struct(&data_buffer, U8) = data2;

      Audio_MidiMessage *new_midi_msg = wasapi_state->midi_msg_freelist;
      if(new_midi_msg != 0) {
	SLLStackPop(wasapi_state->midi_msg_freelist);
	ZeroStruct(new_midi_msg);
      }else {
	new_midi_msg = arena_push_struct(arena, Audio_MidiMessage);
      }
      Assert(new_midi_msg != 0);

      // TODO: translate the timestamp to a sample index somehow
      new_midi_msg->opcode = opcode;
      new_midi_msg->channel = channel;
      new_midi_msg->body = buf_from_push_buffer(data_buffer);

      SLLQueuePush(wasapi_state->first_midi_msg, wasapi_state->last_midi_msg, new_midi_msg);
      ++wasapi_state->midi_msg_count;

      while(AtomicCompareAndSwap(&wasapi_state->midi_lock, 1, 0)) {}
    }break;

    // TODO: handle sysex, errors
    default:
    {} break;
  }
}

proc B32
wasapi_midi_init(Wasapi_State *wasapi)
{
  MMRESULT result = 0;
  U64 pos = arena_pos(wasapi->arena);

  U32 midi_in_count = midiInGetNumDevs();
  String8List midi_input_device_names = {0};
  for(U32 midi_in_device_id = 0; midi_in_device_id < midi_in_count; ++midi_in_device_id)
  {
    MIDIINCAPS midi_in_capabilities = {0};
    result = midiInGetDevCaps(midi_in_device_id, &midi_in_capabilities, sizeof(midi_in_capabilities));
    if(result != MMSYSERR_NOERROR) goto wasapi_midi_in_get_caps_failure;

    str8_list_push(wasapi->arena, &midi_input_device_names, Str8Cstr(midi_in_capabilities.szPname));
  }

  HMIDIIN midi_input_handle = 0;
  U32 midi_device_id = 1; // TODO: make this variable/overrideable
  result = midiInOpen(&midi_input_handle, midi_device_id, (DWORD_PTR)wasapi_midi_in_process, 0, CALLBACK_FUNCTION);
  if(result != MMSYSERR_NOERROR) goto wasapi_midi_in_open_failure;

  result = midiInStart(midi_input_handle);
  if(result != MMSYSERR_NOERROR) goto wasapi_midi_in_start_failure;

  if(0) {
  wasapi_midi_in_start_failure:
  wasapi_midi_in_open_failure:
  wasapi_midi_in_get_caps_failure:
    arena_pop_to(wasapi->arena, pos);
    return(0);
  }

  wasapi->midi_input_device_names = midi_input_device_names;
  wasapi->midi_input_handle = midi_input_handle;
  return(1);
}

proc B32
wasapi_init(String8 client_name)
{
  #if 1

  B32 reuslt = 0;
  HRESULT error = 0;

  U64 latency_hundred_ns = 100000;

  Arena *arena = arena_alloc();
  Wasapi_State *wasapi = arena_push_struct(arena, Wasapi_State);
  if(wasapi == 0) goto wasapi_alloc_failure;
  wasapi->arena = arena;

  IMMDeviceEnumerator *device_enumerator = 0;
  error = CoCreateInstance(&CLSID_MMDeviceEnumerator, 0, CLSCTX_ALL, &IID_IMMDeviceEnumerator,
			   (void**)&device_enumerator);
  if(error != S_OK) goto wasapi_create_device_enumerator_failure;
  Assert(device_enumerator != 0);

  IMMDeviceCollection *input_endpoints = 0;
  IMMDeviceCollection *output_endpoints = 0;
  error = IMMDeviceEnumerator_EnumAudioEndpoints(device_enumerator, eCapture, DEVICE_STATEMASK_ALL,
						 &input_endpoints);
  if(error != S_OK) goto wasapi_enum_input_endpoints_failure;
  error = IMMDeviceEnumerator_EnumAudioEndpoints(device_enumerator, eRender, DEVICE_STATEMASK_ALL,
						 &output_endpoints);
  if(error != S_OK) goto wasapi_enum_output_endpoints_failure;
  Assert(input_endpoints != 0);
  Assert(output_endpoints != 0);

  IMMDevice *input_device = 0;
  IMMDevice *output_device = 0;
  error = IMMDeviceEnumerator_GetDefaultAudioEndpoint(device_enumerator, eCapture, eMultimedia,
						      &input_device);
  if(error != S_OK) goto wasapi_get_default_input_device_failure;
  error = IMMDeviceEnumerator_GetDefaultAudioEndpoint(device_enumerator, eRender, eMultimedia,
						      &output_device);
  if(error != S_OK) goto wasapi_get_default_output_device_failure;
  Assert(input_device != 0);
  Assert(output_device != 0);

  IAudioClient *input_client = 0;
  IAudioClient *output_client = 0;
  error = IMMDevice_Activate(input_device, &IID_IAudioClient, CLSCTX_ALL, 0,
			     (void**)&input_client);
  if(error != S_OK) goto wasapi_activate_input_client_failure;
  error = IMMDevice_Activate(output_device, &IID_IAudioClient, CLSCTX_ALL, 0,
			     (void**)&output_client);
  if(error != S_OK) goto wasapi_activate_output_client_failure;
  Assert(input_client != 0);
  Assert(output_client != 0);

  WAVEFORMATEXTENSIBLE *sink_fmt = 0;
  WAVEFORMATEXTENSIBLE *source_fmt = 0;
  {
    // NOTE: we're trying to get compatible source and sink formats. we first
    //       get the default sink format, then check if the same format is
    //       supported by the source. if not, we check if the closest-matching
    //       source format is supported by the sink. if not, we have no choice
    //       but to deal with the discrepancy
    error = IAudioClient_GetMixFormat(output_client, &(WAVEFORMATEX*)sink_fmt);
    if(error != S_OK) goto wasapi_get_sink_fmt_failure;
    error = IAudioClient_IsFormatSupported(input_client, AUDCLNT_SHAREMODE_SHARED,
					   (WAVEFORMATEX*)sink_fmt,
					   &(WAVEFORMATEX*)source_fmt);
    if(error == S_OK) {
      source_fmt = sink_fmt;
    }else if(error == S_FALSE) {
      error = IAudioClient_IsFormatSupported(output_client, AUDCLNT_SHAREMODE_SHARED,
					     (WAVEFORMATEX*)source_fmt,
					     &(WAVEFORMATEX*)sink_fmt);
      if(error == S_OK) {
	sink_fmt = source_fmt;
      }else if(error == S_FALSE) {
	// TODO: log somehow that there's a sample format discrepancy to deal with
	error = S_OK;
      }else if(error != S_OK) {
	goto wasapi_get_matching_sink_fmt_failure;
      }
    }else if(error != S_OK) {
      goto wasapi_get_matching_source_fmt_failure;
    }
  }
  Assert(sink_fmt != 0);
  Assert(source_fmt != 0);

  error = IAudioClient_Initialize(input_client,
				  AUDCLNT_SHAREMODE_SHARED,
				  AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
				  latency_hundred_ns,
				  0,
				  (WAVEFORMATEX*)source_fmt,
				  0);
  if(error != S_OK) goto wasapi_initialize_input_client_failure;

  error = IAudioClient_Initialize(output_client,
				  AUDCLNT_SHAREMODE_SHARED,
				  AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
				  latency_hundred_ns,
				  0,
				  (WAVEFORMATEX*)sink_fmt,
				  0);
  if(error != S_OK) goto wasapi_initialize_output_client_failure;

  U64 input_latency_frames = latency_hundred_ns * source_fmt->Format.nSamplesPerSec / 10000000;
  U64 output_latency_frames = latency_hundred_ns * sink_fmt->Format.nSamplesPerSec / 10000000;

  U32 source_buffer_count = 0;
  U32 sink_buffer_count = 0;
  error = IAudioClient_GetBufferSize(input_client, &source_buffer_count);
  if(error != S_OK) goto wasapi_get_source_buffer_count_failure;
  error = IAudioClient_GetBufferSize(output_client, &sink_buffer_count);
  if(error != S_OK) goto wasapi_get_sink_buffer_count_failure;

  R32 samplerate_conversion_factor__dest_from_src = ((R32)sink_fmt->Format.nSamplesPerSec/
						     (R32)source_fmt->Format.nSamplesPerSec);
  R32 samplerate_conversion_factor__src_from_dest = ((R32)source_fmt->Format.nSamplesPerSec/
						     (R32)sink_fmt->Format.nSamplesPerSec);

  REFERENCE_TIME output_latency__hundred_ns = 0;
  error = IAudioClient_GetStreamLatency(output_client, &output_latency__hundred_ns);
  if(error != S_OK) goto wasapi_get_output_stream_latency_failure;
  U64 output_buffer_minimum_frames_to_write =
    output_latency__hundred_ns * sink_fmt->Format.nSamplesPerSec / 10000000;
  
  REFERENCE_TIME output_period__default = 0;
  REFERENCE_TIME output_period__minimum = 0;
  error = IAudioClient_GetDevicePeriod(output_client,
				       &output_period__default, &output_period__minimum);
  if(error != S_OK) goto wasapi_get_output_period_failure;

  IAudioCaptureClient *source = 0;
  IAudioRenderClient *sink = 0;
  {
    error = IAudioClient_GetService(input_client, &IID_IAudioCaptureClient,
				    (void**)&source);
    if(error != S_OK) goto wasapi_get_capture_client_failure;
    error = IAudioClient_GetService(output_client, &IID_IAudioRenderClient,
				    (void**)&sink);
    if(error != S_OK) goto wasapi_get_render_client_failure;
  }
  Assert(source != 0);
  Assert(sink != 0);

  ArenaTemp scratch = arena_get_scratch(0, 0);
  String8 render_event_name_8 = str8_concat(scratch.arena,
					    client_name, Str8Lit("render"), Str8Lit("_"));
  String8 capture_event_name_8 = str8_concat(scratch.arena,
					     client_name, Str8Lit("capture"), Str8Lit("_"));
  String16 render_event_name = str16_from_str8(scratch.arena, render_event_name_8);
  String16 capture_event_name = str16_from_str8(scratch.arena, capture_event_name_8);

  HANDLE render_callback_event_handle = CreateEventW(0 ,0, 0, (LPCWSTR)render_event_name.string);
  HANDLE capture_callback_event_handle = CreateEventW(0 ,0, 0, (LPCWSTR)capture_event_name.string);
  arena_release_scratch(scratch);

  error = IAudioClient_SetEventHandle(output_client, render_callback_event_handle);
  if(error != S_OK) goto wasapi_set_render_event_handle_failure;
  error = IAudioClient_SetEventHandle(input_client, capture_callback_event_handle);
  if(error != S_OK) goto wasapi_set_capture_event_handle_failure;

  // NOTE: shared ring buffer setup
  Os_RingBuffer shared_buffer = os_ring_buffer_alloc(2048*sizeof(R32));
  R32 *shared_buffer_samples = (R32*)shared_buffer.mem;
  U32 shared_buffer_sample_capacity = (U32)(shared_buffer.size / sizeof(R32));
  U32 shared_buffer_read_idx = 0;
  U32 shared_buffer_write_idx = 0;
  U32 shared_buffer_last_write_size_in_frames = 0;

  // NOTE: launch process thread
  global_process_data = arena_push_struct(arena, Audio_ProcessData);
  error = IAudioClient_Start(input_client);
  if(error != S_OK) goto wasapi_input_start_failure;
  error = IAudioClient_Start(output_client);
  if(error != S_OK) goto wasapi_output_start_failure;
  
  Arena *process_arena = arena_alloc();
  Os_Handle process_thread_handle = os_thread_launch(wasapi_process, 0);

  if(0) {
    // TODO: put cleanup stuff in here
  wasapi_output_start_failure:
  wasapi_input_start_failure:

  wasapi_set_capture_event_handle_failure:
  wasapi_set_render_event_handle_failure:

  wasapi_get_render_client_failure:
  wasapi_get_capture_client_failure:

  wasapi_get_output_period_failure:

  wasapi_get_output_stream_latency_failure:

  wasapi_get_sink_buffer_count_failure:
  wasapi_get_source_buffer_count_failure:

  wasapi_initialize_output_client_failure:
  wasapi_initialize_input_client_failure:
    
  wasapi_get_matching_sink_fmt_failure:
  wasapi_get_matching_source_fmt_failure:
    
  wasapi_get_sink_fmt_failure:
    
  wasapi_activate_output_client_failure:
  wasapi_activate_input_client_failure:
    
  wasapi_get_default_output_device_failure:
  wasapi_get_default_input_device_failure:
    
  wasapi_enum_output_endpoints_failure:
  wasapi_enum_input_endpoints_failure:
    
  wasapi_create_device_enumerator_failure:
    
  wasapi_alloc_failure:
    return(0);
  }

  wasapi->device_enumerator = device_enumerator;
  wasapi->input_endpoints = input_endpoints;
  wasapi->output_endpoints = output_endpoints;
  wasapi->input_device = input_device;
  wasapi->output_device = output_device;
  wasapi->input_client = input_client;
  wasapi->output_client = output_client;
  wasapi->sink_fmt = sink_fmt;
  wasapi->source_fmt = source_fmt;
  wasapi->sink = sink;
  wasapi->source = source;
  wasapi->input_latency_frames = input_latency_frames;
  wasapi->output_latency_frames = output_latency_frames;
  wasapi->input_buffer_frame_count = source_buffer_count;
  wasapi->output_buffer_frame_count = sink_buffer_count;
  wasapi->samplerate_conversion_factor__dest_from_src = samplerate_conversion_factor__dest_from_src;
  wasapi->samplerate_conversion_factor__src_from_dest = samplerate_conversion_factor__src_from_dest;
  /* wasapi->output_lantency__hundred_ns = output_lantency__hundred_ns; */
  /* wasapi->output_period__default = output_period__default; */
  /* wasapi->output_period__minimum = output_period__minimum; */
  wasapi->output_buffer_minimum_frames_to_write = output_buffer_minimum_frames_to_write;
  wasapi->render_callback_event_handle = render_callback_event_handle;
  wasapi->capture_callback_event_handle = capture_callback_event_handle;
  wasapi->shared_buffer = shared_buffer;
  wasapi->shared_buffer_samples = shared_buffer_samples;
  wasapi->shared_buffer_sample_capacity = shared_buffer_sample_capacity;
  wasapi->shared_buffer_read_idx = shared_buffer_read_idx;
  wasapi->shared_buffer_write_idx = shared_buffer_write_idx;
  wasapi->shared_buffer_last_write_size_in_frames = shared_buffer_last_write_size_in_frames;
  wasapi->wasapi_process_arena = process_arena;
  wasapi->wasapi_process_thread_handle = process_thread_handle;
  
  wasapi_state = wasapi;
  wasapi_midi_init(wasapi_state);
  wasapi_state->running = 1;
  return(1);

  #else
  
#define WasapiLogError(error) do { char* msg; FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM, 0, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, 0); OutputDebugStringA(msg); } while(0)
#define WasapiInitCheckError() if(error != S_OK) { wasapi_state = 0; arena_release(arena); WasapiLogError(error);  goto wasapi_init_end; }
#define WasapiInitCheckErrorPtr(ptr) if(error != S_OK || ptr == 0) { wasapi_state = 0; arena_release(arena); WasapiLogError(error); goto wasapi_init_end; }
  B32 result = 0;
  HRESULT error = 0;

  U64 latency_hundred_ns = 100000;

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
				    AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
				    latency_hundred_ns,
				    0,
				    (WAVEFORMATEX*)wasapi_state->source_fmt,
				    0);
    WasapiInitCheckError();
    error = IAudioClient_Initialize(wasapi_state->output_client,
				    AUDCLNT_SHAREMODE_SHARED,
				    AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
				    latency_hundred_ns,
				    0,
				    (WAVEFORMATEX*)wasapi_state->sink_fmt,
				    0);
    WasapiInitCheckError();
    wasapi_state->input_latency_frames =
      latency_hundred_ns * wasapi_state->source_fmt->Format.nSamplesPerSec / 10000000;
    wasapi_state->output_latency_frames =
      latency_hundred_ns * wasapi_state->sink_fmt->Format.nSamplesPerSec / 10000000;

    // NOTE: get input/output buffer counts:
    U32 source_buffer_count = 0;
    error = IAudioClient_GetBufferSize(wasapi_state->input_client, &source_buffer_count);
    WasapiInitCheckError();
    wasapi_state->input_buffer_frame_count = source_buffer_count;
    
    U32 sink_buffer_count = 0;
    error = IAudioClient_GetBufferSize(wasapi_state->output_client, &sink_buffer_count);
    WasapiInitCheckError();
    wasapi_state->output_buffer_frame_count = sink_buffer_count;

    wasapi_state->samplerate_conversion_factor__dest_from_src =
      ((R32)wasapi_state->sink_fmt->Format.nSamplesPerSec /
       (R32)wasapi_state->source_fmt->Format.nSamplesPerSec);
    wasapi_state->samplerate_conversion_factor__src_from_dest =
      ((R32)wasapi_state->source_fmt->Format.nSamplesPerSec /
       (R32)wasapi_state->sink_fmt->Format.nSamplesPerSec);

    // NOTE: get output latency to determine the minimum write size
    REFERENCE_TIME output_latency__hundred_ns = 0;
    error = IAudioClient_GetStreamLatency(wasapi_state->output_client, &output_latency__hundred_ns);
    WasapiInitCheckError();
    wasapi_state->output_buffer_minimum_frames_to_write =
      output_latency__hundred_ns * wasapi_state->sink_fmt->Format.nSamplesPerSec / 10000000;

    // NOTE: get output device period
    REFERENCE_TIME output_period__default = 0;
    REFERENCE_TIME output_period__minimum = 0;
    error = IAudioClient_GetDevicePeriod(wasapi_state->output_client,
					 &output_period__default, &output_period__minimum);
    WasapiInitCheckError();

    // NOTE: get source/sink
    error = IAudioClient_GetService(wasapi_state->input_client, &IID_IAudioCaptureClient,
				    (void**)&wasapi_state->source);
    WasapiInitCheckErrorPtr(wasapi_state->source);
    error = IAudioClient_GetService(wasapi_state->output_client, &IID_IAudioRenderClient,
				    (void**)&wasapi_state->sink);
    WasapiInitCheckErrorPtr(wasapi_state->sink);    

    // NOTE: set up callback events
    ArenaTemp scratch = arena_get_scratch(0, 0);
    String8 render_event_name_8 = str8_concat(scratch.arena,
					      client_name, Str8Lit("render"), Str8Lit("_"));
    String8 capture_event_name_8 = str8_concat(scratch.arena,
					       client_name, Str8Lit("capture"), Str8Lit("_"));
    String16 render_event_name = str16_from_str8(scratch.arena, render_event_name_8);
    String16 capture_event_name = str16_from_str8(scratch.arena, capture_event_name_8);    
    wasapi_state->render_callback_event_handle = CreateEventW(0, 0, 0,
							      (LPCWSTR)render_event_name.string);
    wasapi_state->capture_callback_event_handle = CreateEventW(0, 0, 0,
							       (LPCWSTR)capture_event_name.string);
    error = IAudioClient_SetEventHandle(wasapi_state->output_client,
					wasapi_state->render_callback_event_handle);
    error = IAudioClient_SetEventHandle(wasapi_state->input_client,
					wasapi_state->capture_callback_event_handle);
    arena_release_scratch(scratch);
    WasapiInitCheckError();

    // NOTE: initialize shared buffer
    wasapi_state->shared_buffer = os_ring_buffer_alloc(2048*sizeof(R32));
    wasapi_state->shared_buffer_samples = (R32*)wasapi_state->shared_buffer.mem;
    wasapi_state->shared_buffer_sample_capacity = (U32)(wasapi_state->shared_buffer.size/sizeof(R32));
    wasapi_state->shared_buffer_read_idx = 0;
    wasapi_state->shared_buffer_write_idx = 0;
    wasapi_state->shared_buffer_last_write_size_in_frames = 0;

#if 0
  U32 test_idx = 2;
  input_ring_buffer_samples[test_idx] = 2.f;
  Assert(input_ring_buffer_samples[test_idx] ==
	 input_ring_buffer_samples[test_idx + input_ring_buffer_sample_count]);
#endif

    // TODO: make sure source and sink have compatible formats

    // NOTE: launch process thread
    global_process_data = arena_push_struct(arena, Audio_ProcessData);
    error = IAudioClient_Start(wasapi_state->input_client);
    WasapiInitCheckError();
    error = IAudioClient_Start(wasapi_state->output_client);
    WasapiInitCheckError();
    wasapi_state->wasapi_process_arena = arena_alloc();
    wasapi_state->wasapi_process_thread_handle = os_thread_launch(wasapi_process, 0);
    wasapi_state->running = 1;
#endif
    
    result = 1;
  }
  
 wasapi_init_end:
  return(result);
#endif
}

proc void
wasapi_process(void *data)
{
  Unused(data);
  HRESULT result = 0;
#if 1
  R32 sample_period = 1.f/(R32)wasapi_state->sink_fmt->Format.nSamplesPerSec;
  R32 samples[109] = {0};
  for(U32 sample_idx = 0; sample_idx < ArrayCount(samples); ++sample_idx) {
    samples[sample_idx] = 0.1f*sinf(440.f * (R32)sample_idx * TAU32 * sample_period);
  }

  U32 sample_idx = 0;
#endif

#if 1
  U32 input_sample_rate = wasapi_state->source_fmt->Format.nSamplesPerSec;
  U32 input_channel_count = wasapi_state->source_fmt->Format.nChannels;
  U32 input_sample_format = WavSampleKind_R32; // TODO: actually do the sample foramt translation
  
  U32 output_sample_rate = wasapi_state->sink_fmt->Format.nSamplesPerSec;
  U32 output_channel_count = wasapi_state->sink_fmt->Format.nChannels;
  U32 output_sample_format = WavSampleKind_R32; // TODO: actually do the sample foramt translation
  WavWriter *wav_writer_in = begin_wav(input_sample_rate, input_channel_count, input_sample_format);
  WavWriter *wav_writer_out = begin_wav(output_sample_rate, output_channel_count, output_sample_format);

  U32 chunk_count = 0;
  U32 chunk_cap = 500;
  B32 wav_written = 0;
#endif  

  Arena *process_arena = wasapi_state->wasapi_process_arena;
  U32 capture_event_count = 0;
  U32 render_event_count = 0;

  for(;;) {
    if(wasapi_state->running) {

      //WaitForSingleObject(wasapi_state->callback_event_handle, INFINITE);
      DWORD wait_result = WaitForMultipleObjects(ArrayCount(wasapi_state->callback_event_handles),
						 wasapi_state->callback_event_handles,
						 FALSE,
						 INFINITE);
      U32 signaled_handle_idx = wait_result - WAIT_OBJECT_0;
      HANDLE signaled_handle = wasapi_state->callback_event_handles[signaled_handle_idx];
      if(signaled_handle == wasapi_state->capture_callback_event_handle) {
	++capture_event_count;
	if(wasapi_state->shared_buffer_last_write_size_in_frames != 0) {
	  OutputDebugStringA("input: no read since last write\n");
	}
	  
	// NOTE: get the number of frames available in the input buffer
	U32 input_frames_available = 0;
	result = IAudioClient_GetCurrentPadding(wasapi_state->input_client, &input_frames_available);
	if(result != S_OK) { Win32LogError(result); }

	// NOTE: copy all samples from the input buffer to the shared buffer
	U32 frames_read = 0;
	while(frames_read < input_frames_available) {
	  U8 *src_samples = 0;
	  U32 frames_to_read = 0;
	  DWORD flags = 0;
	  U64 timestamp = 0;
	  result = IAudioCaptureClient_GetBuffer(wasapi_state->source, &src_samples, &frames_to_read,
						 &flags, 0, &timestamp);
	  if(result != S_OK) { Win32LogError(result); }

	  {
	    R32 *src = (R32*)src_samples;
	    R32 *dest = wasapi_state->shared_buffer_samples + wasapi_state->shared_buffer_write_idx;
	    CopyArray(dest, src, R32, frames_to_read);
	    
	    frames_read += frames_to_read;
	    wasapi_state->shared_buffer_write_idx += frames_to_read;
	    wasapi_state->shared_buffer_write_idx %= wasapi_state->shared_buffer_sample_capacity;
	    wasapi_state->shared_buffer_last_write_size_in_frames += frames_to_read;
	  }

	  result = IAudioCaptureClient_ReleaseBuffer(wasapi_state->source, frames_to_read);
	  if(result != S_OK) { Win32LogError(result); }
	}

      }else if(signaled_handle == wasapi_state->render_callback_event_handle) {
	++render_event_count;
	// NOTE: get the number of frames available in the output buffer
	U32 output_padding_frames = 0;
	result = IAudioClient_GetCurrentPadding(wasapi_state->output_client, &output_padding_frames);
	if(result != S_OK) { Win32LogError(result); }
	U32 frames_to_write = wasapi_state->output_buffer_frame_count - output_padding_frames;

	R32 *in    = arena_push_array_z(process_arena, R32, frames_to_write);
	R32 *out_l = arena_push_array_z(process_arena, R32, frames_to_write);
	R32 *out_r = arena_push_array_z(process_arena, R32, frames_to_write);

	// NOTE: read samples from the shared buffer, perform sample-rate conversion
	// TODO: better sample-rate conversion
	{
	  U32 frames_to_read = wasapi_state->shared_buffer_last_write_size_in_frames;
	  if(frames_to_read) {
	    B32 all_frames_read = 0;
	    R32 *src = wasapi_state->shared_buffer_samples + wasapi_state->shared_buffer_read_idx;
	    R32 *dest = in;
	    for(U32 dest_idx = 0; dest_idx < frames_to_write && !all_frames_read; ++dest_idx) {
	      R32 src_pos = (R32)dest_idx * wasapi_state->samplerate_conversion_factor__src_from_dest;
	      U32 src_idx = FloorU32(src_pos);
	      R32 src_frac = src_pos - (R32)src_idx;

	      R32 converted_sample = 0.f;
	      if(src_idx + 1 < frames_to_read) {
		converted_sample = lerp(src[src_idx], src[src_idx + 1], src_frac);
	      }else {
		// TODO: This is a kinda hacky way of ensuring we don't read
		//       more samples than are available in the shared buffer.
		//       I'm not sure this is totally necessary any more, and if
		//       it still is, there's probably a better way of doing it.
		converted_sample = src[src_idx];
		frames_to_write = dest_idx + 1;
		all_frames_read = 1;
		OutputDebugStringA("output: read all input frames\n");
	      }
	      dest[dest_idx] = converted_sample;
	    }
	    
	    wasapi_state->shared_buffer_read_idx += frames_to_read;
	    wasapi_state->shared_buffer_read_idx %= wasapi_state->shared_buffer_sample_capacity;
	    //wasapi_state->shared_buffer_available_frames -= frames_to_read;
	    wasapi_state->shared_buffer_last_write_size_in_frames = 0;
	  }else {
	    OutputDebugStringA("output: no frames to read\n");
	  }
	}

	// NOTE: pull midi messages off of queue
	Audio_MidiMessage *first_midi_msg = 0;
	Audio_MidiMessage *last_midi_msg = 0;
	U64 midi_msg_count = 0;
	{
	  while(AtomicCompareAndSwap(&wasapi_state->midi_lock, 0, 1)) {}

	  first_midi_msg = wasapi_state->first_midi_msg;
	  wasapi_state->first_midi_msg = 0;

	  last_midi_msg = wasapi_state->last_midi_msg;
	  wasapi_state->last_midi_msg = 0;

	  midi_msg_count = wasapi_state->midi_msg_count;
	  wasapi_state->midi_msg_count = 0;

	  while(AtomicCompareAndSwap(&wasapi_state->midi_lock, 1, 0)) {}
	}

	// NOTE: call audio process
	global_process_data->input[0] = in;
	global_process_data->input[1] = in;
	global_process_data->output[0] = out_l;
	global_process_data->output[1] = out_r;
	global_process_data->first_midi_msg = first_midi_msg;
	global_process_data->last_midi_msg = last_midi_msg;
	global_process_data->midi_msg_count = midi_msg_count;
	global_process_data->sample_period = 1.f/(R32)wasapi_state->sink_fmt->Format.nSamplesPerSec;
	global_process_data->sample_count = frames_to_write;

	audio_process(global_process_data);

	// NOTE: write samples to the output buffer
	U8 *dest_samples = 0;
	result = IAudioRenderClient_GetBuffer(wasapi_state->sink, frames_to_write, &dest_samples);
	if(result != S_OK) { Win32LogError(result); }
	  
	{	 
	  R32 *src_l = out_l;
	  R32 *src_r = out_r;
	  R32 *dest = (R32*)dest_samples;
	  for(U32 sample_idx = 0; sample_idx < frames_to_write; ++sample_idx) {
	    *dest++ = *src_l++;
	    *dest++ = *src_r++;
	  }	 
	}
	
	result = IAudioRenderClient_ReleaseBuffer(wasapi_state->sink, frames_to_write, 0);
	if(result != S_OK) { Win32LogError(result); }

	// NOTE: put midi messages on freelist
	{
	  while(AtomicCompareAndSwap(&wasapi_state->midi_lock, 0, 1)) {}

	  if(first_midi_msg != 0 && last_midi_msg != 0) {
	    last_midi_msg->next = wasapi_state->midi_msg_freelist;
	    wasapi_state->midi_msg_freelist = first_midi_msg;
	  }

	  while(AtomicCompareAndSwap(&wasapi_state->midi_lock, 1, 0)) {}
	}
	
	arena_clear(process_arena);
      }else {
	Unreachable();
      }      

#if 0
      U32 output_padding_frames = 0;
      result = IAudioClient_GetCurrentPadding(wasapi_state->output_client, &output_padding_frames);
      U32 output_available_frames = wasapi_state->output_buffer_frame_count - output_padding_frames;
      U8 *dest_samples = 0;
      result = IAudioRenderClient_GetBuffer(wasapi_state->sink, output_available_frames, &dest_samples);
      if(result != S_OK) {
	Win32LogError(result);
      }

      if(output_available_frames < wasapi_state->output_latency_frames) continue;      

      R32 *in    = arena_push_array_z(process_arena, R32, output_available_frames);
      R32 *out_l = arena_push_array_z(process_arena, R32, output_available_frames);
      R32 *out_r = arena_push_array_z(process_arena, R32, output_available_frames);

      U32 input_available_frames = 0;
      result = IAudioClient_GetCurrentPadding(wasapi_state->input_client, &input_available_frames);

      U32 frames_read = 0;
      U32 frames_written = 0;
      while(frames_read < input_available_frames) {	

	U8 *src_samples = 0;
	U32 frames_to_read = 0;
	DWORD flags = 0;
	U64 timestamp = 0;
	result = IAudioCaptureClient_GetBuffer(wasapi_state->source,
					       &src_samples, &frames_to_read, &flags, 0, &timestamp);
	if(result != S_OK) {
	  Win32LogError(result);
	}
	switch(flags) {
	case AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY: {
	  OutputDebugStringA("wasapi input data discontinuity\n");
	}break;
	case AUDCLNT_BUFFERFLAGS_SILENT: {
	  OutputDebugStringA("wasapi input silent\n");
	}break;
	case AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR: {
	  OutputDebugStringA("wasapi input timestamp error\n");
	}break;
	default: {} break;
	}

	// NOTE: sample-rate conversion, write converted samples to in
	// TODO: better sample-rate conversion!!!
	{
	  U32 frames_to_write =
	    RoundU32(wasapi_state->samplerate_conversion_factor__dest_from_src * (R32)frames_to_read);
	  Assert(frames_written + frames_to_write <= output_available_frames);
	  R32 *src = (R32*)src_samples;
	  R32 *dest = in + frames_written;
	  for(U32 dest_idx = 0; dest_idx < frames_to_write; ++dest_idx) {
	    R32 src_pos = (R32)dest_idx * wasapi_state->samplerate_conversion_factor__src_from_dest;
	    U32 src_idx = FloorU32(src_pos);
	    R32 src_frac = src_pos - (R32)src_idx;

	    R32 converted_sample = 0.f;
	    if(src_idx + 1 < frames_to_read) {
	      converted_sample = lerp(src[src_idx], src[src_idx + 1], src_frac);
	    }else {
	      converted_sample = src[src_idx];
	    }
	    dest[dest_idx] = converted_sample;
	  }
	  
	  frames_written += frames_to_write;
	}

	/* CopyArray(input_ring_buffer_samples + input_ring_buffer_write_idx, src_samples, */
	/* 	  R32, frames_to_read); */
	/* input_ring_buffer_write_idx += frames_to_read; */
	frames_read += frames_to_read;
	result = IAudioCaptureClient_ReleaseBuffer(wasapi_state->source, frames_to_read);
      }      

      // NOTE: call audio process
      global_process_data->input[0] = in;
      global_process_data->input[1] = in;
      global_process_data->output[0] = out_l;
      global_process_data->output[1] = out_r;
      global_process_data->first_midi_msg = 0;
      global_process_data->last_midi_msg = 0;
      global_process_data->midi_msg_count = 0;
      global_process_data->sample_period = 1.f/(R32)wasapi_state->sink_fmt->Format.nSamplesPerSec;
      global_process_data->sample_count = frames_written;

      audio_process(global_process_data);

      // NOTE: write to output
      {
	R32 *src_l = out_l;
	R32 *src_r = out_r;
	R32 *dest = (R32*)dest_samples;
	for(U32 sample_idx = 0; sample_idx < frames_written; ++sample_idx) {
	  *dest++ = *src_l++;
	  *dest++ = *src_r++;
	}
      }

      result = IAudioRenderClient_ReleaseBuffer(wasapi_state->sink, frames_written, 0);
	
      /* CopyArray(in, input_ring_buffer_samples + input_ring_buffer_read_idx, R32, frames_read); */
      /* input_ring_buffer_read_idx += frames_read; */

#if 0      
      if(frames_available) {	
	Assert(frames_to_read <= frames_available);
	Assert(frames_available >= wasapi_state->output_latency_frames);
	//Assert(frames_to_read == wasapi_state->input_latency_frames);
	
	// DEBUG: accumulate input chunks
#if 0
	if(chunk_count < chunk_cap) {
	  wav_push_chunk(wav_writer, frames_to_read, src_samples);
	  ++chunk_count;
	}else {
	  if(!wav_written) {
	    end_wav(wav_writer, Str8Lit(DATA_DIR"/test/wasapi_input_test.wav"));
	    wav_written = 1;
	  }
	}
#endif	

	U32 frames_to_read_at_samplerate = frames_to_read;
	{
	  R32 sample_rate_ratio__src_to_dest = ((R32)wasapi_state->sink_fmt->Format.nSamplesPerSec /
						(R32)wasapi_state->source_fmt->Format.nSamplesPerSec);
	  R32 sample_rate_ratio__dest_to_src = 1.f/sample_rate_ratio__src_to_dest;
	  frames_to_read_at_samplerate = (U32)(sample_rate_ratio__src_to_dest * (R32)(frames_to_read - 1) + 0.5f);
	  Assert(frames_to_read_at_samplerate <= frames_available);
	  R32 *src = (R32*)src_samples;
	  R32 *dest = in;
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

	// DEBUG: accumulate sample-rate-converted input chunks
#if 0
	if(chunk_count < chunk_cap) {
	  wav_push_chunk(wav_writer, frames_to_read_at_samplerate, in);
	  ++chunk_count;
	}else {
	  if(!wav_written) {
	    end_wav(wav_writer, Str8Lit(DATA_DIR"/test/wasapi_input_sr_converted_test.wav"));
	    wav_written = 1;
	  }
	}
#endif

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
	  for(U32 i = 0; i < frames_to_read_at_samplerate; ++i) {
	    *dest++ = samples[sample_idx];
	    *dest++ = samples[sample_idx];
	    ++sample_idx;
	    sample_idx %= ArrayCount(samples);
	  }
#endif

	  // DEBUG: accumulate output chunks
#if 0
	  if(chunk_count < chunk_cap) {
	    wav_push_chunk(wav_writer_out, frames_to_read_at_samplerate, dest_samples);
	    ++chunk_count;
	  }else {
	    if(!wav_written) {
	      end_wav(wav_writer_out, Str8Lit(DATA_DIR"/test/wasapi_output_test.wav"));
	      wav_written = 1;
	    }
	  }
#endif
	}

	// TODO: WHAT IS THE CAUSE OF THE SCRATCHY NOISE?????
	if(frames_to_read_at_samplerate < 480) {
	  OutputDebugStringA("wasapi: wrote too few samples\n");
	}

	IAudioRenderClient_ReleaseBuffer(wasapi_state->sink, frames_to_read_at_samplerate, 0);
      }else {
	// TODO: use logging system
	OutputDebugStringA("output buffer is full\n");
      }
#endif
#endif
    }else {
      // TODO: sleep
    }
    arena_clear(process_arena);
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
