/**
 * TODO:
 * make the rendered spectrum not blink
 * the rendered spectrum should have sharper peaks, figure out why it doesn't
 * compare the rendered spectrum against a known implementation
 * accumulate spectrum data over time to present as spectrogram, with option of viewing each window as a spectrum
 * pull out the renering code into a generalized renderer
 * pull out the opengl code for use across programs/modules
 * give glyphs transparent backgrounds
 * get the sample rate from the audio backend
 */

#include "base/base.h"
#include "os/os.h"

#include "wayland.h"

#include "font/font.h"
#include "audio/audio.h"
#include "fourier/fourier.h"

#include "base/base.c"
#include "os/os.c"
#include "font/font.c"
#include "audio/audio.c"
#include "fourier/fourier.c"

#include "wayland.c"

//
// audio
//
typedef struct SineOscState
{
  R32 phasor;
  R32 base_freq;
  R32 pitch_bend;
  R32 volume;
} SineOscState;

typedef struct AudioBuffer
{
  U64 sample_count;
  R32 *samples[2];
} AudioBuffer;

typedef struct AudioBufferNode AudioBufferNode;
struct AudioBufferNode
{
  AudioBufferNode *next;
  AudioBuffer buf;
};

typedef struct AudioBufferList
{
  Arena *arena;

  U32 lock;

  AudioBufferNode *first;
  AudioBufferNode *last;
  U64 count;

  AudioBufferNode *freelist;
} AudioBufferList;

global AudioBufferList audio_buffer_list = {0};

void
audio_process(Audio_ProcessData *data)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);

  SineOscState *sine_state = data->user_data;
  U32 sample_count = data->sample_count;

  if(sine_state) {
    // NOTE: react to midi messages
    R32 base_freq = sine_state->base_freq;
    R32 pitch_bend = sine_state->pitch_bend;
    R32 *freq_buffer = arena_push_array(scratch.arena, R32, sample_count);
    for(U32 sample_idx = 0; sample_idx < sample_count; ++sample_idx) {

      Audio_MidiMessage *midi_msg = audio_next_midi_message(data);
      if(midi_msg) {
	if(midi_msg->sample_idx >= sample_idx) {

	  switch(midi_msg->opcode) {
	  
	  case MidiOpcode_note_on: {
	    U32 midi_note = audio_midi_get_note_number(midi_msg);
	    base_freq = audio_hertz_from_midi_note(midi_note);
	  } break;
	  
	  case MidiOpcode_pitch_bend: {
	    pitch_bend = audio_midi_get_pitch_bend(midi_msg);
	  } break;
	  
	  default: {} break;
	  }

	  audio_dequeue_midi_message(data);
	}
      }

      // TODO: smooth out pitch bend changes
      freq_buffer[sample_idx] = base_freq * pitch_bend;
    }

    sine_state->base_freq = base_freq;
    sine_state->pitch_bend = pitch_bend;
  
    // NOTE: compute audio samples
    R32 *mix_buffer = arena_push_array_z(scratch.arena, R32, sample_count);
    for(U32 sample_idx = 0; sample_idx < sample_count; ++sample_idx) {

      R32 step = TAU32 * freq_buffer[sample_idx] * data->sample_period;
      sine_state->phasor += step;
      if(sine_state->phasor > TAU32) {
	sine_state->phasor -= TAU32;
      }

      R32 sine_sample = sine_state->volume * sinf(sine_state->phasor);
      mix_buffer[sample_idx] += sine_sample;
    }

    // NOTE: output samples
    CopyArray(data->output[0], mix_buffer, R32, sample_count);
    CopyArray(data->output[1], mix_buffer, R32, sample_count);

    // TODO: queue audio buffer
    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 0, 1)) {}
    {
      AudioBufferNode *node = 0;
      if(audio_buffer_list.freelist) {

	node = audio_buffer_list.freelist;
	Assert(node->buf.sample_count == sample_count);
	SLLStackPop(audio_buffer_list.freelist);
      }else {

	node = arena_push_struct(audio_buffer_list.arena, AudioBufferNode);
	node->buf.sample_count = sample_count;
	node->buf.samples[0] = arena_push_array(audio_buffer_list.arena, R32, sample_count);
	node->buf.samples[1] = arena_push_array(audio_buffer_list.arena, R32, sample_count);
      }

      CopyArray(node->buf.samples[0], mix_buffer, R32, sample_count);
      CopyArray(node->buf.samples[1], mix_buffer, R32, sample_count);

      SLLQueuePush(audio_buffer_list.first, audio_buffer_list.last, node);
      ++audio_buffer_list.count;
    }
    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 1, 0)) {}
  }
  
  arena_release_scratch(scratch);
}

