#include "base/base.h"
#include "audio/audio.h"

#include "base/base.c"
#include "audio/audio.c"

typedef struct DelayLine
{
  Os_RingBuffer prev_samples[2];
  U64 delay_samples;
} DelayLine;

void
audio_process(Audio_ProcessData *data)
{
  DelayLine *delay_line = data->user_data;
  U64 sample_count = data->sample_count;

  Os_RingBuffer *prev_samples0 = &delay_line->prev_samples[0];
  Os_RingBuffer *prev_samples1 = &delay_line->prev_samples[1];

  // NOTE: fill buffer
  {
    R32 *src0 = data->input[0];
    R32 *src1 = data->input[1];

    Assert(sample_count <= os_ring_buffer_free(prev_samples0));
    Assert(sample_count <= os_ring_buffer_free(prev_samples1));
    Os_RingBufferSpan write_span0 = os_ring_buffer_write_span(prev_samples0);
    Os_RingBufferSpan write_span1 = os_ring_buffer_write_span(prev_samples1);
    R32 *dest0 = (R32*)write_span0.start;
    R32 *dest1 = (R32*)write_span1.start;

    CopyArray(dest0, src0, R32, sample_count);
    CopyArray(dest1, src1, R32, sample_count);
    os_ring_buffer_write_end(prev_samples0, sample_count*sizeof(R32));
    os_ring_buffer_write_end(prev_samples1, sample_count*sizeof(R32));
  }

  // NOTE: write to output
  {
    R32 *src0 = data->input[0];
    R32 *src1 = data->input[1];

    Assert(sample_count <= os_ring_buffer_used(prev_samples0));
    Assert(sample_count <= os_ring_buffer_used(prev_samples1));
    Os_RingBufferSpan read_span0 = os_ring_buffer_read_span(prev_samples0);
    Os_RingBufferSpan read_span1 = os_ring_buffer_read_span(prev_samples1);
    R32 *del_src0 = (R32*)read_span0.start;
    R32 *del_src1 = (R32*)read_span1.start;

    R32 *dest0 = data->output[0];
    R32 *dest1 = data->output[1];
    for(U64 sample_idx = 0; sample_idx < sample_count; ++sample_idx)
    {
      dest0[sample_idx] = 0.5f*(src0[sample_idx] + del_src0[sample_idx]);
      dest1[sample_idx] = 0.5f*(src1[sample_idx] + del_src1[sample_idx]);
    }

    os_ring_buffer_read_end(prev_samples0, sample_count*sizeof(R32));
    os_ring_buffer_read_end(prev_samples1, sample_count*sizeof(R32));
  }
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
  DelayLine delay_line = {0};
  os_ring_buffer_init(&delay_line.prev_samples[0], 2*delay_samples*sizeof(R32));
  os_ring_buffer_init(&delay_line.prev_samples[1], 2*delay_samples*sizeof(R32));
  os_ring_buffer_write_end(&delay_line.prev_samples[0], delay_samples*sizeof(R32));
  os_ring_buffer_write_end(&delay_line.prev_samples[1], delay_samples*sizeof(R32));
  Assert(delay_line.prev_samples[0].size == delay_line.prev_samples[1].size);
  printf("capacity: %llu samples\n", delay_line.prev_samples[0].size / sizeof(R32));
  delay_line.delay_samples = delay_samples;
  audio_set_process_data(&delay_line);

  audio_start();
  while(1) {}
  audio_stop();

  return 0;

failure:
  return 1;
}
