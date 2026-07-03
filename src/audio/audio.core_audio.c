// -----------------------------------------------------------------------------
// state

proc B32
core_audio_init(Arena *arena)
{
  ArenaTemp scratch = arena_get_scratch(&arena, 1);

  OSStatus status = 0;

  // NOTE: get all object ids
  AudioObjectPropertyAddress id_address = {
    .selector = kAudioHardwarePropertyDevices,
    .scope = kAudioObjectPropertyScopeGlobal,
    .element = kAudioObjectPropertyElementMain,
  };

  U32 id_data_size = 0;
  if((status = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &id_address, 0, 0, &id_data_size)))
  { goto core_audio_init_failure; }

  U32 device_count = id_data_size / sizeof(AudioObjectID);
  AudioObjectID *device_ids = arena_push_array(scratch.arena, AudioObjectID, device_count);
  if((status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &id_address, 0, 0, &id_data_size, device_ids)))
  { goto core_audio_init_failure; }

  // NOTE: get default input & output device ids
  AudioObjectPropertyAddress default_input_address = {
    .selector = kAudioHardwarePropertyDefaultInputDevice,
    .scope = kAudioObjectPropertyScopeGlobal,
    .element = kAudioObjectPropertyElementMain,
  };

  AudioObjectPropertyAddress default_output_address = {
    .selector = kAudioHardwarePropertyDefaultOutputDevice,
    .scope = kAudioObjectPropertyScopeGlobal,
    .element = kAudioObjectPropertyElementMain,
  };

  AudioObjectID default_input_device_id = 0;
  U32 default_input_device_id_size = sizeof(default_input_device_id);
  if((status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &default_input_address, 0, 0, &default_input_device_id_size, &default_input_device_id)))
  { goto core_audio_init_failure; }

  AudioObjectID default_output_device_id = 0;
  U32 default_output_device_id_size = sizeof(default_output_device_id);
  if((status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &default_output_address, 0, 0, &default_output_device_id_size, &default_output_device_id)))
  { goto core_audio_init_failure; }

  // NOTE: get all object names
  AudioObjectPropertyAddress name_address = {
    .selector = kAudioObjectPropertyName,
    .scope = kAudioObjectPropertyScopeGlobal,
    .element = kAudioObjectPropertyElementMain,
  };

  String8 *device_names = arena_push_array(scratch.arena, String8, device_count);
  for(U32 device_idx = 0; device_idx < device_count; ++device_idx)
  {
    CFStringRef device_name_cf = 0;
    U32 name_size = sizeof(device_name_cf);
    AudioObjectID device_id = device_ids[device_idx];
    if((status = AudioObjectGetPropertyData(device_id, &name_address, 0, 0, &name_size, &device_name_cf)))
    { goto core_audio_init_failure; }

    String8 device_name = str8_from_cfstr(scratch.arena, device_name_cf);
    device_names[device_idx] = device_name;
  }

  // NOTE: instantiate and configure audio units for default input and output devices
  AudioComponentDescription auhal_desc = {
    .componentType = kAudioUnitType_Output,
    .componentSubType = kAudioUnitSubType_HALOutput,
    .componentManufacturer = kAudioUnitManufacturer_Apple,
    .componentFlags = 0,
    .componentFlagsMask = 0,
  };
  AudioComponent auhal_comp = AudioComponentFindNext(0, &auhal_desc);
  if(!auhal_comp)
  { goto core_audio_init_failure; }

  AudioUnit au_in;
  AudioUnit au_out;
  if((status = AudioComponentInstanceNew(auhal_comp, &au_in)))
  { goto core_audio_init_failure; }
  if((status = AudioComponentInstanceNew(auhal_comp, &au_out)))
  { goto core_audio_init_failure; }

  U32 enable_io = 1;
  U32 disable_io = 0;

  if((status = AudioUnitSetProperty(au_in, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &enable_io, sizeof(enable_io))))
  { goto core_audio_init_failure; }
  if((status = AudioUnitSetProperty(au_in, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &disable_io, sizeof(disable_io))))
  { goto core_audio_init_failure; }

  if((status = AudioUnitSetProperty(au_out, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &disable_io, sizeof(disable_io))))
  { goto core_audio_init_failure; }
  if((status = AudioUnitSetProperty(au_out, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &enable_io, sizeof(enable_io))))
  { goto core_audio_init_failure; }

  if((status = AudioUnitSetProperty(au_in, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &default_input_device_id, sizeof(default_input_device_id))))
  { goto core_audio_init_failure; }
  if((status = AudioUnitSetProperty(au_out, kAudioOutputUnitProperty_CurrentDevice, kAudioUnitScope_Global, 0, &default_output_device_id, sizeof(default_output_device_id))))
  { goto core_audio_init_failure; }

  // NOTE: set input and output stream formats
  AudioStreamBasicDescription in_fmt = {
    .sample_rate = 48000.0,
    .format_id = kAudioFormatLinearPCM,
    .format_flags = kLinearPCMFormatFlagIsFloat,
    .bytes_per_packet = 1*sizeof(R32),
    .frames_per_packet = 1,
    .bytes_per_frame = 1*sizeof(R32),
    .channels_per_frame = 1,
    .bits_per_channel = 8*sizeof(R32),
  };
  if((status = AudioUnitSetProperty(au_in, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &in_fmt, sizeof(in_fmt))))
  { goto core_audio_init_failure; }

  AudioStreamBasicDescription out_fmt = {
    .sample_rate = 48000.0,
    .format_id = kAudioFormatLinearPCM,
    .format_flags = kLinearPCMFormatFlagIsFloat|kLinearPCMFormatFlagIsNonInterleaved,
    .bytes_per_packet = 1*sizeof(R32),
    .frames_per_packet = 1,
    .bytes_per_frame = 1*sizeof(R32),
    .channels_per_frame = 2,
    .bits_per_channel = 8*sizeof(R32),
  };
  if((status = AudioUnitSetProperty(au_out, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &out_fmt, sizeof(out_fmt))))
  { goto core_audio_init_failure; }

  // NOTE: set input and output callbacks
  AURenderCallbackStruct in_cb = {
    .input_proc = core_audio_input_device_proc,
    .input_proc_ref_con = 0,
  };

  // TODO: do I set the _render_ callback on the _input_ unit?
  if((status = AudioUnitSetProperty(au_in, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Global, 0, &in_cb, sizeof(in_cb))))
  { goto core_audio_init_failure; }

  AURenderCallbackStruct out_cb = {
    .input_proc = core_audio_output_device_proc,
    .input_proc_ref_con = 0,
  };

  if((status = AudioUnitSetProperty(au_out, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Global, 0, &out_cb, sizeof(out_cb))))
  { goto core_audio_init_failure; }

  AudioUnitInitialize(au_in);
  AudioUnitInitialize(au_out);

  // NOTE: allocate state
  core_audio_state = arena_push_struct(arena, CoreAudio_State);
  core_audio_state->arena = arena;

  // NOTE: allocate devices
  for(U32 device_idx = 0; device_idx < device_count; ++device_idx)
  {
    String8 device_name = device_names[device_idx];
    CoreAudio_Device *device = core_audio__device_alloc(device_name);
    device->id = device_ids[device_idx];

    if(device_ids[device_idx] == default_input_device_id)
    {
      core_audio_state->default_input_device = device;
      core_audio_state->input_device = device;
    }
    if(device_ids[device_idx] == default_output_device_id)
    {
      core_audio_state->default_output_device = device;
      core_audio_state->output_device = device;
    }
  }

  core_audio_state->input_unit = au_in;
  core_audio_state->output_unit = au_out;

  os_ring_buffer_init(&core_audio_state->samples, KB(64));

  arena_release_scratch(scratch);
  return 1;

