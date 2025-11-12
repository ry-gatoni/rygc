/** TODO:
 - get midi message sample index from timestamp
 - after using the pitch bend wheel, the sound fluctuates in pitch again
   a second or two later even though no new interaction happened, as if not all
   the messages are being cleared from the queue, or are somehow getting back on
   without new messages being sent.
 */

proc B32
wasapi_midi_init(void)
{
  MMRESULT result = 0;
  U64 pos = arena_pos(wasapi_state->arena);

  U32 midi_in_count = midiInGetNumDevs();
  String8List midi_input_device_names = {0};
  for(U32 midi_in_device_id = 0; midi_in_device_id < midi_in_count; ++midi_in_device_id)
  {
    MIDIINCAPS midi_in_capabilities = {0};
    result = midiInGetDevCaps(midi_in_device_id, &midi_in_capabilities, sizeof(midi_in_capabilities));
    if(result != MMSYSERR_NOERROR) goto wasapi_midi_in_get_caps_failure;

    str8_list_push(wasapi_state->arena, &midi_input_device_names, Str8Cstr(midi_in_capabilities.szPname));
  }

  HMIDIIN midi_input_handle = 0;
  U32 midi_device_id = 1; // TODO: make this variable/overrideable
  Win32_ThreadInfo *audio_process_thread_info = win32_thread_info_from_os_handle(wasapi_state->wasapi_process_thread_handle);
  result = midiInOpen(&midi_input_handle, midi_device_id, audio_process_thread_info->id, 0, CALLBACK_THREAD);
  if(result != MMSYSERR_NOERROR) goto wasapi_midi_in_open_failure;

  result = midiInStart(midi_input_handle);
  if(result != MMSYSERR_NOERROR) goto wasapi_midi_in_start_failure;

  if(0) {
  wasapi_midi_in_start_failure:
  wasapi_midi_in_open_failure:
  wasapi_midi_in_get_caps_failure:
    arena_pop_to(wasapi_state->arena, pos);
    return(0);
  }

  wasapi_state->midi_input_device_names = midi_input_device_names;
  wasapi_state->midi_input_handle = midi_input_handle;
  return(1);
}

proc B32
wasapi_init(String8 client_name)
{
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
  wasapi_midi_init();
  wasapi_state->running = 1;
  return(1);
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

  // NOTE: process arena is cleared every time we pass audio to the render stream
  Arena *process_arena = wasapi_state->wasapi_process_arena;
  U32 capture_event_count = 0;
  U32 render_event_count = 0;

  for(;;) {
    if(wasapi_state->running) {

      DWORD wait_result = MsgWaitForMultipleObjects(ArrayCount(wasapi_state->callback_event_handles),
						    wasapi_state->callback_event_handles,
						    FALSE,
						    INFINITE,
						    QS_POSTMESSAGE);
      U32 signaled_handle_idx = wait_result - WAIT_OBJECT_0;
      HANDLE signaled_handle = wasapi_state->callback_event_handles[signaled_handle_idx];
      if(signaled_handle == wasapi_state->capture_callback_event_handle)
      {
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

      }
      else if(signaled_handle == wasapi_state->render_callback_event_handle)
      {
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
	  first_midi_msg = wasapi_state->first_midi_msg;
	  wasapi_state->first_midi_msg = 0;

	  last_midi_msg = wasapi_state->last_midi_msg;
	  wasapi_state->last_midi_msg = 0;

	  midi_msg_count = wasapi_state->midi_msg_count;
	  wasapi_state->midi_msg_count = 0;
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
	
	arena_clear(process_arena);
      }
      else // NOTE: available; pop midi messages off the queue
      {	
	MSG msg = {0};
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
	  switch(msg.message)
	  {
	    case MIM_DATA:
	    {
	      Arena *arena = wasapi_state->wasapi_process_arena;

	      PushBuffer midi_push_buffer = push_buffer_alloc(arena, 2);
	      *buf_push_struct(&midi_push_buffer, U8) = ((msg.lParam >> 1*8) & 0xFF);
	      *buf_push_struct(&midi_push_buffer, U8) = ((msg.lParam >> 2*8) & 0xFF);

	      Audio_MidiMessage *new_msg = arena_push_struct(arena, Audio_MidiMessage);
	      new_msg->sample_idx = 0; // TODO: compute this from the message timestamp in milliseconds
	      new_msg->opcode = ((msg.lParam >> 0*8) & 0xF0);
	      new_msg->channel = ((msg.lParam >> 0*8) & 0x0F);
	      new_msg->body = buf_from_push_buffer(midi_push_buffer);

	      SLLQueuePush(wasapi_state->first_midi_msg, wasapi_state->last_midi_msg, new_msg);
	      ++wasapi_state->midi_msg_count;
	    }break;

	    // TODO: sysex, errors
	    default:
	    {}break;
	  }
	}
      }
    }else {
      // TODO: sleep
    }
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
