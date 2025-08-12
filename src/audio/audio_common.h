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

// NOTE: audio process entry point
extern void audio_process(Audio_ProcessData *data);

// NOTE: interface
proc B32 audio_init(String8 client_name);
proc void audio_uninit(void);
proc void audio_set_user_data(void *data);
proc U32 audio_get_sample_rate(void);

// NOTE: midi message handling
proc Audio_MidiMessage* audio_next_midi_message(Audio_ProcessData *data);
proc void audio_dequeue_midi_message(Audio_ProcessData *data);

// NOTE: midi utils
proc U32 audio_midi_get_note_number(Audio_MidiMessage *midi_msg);
proc R32 audio_midi_get_pitch_bend(Audio_MidiMessage *midi_msg);
proc R32 audio_hertz_from_midi_note(U32 midi_note);

