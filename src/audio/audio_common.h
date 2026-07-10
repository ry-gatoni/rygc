// TODO:
// - devices:
//   - change active device
//   - use mutliple devices
//   - configure buffer size
// - streams:
//   - distinguish pull direction (caller is requesting samples) from push direction (caller has samples to give)
// - midi?
// - parameters?

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

typedef enum Audio_StreamStatus
{
  Audio_StreamStatus_ok,
  Audio_StreamStatus_error,
} Audio_StreamStatus;

typedef struct Audio_Stream Audio_Stream;
typedef Audio_StreamStatus Audio_StreamRefillProc(Audio_Stream *self, Audio_Stream *caller);
struct Audio_Stream
{
  R32 *samples_start;
  R32 *samples_end;
  R32 *sample_cursor;
  Audio_StreamRefillProc *refill; // NOTE: null iff this is a sentinel stream (endpoint in a particular direction)
};

typedef struct Audio_OutputStream
{
#define AUDIO_OUTPUT_MAX_CHANNEL_COUNT 32
  Audio_Stream self;
  Audio_Stream *sources[AUDIO_OUTPUT_MAX_CHANNEL_COUNT];
} Audio_OutputStream;

typedef struct Audio_InputStream
{
  Audio_Stream self;
  Os_RingBuffer samples;
} Audio_InputStream;

typedef struct Audio_PassthruStream
{
  Audio_Stream self;
  Audio_Stream *source;
} Audio_PassthruStream;

typedef struct Audio_State
{
  Arena *arena;

  U64 sample_rate;
  void *process_user_data;

  Audio_Stream **streams;
  U32 stream_count;
  U32 stream_capacity;

#define AUDIO_INPUT_MAX_CHANNEL_COUNT 32
  Audio_InputStream input_streams[AUDIO_INPUT_MAX_CHANNEL_COUNT];
  Audio_OutputStream *output_stream;
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
// streams

proc void audio_stream_add(Audio_Stream *stream);

proc void audio_stream_connect_output(Audio_Stream *stream, U32 channel_idx);
proc Audio_Stream* audio_stream_get_input(U32 channel_idx);

// -----------------------------------------------------------------------------
// helpers

// NOTE: midi message handling
proc Audio_MidiMessage* audio_next_midi_message(Audio_ProcessData *data);
proc void audio_dequeue_midi_message(Audio_ProcessData *data);

// NOTE: midi utils
proc U32 audio_midi_get_note_number(Audio_MidiMessage *midi_msg);
proc R32 audio_midi_get_pitch_bend(Audio_MidiMessage *midi_msg);
proc R32 audio_hertz_from_midi_note(U32 midi_note);