//
// rendering
//
// TODO: should there be two of these, for each endianness?
proc U32
color_u32_from_v4(V4 color)
{
  U32 result = ((((U32)(color.r*255.f)) << 0*8) |
		(((U32)(color.g*255.f)) << 1*8) |
		(((U32)(color.b*255.f)) << 2*8) |
		(((U32)(color.a*255.f)) << 3*8));
  return(result);
}

global char vert_shader_src[] =
  "#version 330\n"
  "layout (location = 0) in vec3 v_p;\n"
  "layout (location = 1) in vec2 v_uv;\n"
  "layout (location = 2) in vec4 v_c;\n"
  "out vec2 f_uv;\n"
  "out vec4 f_c;\n"
  "void main() {\n"
  "gl_Position = vec4(v_p, 1);\n"
  "f_uv = v_uv;\n"
  "f_c = v_c;\n"  
  "}\n";

global char frag_shader_src[] =
  "#version 330\n"
  "in vec2 f_uv;\n"
  "in vec4 f_c;\n"
  "uniform sampler2D atlas;\n"
  "out vec4 out_color;\n"
  "void main() {\n"
  "vec4 sampled = texture(atlas, f_uv);\n"
  "out_color = f_c * vec4(sampled.r, sampled.r, sampled.r, 1);\n"
  "}\n";

typedef struct GlShader
{
  GLuint handle;
  String8 log;
} GlShader;

proc GlShader
gl_make_shader(Arena *arena, char *src, GLenum kind)
{  
  GLuint shader_id = glCreateShader(kind);
  glShaderSource(shader_id, 1, (const GLchar*const*)&src, 0);
  glCompileShader(shader_id);

  String8 log = {0};
  GLint info_log_length;
  glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length) {
    
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetShaderInfoLog(shader_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint compile_status;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
  if(compile_status == 0) {
    glDeleteShader(shader_id);
    shader_id = 0;
  }

  GlShader result = {0};
  result.handle = shader_id;
  result.log = log;
  return(result);
}

proc GlShader
gl_make_program(Arena *arena, GLuint *shaders, U32 shader_count)
{
  GLuint program_id = glCreateProgram();
  for(U32 shader_idx = 0; shader_idx < shader_count; ++shader_idx)
    { glAttachShader(program_id, shaders[shader_idx]); }
  glLinkProgram(program_id);

  String8 log = {0};
  GLint info_log_length;
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
  if(info_log_length) {
    
    char *buffer = arena_push_array(arena, char, info_log_length + 1);
    GLint len = 0;
    glGetProgramInfoLog(program_id, info_log_length + 1, &len, buffer);
    log.count = len;
    log.string = (U8*)buffer;
  }

  GLint link_status;
  glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);
  if(link_status == 0) {
    glDeleteProgram(program_id);
    program_id = 0;
  }  

  GlShader result = {0};
  result.handle = program_id;
  result.log = log;
  return(result);
}

typedef struct R_Vertex
{
  V3 pos;
  V2 uv;
  U32 color;
} R_Vertex;

typedef struct R_Texture
{
  GLuint handle;
  V2S32 dim;
} R_Texture;

typedef struct R_Font
{
  PackedFont *font;
  R_Texture *atlas;
} R_Font;

typedef struct R_Batch R_Batch;
struct R_Batch
{
  R_Batch *next;

  U32 vertex_cap;
  U32 vertex_count;
  R_Vertex *vertex_buffer;
  
  R_Texture *texture;
};

