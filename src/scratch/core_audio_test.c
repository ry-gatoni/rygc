#include "base/base.h"
#include "audio/audio.h"

#include "base/base.c"
#include "audio/audio.c"

typedef struct DelayLine
{
  Os_RingBuffer prev_samples[2];
  U64 delay_samples;
} DelayLine;

proc void
delay_process(Audio_ProcessData *data)
{
  DelayLine *delay_line = data->user_data;
  U64 sample_count = data->sample_count;

  for(U32 channel_idx = 0; channel_idx < ArrayCount(data->input); ++channel_idx)
  {
    Os_RingBuffer *prev_samples = &delay_line->prev_samples[channel_idx];

    // NOTE: fill buffer
    {
      R32 *src = data->input[channel_idx];

      Assert(sample_count <= os_ring_buffer_free(prev_samples));
      Os_RingBufferSpan write_span = os_ring_buffer_write_span(prev_samples);
      R32 *dest = (R32*)write_span.start;

      CopyArray(dest, src, R32, sample_count);
      os_ring_buffer_write_end(prev_samples, sample_count*sizeof(R32));
    }

    // NOTE: write to output
    {
      R32 *src = data->input[channel_idx];

      Assert(sample_count <= os_ring_buffer_used(prev_samples));
      Os_RingBufferSpan read_span = os_ring_buffer_read_span(prev_samples);
      R32 *del_src = (R32*)read_span.start;

      R32 *dest = data->output[channel_idx];
      for(U64 sample_idx = 0; sample_idx < sample_count; ++sample_idx)
      {
	dest[sample_idx] = 0.5f*(src[sample_idx] + del_src[sample_idx]);
      }

      os_ring_buffer_read_end(prev_samples, sample_count*sizeof(R32));
    }
  }
}

typedef DelayLine CombFilter;

proc void
comb_filter_process(Audio_ProcessData *data)
{
  CombFilter *comb_filter = data->user_data;
  U64 sample_count = data->sample_count;

  for(U32 channel_idx = 0; channel_idx < ArrayCount(data->input); ++channel_idx)
  {
    Os_RingBuffer *prev_samples = &comb_filter->prev_samples[channel_idx];

    R32 *src = data->input[channel_idx];

    Os_RingBufferSpan read_span = os_ring_buffer_read_span(prev_samples);
    R32 *del_src = (R32*)read_span.start;

    Os_RingBufferSpan write_span = os_ring_buffer_write_span(prev_samples);
    R32 *del_dest = (R32*)write_span.start;

    R32 *dest = data->output[channel_idx];
    for(U64 sample_idx = 0; sample_idx < sample_count; ++sample_idx)
    {
      R32 out_sample = 0.5f*(src[sample_idx] + del_src[sample_idx]);
      del_dest[sample_idx] = out_sample;
      dest[sample_idx] = out_sample;
    }

    os_ring_buffer_write_end(prev_samples, sample_count*sizeof(R32));
    os_ring_buffer_read_end(prev_samples, sample_count*sizeof(R32));
  }
}

typedef struct AllpassFilter
{
  Os_RingBuffer ff_samples[2];
  Os_RingBuffer fb_samples[2];
  U64 order;
} AllpassFilter;

proc void
allpass_filter_process(Audio_ProcessData *data)
{
  AllpassFilter *allpass_filter = data->user_data;
  U64 sample_count = data->sample_count;

  for(U32 channel_idx = 0; channel_idx < ArrayCount(data->input); ++channel_idx)
  {
    Os_RingBuffer *ff_samples = &allpass_filter->ff_samples[channel_idx];
    Os_RingBuffer *fb_samples = &allpass_filter->fb_samples[channel_idx];

    // NOTE: fill feedforward buffer
    {
      R32 *src = data->input[channel_idx];

      Os_RingBufferSpan write_span = os_ring_buffer_write_span(ff_samples);
      R32 *dest = (R32*)write_span.start;

      CopyArray(dest, src, R32, sample_count);
      os_ring_buffer_write_end(ff_samples, sample_count*sizeof(R32));
    }

    // NOTE: write to output, fill feedback buffer
    {
      R32 *src = data->input[channel_idx];

      Os_RingBufferSpan ff_read_span = os_ring_buffer_read_span(ff_samples);
      R32 *ff_src = (R32*)ff_read_span.start;

      Os_RingBufferSpan fb_read_span = os_ring_buffer_read_span(fb_samples);
      R32 *fb_src = (R32*)fb_read_span.start;

      Os_RingBufferSpan fb_write_span = os_ring_buffer_write_span(fb_samples);
      R32 *fb_dest = (R32*)fb_write_span.start;

      R32 *dest = data->output[channel_idx];
      for(U64 sample_idx = 0; sample_idx < sample_count; ++sample_idx)
      {
	R32 out_sample = src[sample_idx] + 0.5f*(fb_src[sample_idx] - ff_src[sample_idx]);
	dest[sample_idx] = out_sample;
	fb_dest[sample_idx] = out_sample;
      }

      os_ring_buffer_write_end(fb_samples, sample_count*sizeof(R32));
      os_ring_buffer_read_end(fb_samples, sample_count*sizeof(R32));
      os_ring_buffer_read_end(ff_samples, sample_count*sizeof(R32));
    }
  }
}

