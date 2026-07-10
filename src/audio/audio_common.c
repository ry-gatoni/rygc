proc Audio_MidiMessage*
audio_next_midi_message(Audio_ProcessData *data)
{
  Audio_MidiMessage *msg = data->first_midi_msg;
  return(msg);
}

proc void
audio_dequeue_midi_message(Audio_ProcessData *data)
{
  SLLQueuePop(data->first_midi_msg, data->last_midi_msg);
}

proc U32
audio_midi_get_note_number(Audio_MidiMessage *msg)
{
  Assert(msg->opcode == MidiOpcode_note_on || msg->opcode == MidiOpcode_note_off);
  U32 result = msg->body.mem[0];
  return(result);
}

proc R32
audio_midi_get_pitch_bend(Audio_MidiMessage *msg)
{
  Assert(msg->opcode == MidiOpcode_pitch_bend);
  U32 pitch_bend_max_neg = (1 << 13);
  S16 pitch_bend_int = ((msg->body.mem[1] & 0x7F) << 7 |
			(msg->body.mem[0] & 0x7F)) - pitch_bend_max_neg;
  R32 result = powf(2.f, (R32)(pitch_bend_int)/(R32)(pitch_bend_max_neg));
  return(result);
}

proc R32
audio_hertz_from_midi_note(U32 note)
{
  R32 result = 440.f*powf(2.f, (R32)((S32)note - 69)/12.f);
  return(result);
}

// -----------------------------------------------------------------------------
// internal

proc Audio_StreamStatus
audio_input_stream_refill(Audio_Stream *self, Audio_Stream *caller)
{
  Audio_InputStream *input = (Audio_InputStream*)self;

  Unused(caller);

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

proc void
audio_input_stream_init(Audio_InputStream *stream)
{
  os_ring_buffer_init(&stream->samples, KB(32));
  stream->self.refill = audio_input_stream_refill;
}

proc Audio_StreamStatus
audio_passthru_stream_refill(Audio_Stream *self, Audio_Stream *caller)
{
  Audio_PassthruStream *passthru = (Audio_PassthruStream*)self;

  Audio_Stream *source = passthru->source;
  source->sample_cursor = source->samples_end;
  source->refill(source, self);
  U64 samples_available = IntFromPtr(source->samples_end - source->samples_start)/sizeof(R32);

  Assert(caller);
  Assert(caller->refill == 0);
  Assert(caller->sample_cursor == caller->samples_start);
  U64 samples_to_write = IntFromPtr(caller->samples_end - caller->samples_start)/sizeof(R32);
  Assert(samples_to_write == samples_available); // TODO: handle over/under run
  R32 *src = source->samples_start;
  R32 *dest = caller->samples_start;
  CopyArray(dest, src, R32, samples_to_write);

  return Audio_StreamStatus_ok;
}

// -----------------------------------------------------------------------------
// state

proc B32
audio_init(void)
{
  Arena *arena = arena_alloc();
  audio_state = arena_push_struct(arena, Audio_State);
  audio_state->arena = arena;

  audio_state->stream_capacity = 128;
  audio_state->stream_count = 0;
  audio_state->streams = arena_push_array(arena, Audio_Stream*, audio_state->stream_capacity);

  for(U32 stream_idx = 0; stream_idx < ArrayCount(audio_state->input_streams); ++stream_idx)
  {
    Audio_InputStream *input_stream = &audio_state->input_streams[stream_idx];
    audio_input_stream_init(input_stream);
    audio_state->streams[audio_state->stream_count++] = &input_stream->self;
  }
  audio_state->output_stream = arena_push_struct(arena, Audio_OutputStream);
  audio_state->streams[audio_state->stream_count++] = &audio_state->output_stream->self;

  B32 result;
#if AUDIO_BACKEND == AUDIO_BACKEND_JACK
  result = jack_init(arena);
#elif AUDIO_BACKEND == AUDIO_BACKEND_WASAPI
  result = wasapi_init(arena);
#elif AUDIO_BACKEND == AUDIO_BACKEND_CORE_AUDIO
  result = core_audio_init(arena);
#else
# error backend not implemented
#endif
  return result;
}

proc void
audio_uninit(void)
{
#if AUDIO_BACKEND == AUDIO_BACKEND_JACK
  jack_uninit();
#elif AUDIO_BACKEND == AUDIO_BACKEND_WASAPI
  wasapi_uninit();
#elif AUDIO_BACKEND == AUDIO_BACKEND_CORE_AUDIO
  core_audio_uninit();
#else
# error backend not implemented
#endif

  Arena *arena = audio_state->arena;
  audio_state = 0;
  arena_clear(arena);
}

// -----------------------------------------------------------------------------
// process

proc void
audio_set_process_data(void *data)
{
  audio_state->process_user_data = data;
}

// -----------------------------------------------------------------------------
// streams

proc void
audio_stream_add(Audio_Stream *stream)
{
  Assert(audio_state->stream_count < audio_state->stream_capacity);
  Audio_Stream **slot = audio_state->streams + audio_state->stream_count++;
  *slot = stream;
}

proc void
audio_stream_connect_output(Audio_Stream *stream, U32 channel_idx)
{
  Assert(channel_idx <= ArrayCount(audio_state->output_stream->sources));
  audio_state->output_stream->sources[channel_idx] = stream;
}

proc Audio_Stream*
audio_stream_get_input(U32 channel_idx)
{
  Assert(channel_idx <= ArrayCount(audio_state->input_streams));
  Audio_Stream *result = &audio_state->input_streams[channel_idx].self;
  return result;
}