typedef struct R_Commands
{
  Arena *arena;

  V2S32 window_dim;
  V2 ndc_scale;

  R_Batch *first_batch;
  R_Batch *last_batch;
  U32 batch_count;

  R_Texture *white_texture;

  R_Batch *batch_freelist;

  GLuint vao;
  GLuint vbo;
  GLuint sampler;
  GLint sampler_loc;
  
  GlShader vert_shader;
  GlShader frag_shader;
  GlShader shader_prog;
} R_Commands;

typedef enum RenderLevel
{
  RenderLevel_top,
  RenderLevel_label,
  RenderLevel_grid,
  RenderLevel_signal,
  RenderLevel_background,
  RenderLevel_count,
} RenderLevel;

proc R_Font*
render_alloc_font(Arena *arena, PackedFont *font)
{
  R_Texture *atlas = arena_push_struct(arena, R_Texture);
  atlas->handle = font->atlas_texture_id;
  atlas->dim = v2s32(font->atlas_width, font->atlas_height);
      
  R_Font *result = arena_push_struct(arena, R_Font);      
  result->font = font;
  result->atlas = atlas;
  return(result);
}

proc R_Commands*
render_alloc_commands(Arena *arena)
{    
  R_Commands *result = arena_push_struct_z(arena, R_Commands);
  result->arena = arena;

  // NOTE: generate white texture
  result->white_texture = arena_push_struct(result->arena, R_Texture);  
  glGenTextures(1, &result->white_texture->handle);
  result->white_texture->dim = v2s32(16, 16);
  glBindTexture(GL_TEXTURE_2D, result->white_texture->handle);
  GlTextureDefaultParams();
  U8 white[256];
  SetSize(white, U8_MAX, ArrayCount(white));
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 16, 16, 0, GL_RED, GL_UNSIGNED_BYTE, white);

  // NOTE: compile and link shaders
  ArenaTemp scratch = arena_get_scratch(&arena, 1);
  result->vert_shader = gl_make_shader(scratch.arena, vert_shader_src, GL_VERTEX_SHADER);
  if(result->vert_shader.log.count) {
    fprintf(stderr, "ERROR: vert_shader: %.*s\n",
	    (int)result->vert_shader.log.count, result->vert_shader.log.string);
  }
  
  result->frag_shader = gl_make_shader(scratch.arena, frag_shader_src, GL_FRAGMENT_SHADER);
  if(result->frag_shader.log.count) {
    fprintf(stderr, "ERROR: frag_shader: %.*s\n",
	    (int)result->frag_shader.log.count, result->frag_shader.log.string);
  }

  GLuint shaders[] = {
    result->vert_shader.handle,
    result->frag_shader.handle,
  };
  result->shader_prog = gl_make_program(scratch.arena, shaders, ArrayCount(shaders));
  if(result->shader_prog.log.count) {
    fprintf(stderr, "ERROR: shader_program: %.*s\n",
	    (int)result->shader_prog.log.count, result->shader_prog.log.string);
  }

  glUseProgram(result->shader_prog.handle);

  // NOTE: generate arrays, samplers, uniforms
  glGenVertexArrays(1, &result->vao);
  glBindVertexArray(result->vao);

  glGenBuffers(1, &result->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, result->vbo);

  glGenSamplers(1, &result->sampler);
  result->sampler_loc = glGetUniformLocation(result->shader_prog.handle, "atlas");
  glUniform1i(result->sampler_loc, 0);

  arena_release_scratch(scratch);
  return(result);
}

proc void
render_begin_frame(R_Commands *commands, S32 window_width, S32 window_height)
{
  commands->window_dim = v2s32(window_width, window_height);
  commands->ndc_scale = v2(2.f/(R32)window_width, 2.f/(R32)window_height);
}

