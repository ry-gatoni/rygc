#include "audio.core_audio.api_defs.h"

// -----------------------------------------------------------------------------
// internal types

typedef struct CoreAudio_Device CoreAudio_Device;
struct CoreAudio_Device
{
  CoreAudio_Device *next;
  CoreAudio_Device *prev;

  String8 name;
  U32 input_channel_count;
  U32 output_channel_count;

  U64 max_sample_rate;

  AudioObjectID id;
};

#define AU_CALLBACK_PROC(name) OSStatus (name)(void *in_ref_con, AudioUnitRenderActionFlags *io_action_flags, const AudioTimestamp *in_timestamp, U32 in_bus_number, U32 in_num_frames, AudioBufferList *io_data)
typedef AU_CALLBACK_PROC(CoreAudio_StreamProc);

typedef struct CoreAudio_Stream CoreAudio_Stream;
struct CoreAudio_Stream
{
  CoreAudio_Stream *next;
  CoreAudio_Stream *prev;

  AudioUnit unit;
  CoreAudio_Device *device;
  U64 sample_rate;

  Os_RingBuffer samples;
  CoreAudio_StreamProc *refill;
};

typedef struct CoreAudio_State
{
  Arena *arena;

  CoreAudio_Device *first_device;
  CoreAudio_Device *last_device;
  U64 device_count;

  CoreAudio_Device *first_free_device;
  CoreAudio_Device *last_free_device;

  CoreAudio_Device *default_input_device;
  CoreAudio_Device *default_output_device;

  CoreAudio_Device *input_device;
  CoreAudio_Device *output_device;

  // TODO: abstract this so we can have multiple streams at the same time
  AudioUnit input_unit;
  AudioUnit output_unit;

  U32 input_sample_rate;
  U32 output_sample_rate;

  Os_RingBuffer samples; // TODO: several ring buffers for multi channels?
} CoreAudio_State;

global CoreAudio_State *core_audio_state = 0;

// -----------------------------------------------------------------------------
// state

proc B32 core_audio_init(Arena *arena);
proc void core_audio_uninit(void);

// -----------------------------------------------------------------------------
// devices

proc CoreAudio_Device* core_audio_default_input_device(void);
proc CoreAudio_Device* core_audio_default_output_device(void);

proc CoreAudio_Device* core_audio_input_device(void);
proc CoreAudio_Device* core_audio_output_device(void);

proc void core_audio_set_input_device(CoreAudio_Device *input);
proc void core_audio_set_output_device(CoreAudio_Device *output);

// -----------------------------------------------------------------------------
// process

// NOTE: internal
proc AU_CALLBACK_PROC(core_audio_input_device_proc);
proc AU_CALLBACK_PROC(core_audio_output_device_proc);

// -----------------------------------------------------------------------------
// helpers

proc inline CoreAudio_Device* core_audio__device_alloc(String8 name);
proc inline void core_audio__device_release(CoreAudio_Device *device);

proc inline Audio_Handle core_audio__handle_from_device(CoreAudio_Device *device);
proc inline CoreAudio_Device* core_audio__device_from_audio_handle(Audio_Handle handle);

proc inline String8 str8_from_cfstr(Arena *arena, CFStringRef cfstr);