typedef struct DelayStream
{
  Audio_Stream self;
  Audio_Stream *source;

  Os_RingBuffer prev_samples;
  U64 delay_samples;
} DelayStream;

proc Audio_StreamStatus
delay_stream_refill(Audio_Stream *self, Audio_Stream *caller)
{
  DelayStream *delay = (DelayStream*)self;
  Os_RingBuffer *prev_samples = &delay->prev_samples;
  Audio_Stream *source = delay->source;

  // NOTE: refill input
  source->sample_cursor = source->samples_end;
  source->refill(source, self);
  U64 samples_to_read = IntFromPtr(source->samples_end - source->sample_cursor);

  // NOTE: fill buffer
  {
    R32 *src = source->samples_start;

    Os_RingBufferSpan write_span = os_ring_buffer_write_span(prev_samples);
    U64 samples_available = IntFromPtr(write_span.end - write_span.start)/sizeof(R32);
    Assert(samples_available >= samples_to_read);
    R32 *dest = (R32*)write_span.start;
    CopyArray(dest, src, R32, samples_to_read);
    os_ring_buffer_write_end(prev_samples, samples_to_read*sizeof(R32));
  }

  // NOTE: write samples
  {
    R32 *src = source->samples_start;

    Os_RingBufferSpan read_span = os_ring_buffer_read_span(prev_samples);
    U64 del_samples_available = IntFromPtr(read_span.end - read_span.start)/sizeof(R32);
    Assert(del_samples_available >= samples_to_read);
    R32 *src_del = (R32*)read_span.start;

    Assert(caller->refill == 0);
    Assert(caller->sample_cursor == caller->samples_start);
    U64 dest_samples_available = IntFromPtr(caller->samples_end - caller->samples_start);
    Assert(dest_samples_available >= samples_to_read);
    R32 *dest = caller->samples_start;
    for(U64 sample_idx = 0; sample_idx < samples_to_read; ++sample_idx)
    {
      dest[sample_idx] = 0.5f*(src[sample_idx] + src_del[sample_idx]);
    }
    os_ring_buffer_read_end(prev_samples, samples_to_read*sizeof(R32));
  }

  return Audio_StreamStatus_ok;
}

proc void
delay_stream_init(DelayStream *stream, Audio_Stream *source, U64 delay_samples)
{
  stream->self.refill = delay_stream_refill;
  stream->source = source;
  os_ring_buffer_init(&stream->prev_samples, 2*delay_samples*sizeof(R32));
  os_ring_buffer_write_end(&stream->prev_samples, delay_samples*sizeof(R32));
  stream->delay_samples = delay_samples;
}

typedef struct CombFilterStream
{
  Audio_Stream self;
  Audio_Stream *source;

  Os_RingBuffer prev_samples;
  U64 delay_samples;
} CombFilterStream;

