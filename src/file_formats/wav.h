#define RIFF(str) FOURCC(str)

typedef union RiffId
{
  U32   id;
  U8    str[4];
} RiffId;

#pragma pack(push, 1)
typedef struct RiffHeader
{
  RiffId        chunk_id;
  U32           chunk_size;
} RiffHeader;

typedef struct WaveHeader
{
  RiffId        wave_id;
} WaveHeader;

typedef struct WaveFormatChunk
{
  RiffHeader    header;
  U16           format_tag;
  U16           channel_count;
  U32           sample_rate;
  U32           avg_bytes_per_sec;
  U16           data_block_size;
  U16           bits_per_sample;
} WaveFormatChunk;

typedef struct WaveFormatExtension
{
  U16   cb_size;
  U16   valid_bits_per_sample;
  U32   channel_mask;
  U8    sub_fmt[16];
} WaveFormatExtension;

typedef struct WaveFormatExtended
{
  WaveFormatChunk       fmt;
  WaveFormatExtension   ex;
} WaveFormatExtended;

#pragma pack(pop)

typedef RiffHeader WaveDataChunk;

typedef enum WaveFormat
{
  WaveFormat_pcm        = 0x0001,
  WaveFormat_ieee_float = 0x0003,
  WaveFormat_alaw       = 0x0006,
  WaveFormat_mulaw      = 0x0007,
  WaveFormat_extensible = 0xFFFE,
} WaveFormat;

typedef enum WavSampleKind
{
  WavSampleKind_S16,
  WavSampleKind_R32,
  WavSampleKind_Count,
} WavSampleKind;

global U32 wav_sample_sizes[WavSampleKind_Count] = {
  [WavSampleKind_S16] = sizeof(S16),
  [WavSampleKind_R32] = sizeof(R32),
};

global WaveFormat wav_sample_formats[WavSampleKind_Count] = {
  [WavSampleKind_S16] = WaveFormat_pcm,
  [WavSampleKind_R32] = WaveFormat_ieee_float,
};

/* typedef struct WavChunkNode WavChunkNode; */
/* struct WavChunkNode */
/* { */
/*   WavChunkNode *next; */
/*   U64 frames_size; */
/*   void *frames; */
/* }; */

/* typedef struct WavChunkList */
/* { */
/*   WavChunkNode *first; */
/*   WavChunkNode *last; */
/*   U64 count; */
/*   U64 total_frames_size; */
/* } WavChunkList; */

typedef struct WavWriter
{
  Arena *arena;

  RiffHeader *riff_header;
  WaveHeader *wave_header;
  WaveFormatChunk *fmt_chunk;
  WaveDataChunk *data_chunk;

  U32 frame_size;
  U32 total_frames_size;
} WavWriter;

proc inline U32
wav_sample_size(WavSampleKind kind)
{
  U32 result = wav_sample_sizes[kind];
  return(result);
}

proc inline WaveFormat
wav_sample_format(WavSampleKind kind)
{
  WaveFormat result = wav_sample_formats[kind];
  return(result);
}

proc WavWriter* wav_begin(Arena *arena, U32 sample_rate, U32 channel_count, WavSampleKind sample_kind);
// TODO: it would be awesome if it were possible to auto-cast the result here
proc void* wav_push_chunk(WavWriter *wav, U64 frame_count);
proc void wav_end(WavWriter *wav, String8 file_path);

/* proc WavWriter* begin_wav(U32 sample_rate, U32 channel_count, WavSampleKind sample_kind); */
/* proc void wav_push_chunk(WavWriter *writer, U64 frame_count, void *frames); */
/* proc void end_wav(WavWriter *writer, String8 file_path); */