core_audio_init_failure:
  arena_release_scratch(scratch);
  return 0;
}

proc void
core_audio_uninit(void)
{
  os_ring_buffer_release(&core_audio_state->samples);
  core_audio_state = 0;
}

// -----------------------------------------------------------------------------
// devices

proc CoreAudio_Device*
core_audio_default_input_device(void)
{
  CoreAudio_Device *result = core_audio_state->default_input_device;
  return result;
}

proc CoreAudio_Device*
core_audio_default_output_device(void)
{
  CoreAudio_Device *result = core_audio_state->default_output_device;
  return result;
}

proc CoreAudio_Device*
core_audio_input_device(void)
{
  CoreAudio_Device *result = core_audio_state->input_device;
  return result;
}

proc CoreAudio_Device*
core_audio_output_device(void)
{
  CoreAudio_Device *result = core_audio_state->output_device;
  return result;
}

proc void
core_audio_set_input_device(CoreAudio_Device *device)
{
  core_audio_state->input_device = device;
}

proc void
core_audio_set_output_device(CoreAudio_Device *device)
{
  core_audio_state->input_device = device;
}

proc Audio_Handle
audio_default_input_device(void)
{
  CoreAudio_Device *device = core_audio_default_input_device();
  Audio_Handle result = core_audio__handle_from_device(device);
  return result;
}