proc Audio_StreamStatus
comb_filter_stream_refill(Audio_Stream *self, Audio_Stream *caller)
{
  CombFilterStream *comb_filter = (CombFilterStream*)self;
  Os_RingBuffer *prev_samples = &comb_filter->prev_samples;
  Audio_Stream *source = comb_filter->source;

  // NOTE: refill
  source->sample_cursor = source->samples_end;
  source->refill(source, self);
  U64 samples_to_read = source->samples_end - source->sample_cursor;

  R32 *src = source->sample_cursor;

  Os_RingBufferSpan write_span = os_ring_buffer_write_span(prev_samples);
  //U64 write_samples_available = (write_span.end - write_span.start)/sizeof(R32);
  R32 *dest_del = (R32*)write_span.start;

  Os_RingBufferSpan read_span = os_ring_buffer_read_span(prev_samples);
  U64 read_samples_available = (write_span.end - write_span.start)/sizeof(R32);
  Assert(read_samples_available >= samples_to_read);
  R32 *src_del = (R32*)read_span.start;

  Assert(caller->refill == 0);
  U64 dest_samples_available = caller->samples_end - caller->sample_cursor;
  Assert(dest_samples_available >= samples_to_read);
  R32 *dest = caller->sample_cursor;
  for(U64 sample_idx = 0; sample_idx < samples_to_read; ++sample_idx)
  {
    R32 out_sample = 0.5f*(src[sample_idx] + src_del[sample_idx]);
    dest_del[sample_idx] = out_sample;
    dest[sample_idx] = out_sample;
  }
  os_ring_buffer_write_end(prev_samples, samples_to_read*sizeof(R32));
  os_ring_buffer_read_end(prev_samples, samples_to_read*sizeof(R32));

  return Audio_StreamStatus_ok;
}

proc void
comb_filter_stream_init(CombFilterStream *stream, Audio_Stream *source, U64 delay_samples)
{
  stream->self.refill = comb_filter_stream_refill;
  stream->source = source;
  os_ring_buffer_init(&stream->prev_samples, 2*delay_samples*sizeof(R32));
  os_ring_buffer_write_end(&stream->prev_samples, delay_samples*sizeof(R32));
  stream->delay_samples = delay_samples;
}

typedef struct AllpassFilterStream
{
  Audio_Stream self;
  Audio_Stream *source;

  Os_RingBuffer ff_samples;
  Os_RingBuffer fb_samples;
  U64 order;

  // TODO: avoid, in case of being connected to output?
  Os_RingBuffer dest_samples;
} AllpassFilterStream;

proc Audio_StreamStatus
allpass_filter_stream_refill(Audio_Stream *self, Audio_Stream *caller)
{
  AllpassFilterStream *allpass_filter = (AllpassFilterStream*)self;

  Audio_Stream *source = allpass_filter->source;
  Os_RingBuffer *ff_samples = &allpass_filter->ff_samples;
  Os_RingBuffer *fb_samples = &allpass_filter->fb_samples;
  Os_RingBuffer *dest_samples = &allpass_filter->dest_samples;

  Assert(caller);
  B32 caller_is_output = caller->refill == 0;

  // NOTE: pull from source
  source->sample_cursor = source->samples_end;
  source->refill(source, self);
  U64 samples_to_read = IntFromPtr(source->samples_end - source->samples_start);

  // NOTE: advance read cursor
  if(!caller_is_output)
  {
    U64 samples_read = IntFromPtr(self->sample_cursor - self->samples_start);
    os_ring_buffer_read_end(dest_samples, samples_read*sizeof(R32));
  }

  // NOTE: fill feedforward buffer
  {
    R32 *src = source->samples_start;

    Os_RingBufferSpan write_span = os_ring_buffer_write_span(ff_samples);
    R32 *dest = (R32*)write_span.start;

    CopyArray(dest, src, R32, samples_to_read);
    os_ring_buffer_write_end(ff_samples, samples_to_read*sizeof(R32));
  }

  // NOTE: write to dest, fill feedback buffer
  {
    R32 *src = source->samples_start;

    Os_RingBufferSpan ff_read_span = os_ring_buffer_read_span(ff_samples);
    R32 *ff_src = (R32*)ff_read_span.start;

    Os_RingBufferSpan fb_read_span = os_ring_buffer_read_span(fb_samples);
    R32 *fb_src = (R32*)fb_read_span.start;

    Os_RingBufferSpan fb_write_span = os_ring_buffer_write_span(fb_samples);
    R32 *fb_dest = (R32*)fb_write_span.start;

    R32 *dest;
    if(caller_is_output)
    {
      dest = caller->samples_start;
    }
    else
    {
      Os_RingBufferSpan dest_span = os_ring_buffer_write_span(dest_samples);
      dest = (R32*)dest_span.start;
    }

    for(U64 sample_idx = 0; sample_idx < samples_to_read; ++sample_idx)
    {
      R32 out_sample = src[sample_idx] + 0.5f*(fb_src[sample_idx] - ff_src[sample_idx]);
      fb_dest[sample_idx] = out_sample;
      dest[sample_idx] = out_sample;
    }

    if(!caller_is_output)
    { os_ring_buffer_write_end(dest_samples, samples_to_read*sizeof(R32)); }
    os_ring_buffer_write_end(fb_samples, samples_to_read*sizeof(R32));
    os_ring_buffer_read_end(fb_samples, samples_to_read*sizeof(R32));
    os_ring_buffer_read_end(ff_samples, samples_to_read*sizeof(R32));
  }

  // NOTE: expose buffer
  if(!caller_is_output)
  {
    Os_RingBufferSpan read_span = os_ring_buffer_read_span(dest_samples);
    U64 samples_available = (read_span.end - read_span.start)/sizeof(R32);
    self->samples_start = (R32*)read_span.start;
    self->samples_end = self->samples_start + samples_available;
    self->sample_cursor = self->samples_start;
  }

  return Audio_StreamStatus_ok;
}

