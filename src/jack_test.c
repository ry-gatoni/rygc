#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

#include <math.h>

#include <jack/jack.h>
#include <jack/midiport.h>

/**
 * TODO:
 * handle midi messages 
 * better device enumeration
 * handle sample rate changes
 * handle latency changes
 * handle port reconnections
 * smooth out continuous parameter changes
 */

typedef struct JackClientState
{
  Arena *arena;

  String8List input_ports;
  String8List output_ports;

  jack_client_t *client;
  jack_status_t status;
  String8 name;

  jack_port_t *midi_port;
  jack_port_t *input_port;
  jack_port_t *output_port[2];
} JackClientState;

global JackClientState jack_client_state;

proc B32
jack_client_init(String8 name)
{
  B32 result = 1;

  jack_client_state.client = jack_client_open((const char*)name.string, JackNullOption, &jack_client_state.status);
  if(jack_client_state.client) {

    jack_client_state.arena = arena_alloc();
    
    if(jack_client_state.status & JackNameNotUnique) {
      name = Str8Cstr(jack_get_client_name(jack_client_state.client));
    }

    jack_client_state.name = arena_push_str8_copy(jack_client_state.arena, name);
  }else {
    result = 0;
  }
  
  return(result);
}

proc B32
jack_client_connect_ports(void)
{
  B32 result = 1;
  jack_client_state.midi_port      = jack_port_register(jack_client_state.client, "midi",
							JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
  jack_client_state.input_port     = jack_port_register(jack_client_state.client, "input",
							JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  jack_client_state.output_port[0] = jack_port_register(jack_client_state.client, "outputL",
							JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  jack_client_state.output_port[1] = jack_port_register(jack_client_state.client, "outputR",
							JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  if(jack_client_state.midi_port && jack_client_state.input_port && jack_client_state.output_port[0] && jack_client_state.output_port[1]) {
    if(jack_activate(jack_client_state.client) == 0) {
      const char **input_ports =
	jack_get_ports(jack_client_state.client, 0, 0, JackPortIsInput | JackPortIsPhysical);

      // TODO: better device enumeration
      if(input_ports) {
	if(jack_connect(jack_client_state.client, jack_port_name(jack_client_state.output_port[0]), input_ports[0]) == 0 &&
	   jack_connect(jack_client_state.client, jack_port_name(jack_client_state.output_port[1]), input_ports[1]) == 0) {
	  fprintf(stderr, "output connected\n");
	}

	for(char **input_port = (char**)input_ports; *input_port; ++input_port) {
	  char *port_name = *input_port;
	  fprintf(stderr, "input port: %s\n", port_name);
	  str8_list_push(jack_client_state.arena, &jack_client_state.input_ports, Str8Cstr(port_name));	  
	}

	free(input_ports);
      }

      const char **output_ports =
	jack_get_ports(jack_client_state.client, 0, 0, JackPortIsOutput | JackPortIsPhysical);
      if(output_ports) {
	if(jack_connect(jack_client_state.client, output_ports[0], jack_port_name(jack_client_state.input_port)) == 0) {
	  fprintf(stderr, "input connected\n");
	}
	
	for(char **output_port = (char**)output_ports; *output_port; ++output_port) {
	  char *port_name = *output_port;
	  fprintf(stderr, "output port: %s\n", port_name);
	  str8_list_push(jack_client_state.arena, &jack_client_state.output_ports, Str8Cstr(port_name));
	  
	  if(str8_contains(Str8Cstr(port_name), Str8Lit("Arturia MiniLab mkII"))) {
	    if(jack_connect(jack_client_state.client, port_name, jack_port_name(jack_client_state.midi_port)) == 0) {
	      fprintf(stderr, "midi connected\n");
	    }
	  }
	}

	free(output_ports);
      }
    }else {
      result = 0;
    }
  }else {
    result = 0;
  }

  return(result);
}

typedef struct JackMidiMessage JackMidiMessage;
struct JackMidiMessage
{
  JackMidiMessage *next;
  U32 timestamp;
  String8 body;
};

typedef struct JackMidiMessageList
{
  JackMidiMessage *first;
  JackMidiMessage *last;
  U64 message_count;
  U64 total_size;
} JackMidiMessageList;

proc void
jack_midi_list_push(Arena *arena, JackMidiMessageList *list, jack_midi_event_t midi_event)
{
  String8 message_body = {.count = midi_event.size, .string = midi_event.buffer};
  JackMidiMessage *message = arena_push_struct(arena, JackMidiMessage);
  message->timestamp = midi_event.time;
  message->body = arena_push_str8_copy(arena, message_body);
  
  SLLQueuePush(list->first, list->last, message);
  ++list->message_count;
  list->total_size += message->body.count;
}

proc R32
hertz_from_midi_note(S8 midi_note)
{
  R32 result = 440.f*powf(2.f, (R32)(midi_note - 69)/12.f);  
  return(result);
}

typedef struct JackProcessData
{
  R32 phasor;
  R32 base_freq;
  R32 pitch_bend;
  R32 sample_period;
} JackProcessData;

proc int
jack_process(jack_nframes_t nframes, void *arg)
{
  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);  

  JackProcessData *data = arg;

  U8  *midi  = jack_port_get_buffer(jack_client_state.midi_port,      nframes);
  R32 *in    = jack_port_get_buffer(jack_client_state.input_port,     nframes);
  R32 *outL  = jack_port_get_buffer(jack_client_state.output_port[0], nframes);
  R32 *outR  = jack_port_get_buffer(jack_client_state.output_port[1], nframes);

  // NOTE: handle midi events
  jack_midi_event_t midi_event;
  jack_nframes_t midi_event_count = jack_midi_get_event_count(midi);
  JackMidiMessageList midi_message_list = {0};

  for(U32 event_index = 0; event_index < midi_event_count; ++event_index) {
    jack_midi_event_get(&midi_event, midi, event_index);
    jack_midi_list_push(scratch.arena, &midi_message_list, midi_event);
  }
  
  R32 base_freq = data->base_freq;
  R32 pitch_bend = data->pitch_bend;
  
  R32 *freq_buffer = arena_push_array(scratch.arena, R32, nframes);
  for(U32 frame_index = 0; frame_index < nframes; ++frame_index) {
    JackMidiMessage *midi_msg = midi_message_list.first;
    if(midi_msg) {
      if(midi_msg->timestamp >= frame_index) {
	U8 opcode = midi_msg->body.string[0];
	fprintf(stderr, "midi message: timestamp=%u, size=%lu, opcode=0x%X\n",
	    midi_msg->timestamp, midi_msg->body.count, opcode);

	switch(opcode & 0xF0) {
	case 0x90: {
	  base_freq = hertz_from_midi_note(midi_msg->body.string[1]);
	} break;
	  
	case 0xE0: {
	  U32 pitch_bend_max_neg = (1 << 13);
	  S16 pitch_bend_int = ((midi_msg->body.string[2] & 0x7F) << 7 |
				(midi_msg->body.string[1] & 0x7F)) - pitch_bend_max_neg;
	  pitch_bend = powf(2.f, (R32)(pitch_bend_int)/(R32)(pitch_bend_max_neg));
	} break;
	  
	default: {} break;
	}	

	SLLQueuePop(midi_message_list.first, midi_message_list.last);
      }
    }

    // TODO: smooth out pitch bend changes
    freq_buffer[frame_index] = base_freq * pitch_bend;
  }
  data->base_freq = base_freq;
  data->pitch_bend = pitch_bend;

  // NOTE: output samples
  R32 *mix_buffer = arena_push_array_z(scratch.arena, R32, nframes);

  for(U32 frame_index = 0; frame_index < nframes; ++frame_index) {
    R32 step = TAU32 * freq_buffer[frame_index] * data->sample_period;
    data->phasor += step;
    if(data->phasor > TAU32) {
      data->phasor -= TAU32;
    }

    R32 sine_sample = 0.1f*sinf(data->phasor);
    mix_buffer[frame_index] += sine_sample;
    mix_buffer[frame_index] += 0.5f*in[frame_index];
  }

  CopyArray(outL, mix_buffer, R32, nframes);
  CopyArray(outR, mix_buffer, R32, nframes);

  arena_release_scratch(scratch);
  return(result);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);
  
  if(jack_client_init(Str8Lit("jack_test"))) {
    jack_nframes_t sample_rate = jack_get_sample_rate(jack_client_state.client);
    
    JackProcessData user_data = {0};
    user_data.base_freq = 440.f;
    user_data.pitch_bend = 1.f;
    user_data.sample_period = 1.f/(R32)sample_rate;
    jack_set_process_callback(jack_client_state.client, jack_process, &user_data);

    if(jack_client_connect_ports()) {

      jack_latency_range_t latency_range;
      jack_port_get_latency_range(jack_client_state.output_port[0], JackPlaybackLatency, &latency_range);
      fprintf(stderr, "outputL latency range: (%u, %u)\n", latency_range.min, latency_range.max);
      
      jack_port_get_latency_range(jack_client_state.output_port[1], JackPlaybackLatency, &latency_range);
      fprintf(stderr, "outputR latency range: (%u, %u)\n", latency_range.min, latency_range.max);

      jack_port_get_latency_range(jack_client_state.input_port, JackCaptureLatency, &latency_range);
      fprintf(stderr, "input latency range:   (%u, %u)\n", latency_range.min, latency_range.max);

      while(1) {}
    }
    
    jack_client_close(jack_client_state.client);
  }
  
  return(0);
}