proc void
render_rect(R_Commands *commands, R_Texture *texture, Rect2 rect, Rect2 uv, RenderLevel render_level, V4 color)
{
  // NOTE: if a texture was not supplied, use the white texture
  if(!texture) {
    texture = commands->white_texture;
    uv = rect2(v2(0, 0), v2(1, 1));
  }

  // NOTE: check if texture is already used in a batch
  R_Batch *batch = 0;
  for(R_Batch *current_batch = commands->first_batch; current_batch; current_batch = current_batch->next) {

    if(texture->handle == current_batch->texture->handle) {

      batch = current_batch;
      break;
    }
  }

  // NOTE: if there is not already a batch with this texture, allocate a new one and add it to the list
  if(!batch) {
    if(commands->batch_freelist) {
      // NOTE: pull a batch off of the freelist
      batch = commands->batch_freelist;
      batch->vertex_count = 0;
      SLLStackPop(commands->batch_freelist);
    }else {
      // NOTE: push a new batch onto the arena
      batch = arena_push_struct_z(commands->arena, R_Batch);
      batch->vertex_cap = KB(32);
      batch->vertex_buffer = arena_push_array_z(commands->arena, R_Vertex, batch->vertex_cap);      
    }
    batch->texture = texture;
    SLLQueuePush(commands->first_batch, commands->last_batch, batch);
    ++commands->batch_count;
  }

  // TODO: allocate a new batch if we go over capacity
  Assert(batch->vertex_count + 6 <= batch->vertex_cap);

  R32 level = (R32)render_level/(R32)RenderLevel_count;
  U32 color_u32 = color_u32_from_v4(color);
  R_Vertex v00 = {0};
  v00.pos = v3(rect.min.x, rect.min.y, level);
  v00.uv = uv.min;
  v00.color = color_u32;
  R_Vertex v01 = {0};
  v01.pos = v3(rect.min.x, rect.max.y, level);
  v01.uv = v2(uv.min.x, uv.max.y);
  v01.color = color_u32;
  R_Vertex v10 = {0};
  v10.pos = v3(rect.max.x, rect.min.y, level);
  v10.uv = v2(uv.max.x, uv.min.y);
  v10.color = color_u32;
  R_Vertex v11 = {0};
  v11.pos = v3(rect.max.x, rect.max.y, level);
  v11.uv = uv.max;
  v11.color = color_u32;

  R_Vertex *verts = batch->vertex_buffer + batch->vertex_count;
  verts[0] = v00;
  verts[1] = v10;
  verts[2] = v11;
  verts[3] = v00;
  verts[4] = v11;
  verts[5] = v01;
  batch->vertex_count += 6;
}

proc void
render_string(R_Commands *commands, R_Font *font, String8 string, V2 pos, V4 color)
{
  V2 ndc_scale = v2(2.f/(R32)commands->window_dim.x,
		    2.f/(R32)commands->window_dim.y);

  for(U32 char_idx = 0; char_idx < string.count; ++char_idx) {

    U8 c = string.string[char_idx];
    PackedGlyph *glyph = font_glyph_from_codepoint(font->font, c);

    Rect2 rect = rect2(v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.min)),
		       v2_add(pos, v2_hadamard(ndc_scale, glyph->rect.max)));
    render_rect(commands, font->atlas, rect, glyph->uv, RenderLevel_label, color);
    
    pos.x += ndc_scale.x * glyph->advance;
  }
}

proc void
render_from_commands(R_Commands *commands)
{
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(R_Vertex),
			PtrFromInt(OffsetOf(R_Vertex, pos)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(R_Vertex),
			PtrFromInt(OffsetOf(R_Vertex, uv)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, 1, sizeof(R_Vertex),
			PtrFromInt(OffsetOf(R_Vertex, color)));

  // NOTE: render all batches
  for(R_Batch *batch = commands->first_batch; batch; batch = batch->next) {
    
    glBufferData(GL_ARRAY_BUFFER, batch->vertex_count*sizeof(R_Vertex), batch->vertex_buffer, GL_STREAM_DRAW);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch->texture->handle);

    glDrawArrays(GL_TRIANGLES, 0, batch->vertex_count);
  }

  // NOTE: push all batches onto the freelist
  commands->last_batch->next = commands->batch_freelist;
  commands->batch_freelist = commands->first_batch;
  commands->first_batch = 0;
  commands->last_batch = 0;
  commands->batch_count = 0;
}

typedef struct SpectrogramState
{
  Arena *arena;
  
  SineOscState *sine_osc_state;

  RangeR32 freq_rng;
  RangeR32 db_rng;

  U32 sample_rate;

  FloatBuffer spectrum_cache;
} SpectrogramState;

