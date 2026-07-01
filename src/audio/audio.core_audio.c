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

  arena_release_scratch(scratch);
  return 1;

core_audio_init_failure:
  arena_release_scratch(scratch);
  return 0;
}

proc void
core_audio_uninit(void)
{

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