proc Audio_Handle
audio_default_output_device(void)
{
  CoreAudio_Device *device = core_audio_default_output_device();
  Audio_Handle result = core_audio__handle_from_device(device);
  return result;
}

proc Audio_Handle
audio_input_device(void)
{
  CoreAudio_Device *device = core_audio_input_device();
  Audio_Handle result = core_audio__handle_from_device(device);
  return result;
}

proc Audio_Handle
audio_output_device(void)
{
  CoreAudio_Device *device = core_audio_output_device();
  Audio_Handle result = core_audio__handle_from_device(device);
  return result;
}

proc void
audio_set_input_device(Audio_Handle input)
{
  CoreAudio_Device *device = core_audio__device_from_audio_handle(input);
  core_audio_set_input_device(device);
}

proc void
audio_set_output_device(Audio_Handle output)
{
  CoreAudio_Device *device = core_audio__device_from_audio_handle(output);
  core_audio_set_output_device(device);
}

proc String8
audio_device_name(Audio_Handle device)
{
  CoreAudio_Device *ca_device = core_audio__device_from_audio_handle(device);
  String8 result = ca_device->name;
  return result;
}

proc Audio_DeviceIterator
audio_device_iterator(void)
{
  Audio_DeviceIterator result = {0};
  result.device = core_audio__handle_from_device(core_audio_state->first_device);
  return result;
}

proc B32
audio_device_iterator_done(Audio_DeviceIterator it)
{
  B32 result = it.device.handle == 0;
  return result;
}

proc Audio_DeviceIterator
audio_device_iterator_next(Audio_DeviceIterator it)
{
  CoreAudio_Device *current_device = core_audio__device_from_audio_handle(it.device);
  CoreAudio_Device *next_device = current_device->next;

  Audio_DeviceIterator result = {0};
  result.device = core_audio__handle_from_device(next_device);
  return result;
}

// -----------------------------------------------------------------------------
// process

proc OSStatus
core_audio_input_device_proc(void *in_ref_con, AudioUnitRenderActionFlags *io_action_flags,
			     const AudioTimestamp *in_timestamp, U32 in_bus_number,
			     U32 in_num_frames, AudioBufferList *io_data)
{
  Unused(in_ref_con);
  Unused(io_action_flags);
  Unused(in_timestamp);
  Unused(in_bus_number);
  Unused(io_data);
  OSStatus status = 0;

  Os_RingBuffer *rb = &core_audio_state->samples;
  Assert(os_ring_buffer_free(rb) >= in_num_frames*sizeof(R32));

  Os_RingBufferSpan dest = os_ring_buffer_write_span(rb);
  AudioBufferList dest_buf = {
    .num_buffers = 1,
    .buffers[0] = {
      .num_channels = 1,
      .data_byte_size = in_num_frames*sizeof(R32),
      .data = dest.start,
    },
  };
  AudioUnit in_unit = core_audio_state->input_unit;
  status = AudioUnitRender(in_unit, io_action_flags, in_timestamp, in_bus_number, in_num_frames, &dest_buf);
  Assert(status == 0);
  os_ring_buffer_write_end(rb, in_num_frames*sizeof(R32));

  return status;
}

