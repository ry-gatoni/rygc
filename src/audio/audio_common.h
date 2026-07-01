typedef struct Audio_MidiMessage Audio_MidiMessage;
struct Audio_MidiMessage
{
  Audio_MidiMessage *next;

  U32 sample_idx;
  U8 opcode;
  U8 channel;
  Buffer body;
};

typedef enum MidiOpcode
{
  MidiOpcode_note_off		   = 0x80,
  MidiOpcode_note_on		   = 0x90,
  MidiOpcode_aftertouch		   = 0xA0,
  MidiOpcode_continuous_controller = 0xB0,
  MidiOpcode_patch_change	   = 0xC0,
  MidiOpcode_channel_pressure	   = 0xD0,
  MidiOpcode_pitch_bend		   = 0xE0,
  MidiOpcode_sysex		   = 0xF0,
} MidiOpcode;

typedef struct Audio_Handle
{
  void *handle;
} Audio_Handle;

typedef struct Audio_DeviceIterator
{
  Audio_Handle device;
} Audio_DeviceIterator;

typedef struct Audio_ProcessData
{
  R32 *input[2];
  R32 *output[2];

  Audio_MidiMessage *first_midi_msg;
  Audio_MidiMessage *last_midi_msg;
  U64 midi_msg_count;

  R32 sample_period;
  U32 sample_count;

  void *user_data;
} Audio_ProcessData;

typedef struct Audio_State
{
  Arena *arena;

  U64 sample_rate;
  void *process_user_data;
} Audio_State;

global Audio_State *audio_state = 0;

// -----------------------------------------------------------------------------
// state

proc B32 audio_init(void);
proc void audio_uninit(void);

// -----------------------------------------------------------------------------
// devices

proc Audio_Handle audio_default_input_device(void);
proc Audio_Handle audio_default_output_device(void);

proc Audio_Handle audio_input_device(void);
proc Audio_Handle audio_output_device(void);

proc void audio_set_input_device(Audio_Handle device);
proc void audio_set_output_device(Audio_Handle device);

proc String8 audio_device_name(Audio_Handle device);

proc Audio_DeviceIterator audio_device_iterator(void);
proc B32 audio_device_iterator_done(Audio_DeviceIterator it);
proc Audio_DeviceIterator audio_device_iterator_next(Audio_DeviceIterator it);

// -----------------------------------------------------------------------------
// process

proc void audio_set_process_data(void *user_data);

proc B32 audio_set_sample_rate(U64 sample_rate);
proc U64 audio_get_sample_rate(void);

extern void audio_process(Audio_ProcessData *data); // NOTE: audio process entry point

proc void audio_start(void);
proc void audio_stop(void);

// -----------------------------------------------------------------------------
// helpers

// NOTE: midi message handling
proc Audio_MidiMessage* audio_next_midi_message(Audio_ProcessData *data);
proc void audio_dequeue_midi_message(Audio_ProcessData *data);

// NOTE: midi utils
proc U32 audio_midi_get_note_number(Audio_MidiMessage *midi_msg);
proc R32 audio_midi_get_pitch_bend(Audio_MidiMessage *midi_msg);
proc R32 audio_hertz_from_midi_note(U32 midi_note);