proc SpectrogramState*
spectrogram_state_alloc(Arena *arena)
{
  SpectrogramState *result = arena_push_struct(arena, SpectrogramState);
  result->arena = arena;
  
  result->sine_osc_state = arena_push_struct(arena, SineOscState);
  result->sine_osc_state->phasor = 0.f;
  result->sine_osc_state->base_freq = 440.f;
  result->sine_osc_state->pitch_bend = 1.f;
  result->sine_osc_state->volume = 0.1f;

  result->freq_rng.min = 2.f;
  result->freq_rng.max = 20000.f;

  result->db_rng.min = -60.f;
  result->db_rng.max = 0.f;

  result->sample_rate = 48000;

  U32 spectrum_cache_count = 1024;
  result->spectrum_cache.count = spectrum_cache_count;
  result->spectrum_cache.mem = arena_push_array(arena, R32, spectrum_cache_count);

  return(result);
}

// TODO: make the commands and font globally accessible, or accessible off the spectrogram state
proc void
draw_spectrum_grid(SpectrogramState *spec_state, R_Commands *render_commands, R_Font *font)
{
  ArenaTemp scratch = arena_get_scratch(0, 0);
  
  U32 freq_line_count = (U32)log10f(spec_state->freq_rng.max/spec_state->freq_rng.min) + 1;
  U32 db_line_count = (U32)((spec_state->db_rng.max - spec_state->db_rng.min)/6.f) + 1;

  R32 freq_line_major_advance = 2.f/(R32)(freq_line_count - 1);
  R32 db_line_major_advance = 2.f/(R32)(db_line_count - 1);

  U32 current_freq = (U32)spec_state->freq_rng.min;
  V2 freq_label_pos = v2(-0.99, -0.95f);
  U32 freq_line_thickness_px = 5;
  U32 freq_line_minor_thickness_px = 2;
  R32 freq_line_thickness = (R32)freq_line_thickness_px * render_commands->ndc_scale.x;
  R32 freq_line_minor_thickness = (R32)freq_line_minor_thickness_px * render_commands->ndc_scale.x;
  V2 freq_line_pos = v2(-1, 0);
  V2 freq_line_dim = v2(freq_line_thickness, 2.f);
  V2 freq_line_minor_dim = v2(freq_line_minor_thickness, 2.f);
  for(U32 freq_line_idx = 0; freq_line_idx < freq_line_count; ++freq_line_idx) {

    Rect2 freq_line = rect2_center_dim(freq_line_pos, freq_line_dim);
    render_rect(render_commands, 0, freq_line, rect2_invalid(), RenderLevel_grid, v4(1, 1, 1, 1));

    if(freq_line_idx < freq_line_count - 1) {

      U32 next_freq = current_freq*10;
      R32 sep = ((R32)next_freq - (R32)current_freq) * 0.1f;
      for(U32 minor_line_idx = 1; minor_line_idx < 10; ++minor_line_idx) {

	R32 offset = freq_line_major_advance*log10f(((R32)current_freq + sep*(R32)minor_line_idx)/(R32)current_freq);
	V2 minor_line_pos = v2(freq_line_pos.x + offset, freq_line_pos.y);
	Rect2 minor_line = rect2_center_dim(minor_line_pos, freq_line_minor_dim);
	render_rect(render_commands, 0, minor_line, rect2_invalid(), RenderLevel_grid, v4(1, 1, 1, 1));
      }
    }
    
    String8 freq_label = str8_push_f(scratch.arena, "%u", current_freq);
    render_string(render_commands, font, freq_label, freq_label_pos, v4(1, 1, 1, 1));

    current_freq *= 10;
    freq_line_pos.x += freq_line_major_advance;
    freq_label_pos.x += freq_line_major_advance;
  }

  S32 current_db = (S32)spec_state->db_rng.min;
  V2 db_label_pos = v2(-0.95f, -0.99);
  U32 db_line_thickness_px = 5;
  R32 db_line_thickness = (R32)db_line_thickness_px * render_commands->ndc_scale.y;
  V2 db_line_pos = v2(0, -1);
  V2 db_line_dim = v2(2.f, db_line_thickness); 
  for(U32 db_line_idx = 0; db_line_idx < db_line_count; ++db_line_idx) {

    Rect2 db_line = rect2_center_dim(db_line_pos, db_line_dim);
    render_rect(render_commands, 0, db_line, rect2_invalid(), RenderLevel_grid, v4(1, 1, 1, 1));

    String8 db_label = str8_push_f(scratch.arena, "%d", current_db);
    render_string(render_commands, font, db_label, db_label_pos, v4(1, 1, 1, 1));

    current_db += 6;
    db_line_pos.y += db_line_major_advance;
    db_label_pos.y += db_line_major_advance;    
  }

  arena_release_scratch(scratch);
}

