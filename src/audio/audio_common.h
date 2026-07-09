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
typedef Audio_StreamStatus Audio_StreamRefillProc(Audio_Stream *self, R32 **dest_samples, U32 channel_count, U32 sample_count);
struct Audio_Stream
{
  R32 *samples_start;
  R32 *samples_end;
  R32 *sample_cursor;
  Audio_StreamRefillProc *refill;
};

typedef struct Audio_InputStream
{
  Audio_Stream self;
  Os_RingBuffer samples;
} Audio_InputStream;

proc Audio_StreamStatus
audio_input_stream_refill(Audio_Stream *self, R32 **dest_samples, U32 channel_count, U32 sample_count)
{
  Audio_InputStream *input = (Audio_InputStream*)self;
  Unused(dest_samples);
  Unused(channel_count);
  Unused(sample_count);

  Os_RingBuffer *samples = &input->samples;

  U64 samples_read = self->sample_cursor - self->samples_start;
  os_ring_buffer_read_end(samples, samples_read*sizeof(R32));

  Os_RingBufferSpan read_span = os_ring_buffer_read_span(samples);
  U64 samples_available = (read_span.end - read_span.start)/sizeof(R32);
  self->samples_start = (R32*)read_span.start;
  self->samples_end = self->samples_start + samples_available;
  self->sample_cursor = self->samples_start;

  return Audio_StreamStatus_ok;
}

typedef struct Audio_PassthruStream
{
  Audio_Stream self;
  Audio_Stream *source;
} Audio_PassthruStream;

proc Audio_StreamStatus
audio_passthru_stream_refill(Audio_Stream *self, R32 **dest_samples, U32 channel_count, U32 sample_count)
{
  Audio_PassthruStream *passthru = (Audio_PassthruStream*)self;

  Assert(dest_samples);
  Assert(channel_count);
  Assert(sample_count);

  Audio_Stream *source = passthru->source;
  source->sample_cursor = source->samples_end;
  source->refill(source, 0, 0, 0);
  U64 samples_available = IntFromPtr(source->samples_end - source->samples_start)/sizeof(R32);
  Assert(sample_count <= samples_available);

  R32 *src = source->samples_start;
  for(U32 channel_idx = 0; channel_idx < channel_count; ++channel_idx)
  {
    R32 *dest = dest_samples[channel_idx];
    CopyArray(dest, src, R32, sample_count);
  }

  return Audio_StreamStatus_ok;
}

typedef struct Audio_State
{
  Arena *arena;

  U64 sample_rate;
  void *process_user_data;

  Audio_Stream **streams;
  U32 stream_count;
  U32 stream_capacity;

  Audio_InputStream *input_stream;
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

proc Audio_Stream* audio_stream_get_input(void);

// -----------------------------------------------------------------------------
// helpers

// NOTE: midi message handling
proc Audio_MidiMessage* audio_next_midi_message(Audio_ProcessData *data);
proc void audio_dequeue_midi_message(Audio_ProcessData *data);

// NOTE: midi utils
proc U32 audio_midi_get_note_number(Audio_MidiMessage *midi_msg);
proc R32 audio_midi_get_pitch_bend(Audio_MidiMessage *midi_msg);
proc R32 audio_hertz_from_midi_note(U32 midi_note);