proc OSStatus
core_audio_output_device_proc(void *in_ref_con, AudioUnitRenderActionFlags *io_action_flags,
			      const AudioTimestamp *in_timestamp, U32 in_bus_number,
			      U32 in_num_frames, AudioBufferList *io_data)
{
  Unused(in_ref_con);
  Unused(io_action_flags);
  Unused(in_timestamp);
  Unused(in_bus_number);

  Os_RingBuffer *rb = &core_audio_state->samples;
  Assert(io_data->num_buffers == 2);
  R32 *dest0 = io_data->buffers[0].data;
  R32 *dest1 = io_data->buffers[1].data;
  if(os_ring_buffer_used(rb) < in_num_frames*sizeof(R32))
  {
    ZeroArray(dest0, R32, in_num_frames);
    ZeroArray(dest1, R32, in_num_frames);
  }
  else
  {
    Os_RingBufferSpan src = os_ring_buffer_read_span(rb);

    ArenaTemp scratch = arena_get_scratch(0, 0);

    R32 *input_samples = arena_push_array_z(scratch.arena, R32, 2*in_num_frames);
    Audio_ProcessData data = {0};
    data.input[0] = input_samples + 0*in_num_frames;
    data.input[1] = input_samples + 1*in_num_frames;
    data.output[0] = dest0;
    data.output[1] = dest1;
    data.sample_count = in_num_frames;
    data.user_data = audio_state->process_user_data;

    CopyArray(data.input[0], src.start, R32, in_num_frames);
    CopyArray(data.input[1], src.start, R32, in_num_frames);
    os_ring_buffer_read_end(rb, in_num_frames*sizeof(R32));

    audio_process(&data);

    arena_release_scratch(scratch);
  }

  return 0;
}

proc void
audio_start(void)
{
  AudioOutputUnitStart(core_audio_state->output_unit);
  AudioOutputUnitStart(core_audio_state->input_unit);
}

proc void
audio_stop(void)
{
  AudioOutputUnitStop(core_audio_state->input_unit);
  AudioOutputUnitStop(core_audio_state->output_unit);
}

// -----------------------------------------------------------------------------
// helpers

proc inline CoreAudio_Device*
core_audio__device_alloc(String8 name)
{
  CoreAudio_Device *result = 0;
  for(CoreAudio_Device *free = core_audio_state->first_free_device; free; free = free->next)
  {
    if(str8s_are_equal(free->name, name))
    {
      result = free;
      break;
    }
  }

  if(result)
  {
    DLLRemove(core_audio_state->first_free_device, core_audio_state->last_free_device, result);
  }
  else
  {
    result = arena_push_struct(core_audio_state->arena, CoreAudio_Device);
    result->name = arena_push_str8_copy(core_audio_state->arena, name);
  }
  Assert(result);

  DLLPushBack(core_audio_state->first_device, core_audio_state->last_device, result);
  ++core_audio_state->device_count;

  return result;
}

proc inline void
core_audio__device_release(CoreAudio_Device *device)
{
  DLLRemove(core_audio_state->first_device, core_audio_state->last_device, device);
  --core_audio_state->device_count;

  DLLPushBack(core_audio_state->first_free_device, core_audio_state->last_free_device, device);
}

proc inline Audio_Handle
core_audio__handle_from_device(CoreAudio_Device *device)
{
  Audio_Handle result = {0};
  result.handle = device;
  return result;
}

proc inline CoreAudio_Device*
core_audio__device_from_audio_handle(Audio_Handle handle)
{
  CoreAudio_Device *result = handle.handle;
  return result;
}

proc inline String8
str8_from_cfstr(Arena *arena, CFStringRef cfstr)
{
  CFIndex cfstr_len = CFStringGetLength(cfstr);
  CFRange cfrng = CFRangeMake(0, cfstr_len);

  CFIndex cfstr_size;
  CFStringGetBytes(cfstr, cfrng, kCFStringEncodingUTF8, 0, 0, 0, 0, &cfstr_size);

  CFIndex cstr_size = cfstr_size + 1;
  U8 *cstr_bytes = arena_push_array(arena, U8, cstr_size);
  Assert(CFStringGetCString(cfstr, (char*)cstr_bytes, cstr_size, kCFStringEncodingUTF8));
  cstr_bytes[cfstr_size] = 0;

  String8 result = str8_range(cstr_bytes, cstr_bytes + cfstr_size);
  return result;
}