proc void
draw_spectrum(SpectrogramState *spec_state, ComplexBuffer spec_buf, R_Commands *render_commands)
{
  U32 bin_count = spec_buf.count/2;
  R32 width = 2.f/(R32)bin_count;	    
  R32 pos_x = -1.f;

  R32 step = log10f(spec_state->freq_rng.max/spec_state->freq_rng.min)/(R32)bin_count;
  R32 exp = log10f(spec_state->freq_rng.min);
  for(U32 i = 0; i < bin_count; ++i) {

    R32 freq = powf(10.f, exp);

    R32 freq_bin_pos = (freq*(R32)spec_buf.count)/(R32)spec_state->sample_rate;
    U32 freq_bin_idx = (U32)freq_bin_pos;
    R32 freq_bin_frac = freq_bin_pos - (R32)freq_bin_idx;
	      
    R32 bin_re_0 = spec_buf.re[freq_bin_idx];
    R32 bin_re_1 = spec_buf.re[freq_bin_idx + 1];
    R32 bin_re = lerp(bin_re_0, bin_re_1, freq_bin_frac);
	      
    R32 bin_im_0 = spec_buf.im[freq_bin_idx];
    R32 bin_im_1 = spec_buf.im[freq_bin_idx + 1];
    R32 bin_im = lerp(bin_im_0, bin_im_1, freq_bin_frac);

    R32 bin_mag_sq = (bin_re*bin_re + bin_im*bin_im);
    R32 bin_mag_db = (log10f(bin_mag_sq) - log10f((R32)bin_count)) * 10.f;
    R32 bin_height = ranger32_map_01(bin_mag_db, spec_state->db_rng);
    R32 bin_height_ndc = 2.f * bin_height - 1.f;

    Rect2 bin_rect = rect2_min_dim(v2(pos_x, -1), v2(width, bin_height_ndc));
    render_rect(render_commands, 0, bin_rect, rect2_invalid(), RenderLevel_signal, v4(0, 0.5f, 0.5f, 1));

    exp += step;
    pos_x += width;
  }
}

