proc WavWriter*
begin_wav(U32 sample_rate, U32 channel_count, WavSampleKind sample_kind)
{
  Arena *arena = arena_alloc();
  WavWriter *writer = arena_push_struct(arena, WavWriter);
  writer->arena = arena;
  writer->sample_rate = sample_rate;
  writer->channel_count = channel_count;
  writer->sample_format = wav_sample_format(sample_kind);
  writer->sample_size = wav_sample_size(sample_kind);
  writer->frame_size = writer->channel_count * writer->sample_size;

  return(writer);
}

proc void
wav_push_chunk(WavWriter *writer, U64 frame_count, void *frames)
{
  U64 frames_size = frame_count * writer->frame_size;
  WavChunkNode *node = arena_push_struct(writer->arena, WavChunkNode);
  node->frames_size = AlignPow2(frames_size, 2);
  node->frames = arena_push_array(writer->arena, U8, node->frames_size);
  CopyArray(node->frames, frames, U8, frames_size);
  
  SLLQueuePush(writer->chunk_list.first, writer->chunk_list.last, node);
  ++writer->chunk_list.count;
  writer->chunk_list.total_frames_size += node->frames_size;
}

proc void
end_wav(WavWriter *writer, String8 file_path)
{
  U64 header_sizes = sizeof(RiffHeader) + sizeof(WaveHeader) + sizeof(WaveFormatChunk);
  U64 wav_size = header_sizes + sizeof(WaveDataChunk) + writer->chunk_list.total_frames_size;
  PushBuffer wav_buffer = push_buffer_alloc(writer->arena, wav_size);
  
  RiffHeader *riff_header = buf_push_struct(&wav_buffer, RiffHeader);
  riff_header->chunk_id.id = RIFF("RIFF");
  riff_header->chunk_size = (U32)(wav_size - sizeof(RiffHeader));
  
  WaveHeader *wave_header = buf_push_struct(&wav_buffer, WaveHeader);
  wave_header->wave_id.id = RIFF("WAVE");
  
  WaveFormatChunk *wave_fmt = buf_push_struct(&wav_buffer, WaveFormatChunk);
  wave_fmt->header.chunk_id.id = RIFF("fmt ");
  wave_fmt->header.chunk_size = sizeof(WaveFormatChunk) - sizeof(RiffHeader);
  wave_fmt->format_tag = writer->sample_format;
  wave_fmt->channel_count = writer->channel_count;
  wave_fmt->sample_rate = writer->sample_rate;
  wave_fmt->avg_bytes_per_sec = writer->sample_rate * writer->frame_size;
  wave_fmt->data_block_size = writer->frame_size;
  wave_fmt->bits_per_sample = 8*writer->sample_size;
  wave_fmt->cb_size = 0;

  WaveDataChunk *wave_data = buf_push_struct(&wav_buffer, WaveDataChunk);
  wave_data->chunk_id.id = RIFF("data");
  wave_data->chunk_size = (U32)writer->chunk_list.total_frames_size;

  U64 sample_data_at = 0;
  U8 *sample_data = buf_push_array(&wav_buffer, U8, wave_data->chunk_size);
  for(WavChunkNode *chunk = writer->chunk_list.first; chunk; chunk = chunk->next) {
    U8 *dest = sample_data + sample_data_at;
    CopyArray(dest, chunk->frames, U8, chunk->frames_size);
    sample_data_at += chunk->frames_size;
  }

  Os_Handle file = os_file_open(file_path, Os_FileOpenFlag_write);
  Buffer wav_contents = {.size = wav_buffer.pos, .mem = wav_buffer.mem};
  B32 result = os_file_write(wav_contents, file);
  arena_release(writer->arena);
}