proc void
allpass_filter_stream_init(AllpassFilterStream *stream, Audio_Stream *source, U64 delay_samples)
{
  stream->self.refill = allpass_filter_stream_refill;
  stream->source = source;
  os_ring_buffer_init(&stream->dest_samples, 2048*sizeof(R32));
  os_ring_buffer_init(&stream->ff_samples, 2*delay_samples*sizeof(R32));
  os_ring_buffer_init(&stream->fb_samples, 2*delay_samples*sizeof(R32));
  os_ring_buffer_write_end(&stream->ff_samples, delay_samples*sizeof(R32));
  os_ring_buffer_write_end(&stream->fb_samples, delay_samples*sizeof(R32));
  stream->order = delay_samples;
}

void
audio_process(Audio_ProcessData *data)
{
  /* delay_process(data); */
  /* comb_filter_process(data); */
  allpass_filter_process(data);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!os_init())
  { goto failure; }

  if(!audio_init())
  { goto failure; }

  for(Audio_DeviceIterator it = audio_device_iterator();
      !audio_device_iterator_done(it);
      it = audio_device_iterator_next(it))
  {
    String8 device_name = audio_device_name(it.device);
    printf("name: %.*s\n", (int)device_name.count, device_name.string);
  }

  String8 default_input_name = audio_device_name(audio_default_input_device());
  printf("input device: %.*s\n", (int)default_input_name.count, default_input_name.string);

  String8 default_output_name = audio_device_name(audio_default_output_device());
  printf("output device: %.*s\n", (int)default_output_name.count, default_output_name.string);

  U64 delay_samples = 960;

  DelayStream delay_l = {0};
  DelayStream delay_r = {0};
  delay_stream_init(&delay_l, audio_stream_get_input(0), delay_samples);
  delay_stream_init(&delay_r, audio_stream_get_input(1), delay_samples);

  CombFilterStream comb_filter_l = {0};
  CombFilterStream comb_filter_r = {0};
  comb_filter_stream_init(&comb_filter_l, audio_stream_get_input(0), delay_samples);
  comb_filter_stream_init(&comb_filter_r, audio_stream_get_input(1), delay_samples);

  AllpassFilterStream allpass_filter_l = {0};
  AllpassFilterStream allpass_filter_r = {0};
  allpass_filter_stream_init(&allpass_filter_l, audio_stream_get_input(0), delay_samples);
  allpass_filter_stream_init(&allpass_filter_r, audio_stream_get_input(1), delay_samples);

  /* audio_stream_connect_output(&delay_l.self, 0); */
  /* audio_stream_connect_output(&delay_r.self, 1); */

  audio_stream_connect_output(&comb_filter_l.self, 0);
  audio_stream_connect_output(&comb_filter_r.self, 1);

  /* audio_stream_connect_output(&allpass_filter_l.self, 0); */
  /* audio_stream_connect_output(&allpass_filter_r.self, 1); */

  audio_start();
  while(1) {}
  audio_stop();

  return 0;

failure:
  return 1;
}