//
// entry point
//
int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  LooseFont loose_font = {0};  
  
  String8 font_file_path = Str8Lit("/usr/share/fonts/liberation-mono-fonts/LiberationMono-Regular.ttf");
  String8 font_file = os_read_entire_file(scratch.arena, font_file_path);
  if(font_file.count) {

    U32 pt_size = 32;
    loose_font = font_parse(scratch.arena, font_file, pt_size);
  }

  if(wayland_init()) {

    WaylandWindow *window = wayland_open_window(Str8Lit("spectrogram"), 640, 480, RenderTarget_hardware);
    if(window) {

      Arena *state_arena = arena_alloc();
      SpectrogramState *spec_state = spectrogram_state_alloc(state_arena);

      if(audio_init(Str8Lit("spectrogram audio"))) {

	audio_buffer_list.arena = arena_alloc();
	audio_set_user_data(spec_state->sine_osc_state);
	// TOOD: get sample rate from audio backend
      }

      glEnable(GL_TEXTURE_2D);
      
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glDepthFunc(GL_LESS);

      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glPixelStorei(GL_PACK_ALIGNMENT, 1);

      Arena *render_arena = arena_alloc();
      R_Commands *commands = render_alloc_commands(render_arena);
      
      PackedFont *packed_font = font_pack(render_arena, &loose_font);
      R_Font *font = render_alloc_font(render_arena, packed_font);

#if 0
      ArenaTemp test_arena = arena_begin_temp(arena);
      FloatBuffer test_signal = {0};
      test_signal.count = 1024;
      test_signal.mem = arena_push_array_z(test_arena.arena, R32, test_signal.count);

      struct {
	R32 freq, amp;
      } fa[] = {
	{ 32.f, 8.f},
	{105.f, 5.f},
	{330.f, 3.f},
      };      
      for(U32 i = 0; i < test_signal.count; ++i) {

	R32 sample = 0;
	for(U32 j = 0; j < ArrayCount(fa); ++j) {

	  R32 freq = fa[j].freq;
	  R32 amp = fa[j].amp;
	  sample += amp*sin(2.f*PI32*freq*(R32)i/(R32)test_signal.count);
	}
	test_signal.mem[i] = sample;
      }

      ComplexBuffer test_fft_result = fft_re(test_arena.arena, test_signal);
      FloatBuffer test_result = ifft_re(test_arena.arena, test_fft_result);
      Unused(test_result);

      arena_end_temp(test_arena);
#endif

      B32 running = 1;
      Arena *frame_arena = arena_alloc();
      while(running) {
	
	Event event = {0};
	EventList events = wayland_get_events(window);
	while(next_event(&events, &event)) {	  
	  switch(event.kind) {	    
	  case EventKind_Close:
	    { running = 0; } break;
	  default:
	    {} break;
	  }
	}

	GlFramebuffer framebuffer = wayland_get_framebuffer(window);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

	S32 window_width  = window->width;
	S32 window_height = window->height;
	//V2 ndc_scale = v2(2.f/(R32)window_width, 2.f/(R32)window_height);	
	glViewport(0, 0, window_width, window_height);
	glClearColor(0, 0, 0, 1);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_begin_frame(commands, window_width, window_height);
		
	V4 background_color = v4(0.09411f, 0.10196f, 0.14902f, 1);
	Rect2 screen_rect = rect2_center_dim(v2(0, 0), v2(2, 2));
	render_rect(commands, 0, screen_rect, rect2_invalid(), RenderLevel_background, background_color);

	draw_spectrum_grid(spec_state, commands, font);

	// NOTE: drawing the spectrum
	{
	  AudioBufferNode *first_node = 0;
	  AudioBufferNode *last_node = 0;
	  U32 buffer_count = 0;
	  while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 0, 1)) {}
	  {
	    first_node = audio_buffer_list.first;
	    last_node = audio_buffer_list.last;
	    buffer_count = audio_buffer_list.count;
	    audio_buffer_list.first = 0;
	    audio_buffer_list.last = 0;
	    audio_buffer_list.count = 0;
	  }
	  while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 1, 0)) {}

	  fprintf(stderr, "dequeued %u buffers\n", buffer_count);

	  // NOTE: draw	  
	  for(AudioBufferNode *node = first_node; node; node = node->next) {

	    AudioBuffer *buf = &node->buf;

	    // NOTE: window_input
	    // TODO: maybe don't overwrite the buffer?
	    FloatBuffer sample_buf = {.count = buf->sample_count, .mem = buf->samples[0]};
	    for(U32 sample_idx = 0; sample_idx < sample_buf.count; ++sample_idx) {

	      R32 window_val = 0.5f*(cosf(TAU32*((R32)sample_idx/(R32)sample_buf.count - 0.5f)) + 1.f);
	      sample_buf.mem[sample_idx] *= window_val;
	    }

	    ComplexBuffer spectrum_buf = fft_re(frame_arena, sample_buf);
#if 1
	    draw_spectrum(spec_state, spectrum_buf, commands);
#else
#  if 1
	    // TODO: work out ranges/scaling
	    U32 bin_count = spectrum_buf.count/2;
	    R32 width = 2.f/(R32)bin_count;	    
	    R32 pos_x = -1.f;
#    if 1
	    RangeR32 freq_rng = ranger32(2.f, 20000.f);
	    RangeR32 db_rng = ranger32(-60.f, 0.f);
	    R32 step = (log10f(freq_rng.max) - log10f(freq_rng.min))/(R32)bin_count;
	    R32 exp = log10f(freq_rng.min);
	    for(U32 i = 0; i < bin_count; ++i) {

	      R32 freq = powf(10.f, exp);

	      //R32 freq_screen_pos = 2.f * ranger32_map_01(freq, freq_rng) - 1.f;
	      //R32 freq_width = (freq - last_freq)/(freq_rng.max - freq_rng.min);

	      R32 freq_bin_pos = (freq*(R32)spectrum_buf.count)/48000.f; // TODO: use actual sample rate
	      U32 freq_bin_idx = (U32)freq_bin_pos;
	      R32 freq_bin_frac = freq_bin_pos - (R32)freq_bin_idx;
	      
	      R32 bin_re_0 = spectrum_buf.re[freq_bin_idx];
	      R32 bin_re_1 = spectrum_buf.re[freq_bin_idx + 1];
	      R32 bin_re = lerp(bin_re_0, bin_re_1, freq_bin_frac);
	      
	      R32 bin_im_0 = spectrum_buf.im[freq_bin_idx];
	      R32 bin_im_1 = spectrum_buf.im[freq_bin_idx + 1];
	      R32 bin_im = lerp(bin_im_0, bin_im_1, freq_bin_frac);

	      R32 bin_mag_sq = (bin_re*bin_re + bin_im*bin_im);
	      //R32 bin_mag = sqrtf(bin_mag_sq);
	      //R32 bin_mag_norm = bin_mag/(R32)bin_count;
	      R32 bin_mag_db = (log10f(bin_mag_sq) - log10f((R32)bin_count)) * 10.f;
	      R32 bin_height = ranger32_map_01(bin_mag_db, db_rng);
	      R32 bin_height_ndc = 2.f * bin_height - 1.f;

	      Rect2 bin_rect = rect2_min_dim(v2(pos_x, -1), v2(width, bin_height_ndc));
	      render_rect(commands, 0, bin_rect, rect2_invalid(), RenderLevel_signal, v4(0, 0.5f, 0.5f, 1));
		
	      exp += step;
	      pos_x += width;
	    }
#    else
	    for(U32 bin_idx = 0; bin_idx < bin_count; ++bin_idx) {

	      R32 bin_re = spectrum_buf.re[bin_idx];
	      R32 bin_im = spectrum_buf.im[bin_idx];
	      R32 bin_mag_sq = (bin_re*bin_re + bin_im*bin_im);
	      R32 bin_mag_sq_norm = bin_mag_sq/(R32)(spectrum_buf.count);

	      Rect2 bin_rect = rect2_min_dim(v2(pos_x, -1), v2(width, bin_mag_sq_norm));
	      render_rect(commands, 0, bin_rect, rect2_invalid(), 0, v4(0, 0.5f, 0.5f, 1));

	      pos_x += width;
	    }
#    endif
#  else
	    R32 width = 2.f/(R32)buf->sample_count;
	    R32 pos_x = -1.f;
	    R32 last_sample_avg = 0.f;
	    for(U32 sample_idx = 0; sample_idx < buf->sample_count; ++sample_idx) {

	      R32 sample_l = buf->samples[0][sample_idx];
	      R32 sample_r = buf->samples[1][sample_idx];

	      R32 sample_avg = (sample_l + sample_r) * 0.5f;	      

	      R32 base_y = Min(sample_avg, last_sample_avg);
	      R32 height = fabsf(sample_avg - last_sample_avg);
	      Rect2 sample_rect = rect2_min_dim(v2(pos_x, base_y), v2(width, height));

	      render_rect(commands, 0, sample_rect, rect2_invalid(), 0, v4(0, 1, 0, 1));

	      last_sample_avg = sample_avg;
	      pos_x += width;
	    }
#  endif
#endif
	  }

	  if(last_node) {
	    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 0, 1)) {}
	    {
	      last_node->next = audio_buffer_list.freelist;
	      audio_buffer_list.freelist = last_node;	  
	    }
	    while(!AtomicCompareAndSwap(&audio_buffer_list.lock, 1, 0)) {}
	  }
	}

	render_from_commands(commands);

	if(!wayland_end_frame(window)) {
	  Assert(!"FATAL: wayland end frame failed");
	}

	arena_clear(frame_arena);
      }

      audio_uninit();
      wayland_close_window(window);
    }
  }

  arena_release_scratch(scratch);
  return(result);
}
