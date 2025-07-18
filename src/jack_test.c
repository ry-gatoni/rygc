#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

#include <math.h>

#include <jack/jack.h>
#include <jack/midiport.h>

/**
 * TODO:
 * stereo output
 * handle midi messages 
 * process input samples
 * better device enumeration
 * get samplerate
 * get latency
 * handle port reconnections
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
  jack_port_t *output_port;
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
  jack_client_state.midi_port   = jack_port_register(jack_client_state.client, "midi",
						     JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
  jack_client_state.input_port  = jack_port_register(jack_client_state.client, "input",
						     JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  jack_client_state.output_port = jack_port_register(jack_client_state.client, "output",
						     JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

  if(jack_client_state.midi_port && jack_client_state.input_port && jack_client_state.output_port) {
    if(jack_activate(jack_client_state.client) == 0) {
      const char **input_ports =
	jack_get_ports(jack_client_state.client, 0, 0, JackPortIsInput | JackPortIsPhysical);

      // TODO: better device enumeration
      if(input_ports) {
	if(jack_connect(jack_client_state.client, jack_port_name(jack_client_state.output_port), input_ports[0]) == 0) {
	  fprintf(stderr, "input connected\n");
	}

	for(char **input_port = (char**)input_ports; *input_port; ++input_port) {
	  char *port_name = *input_port;
	  fprintf(stderr, "input port: %s\n", port_name);
	  str8_list_push(jack_client_state.arena, &jack_client_state.input_ports, Str8Cstr(port_name));

	  jack_port_t *port = jack_port_by_name(jack_client_state.client, port_name);
	  const char *port_type = jack_port_type(port);
	  fprintf(stderr, "  type: %s\n", port_type);
	  // TODO: connect midi port
	}

	free(input_ports);
      }

      const char **output_ports =
	jack_get_ports(jack_client_state.client, 0, 0, JackPortIsOutput | JackPortIsPhysical);
      if(output_ports) {
	if(jack_connect(jack_client_state.client, output_ports[0], jack_port_name(jack_client_state.input_port)) == 0) {
	  fprintf(stderr, "output connected\n");
	}
	
	for(char **output_port = (char**)output_ports; *output_port; ++output_port) {
	  char *port_name = *output_port;
	  fprintf(stderr, "output port: %s\n", port_name);
	  str8_list_push(jack_client_state.arena, &jack_client_state.output_ports, Str8Cstr(port_name));
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

typedef struct JackMidiHeader
{
  U32 timestamp;
  U32 size;
} JackMidiHeader;

typedef struct JackProcessData
{
  R32 phasor;
  R32 freq;
  R32 sample_period;
} JackProcessData;

proc int
jack_process(jack_nframes_t nframes, void *arg)
{
  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);  

  JackProcessData *data = arg;
  R32 step = TAU32 * data->freq * data->sample_period;

  U8  *midi = jack_port_get_buffer(jack_client_state.midi_port,   nframes);
  R32 *in   = jack_port_get_buffer(jack_client_state.input_port,  nframes);
  R32 *out  = jack_port_get_buffer(jack_client_state.output_port, nframes);
  Unused(in);

  // NOTE: handle midi events
  jack_midi_event_t midi_event;
  jack_nframes_t midi_event_count = jack_midi_get_event_count(midi);
  PushBuffer midi_buffer = push_buffer_alloc(scratch.arena, midi_event_count*256);

  for(U32 event_index = 0; event_index < midi_event_count; ++event_index) {
    jack_midi_event_get(&midi_event, midi, event_index);

    JackMidiHeader *event_header = buf_push_struct(&midi_buffer, JackMidiHeader);
    event_header->timestamp = midi_event.time;
    event_header->size = midi_event.size;

    U8 *event_data = buf_push_array(&midi_buffer, U8, event_header->size);
    CopyArray(event_data, midi_event.buffer, U8, event_header->size);
  }
  // TODO: parse events

  // NOTE: output samples
  R32 *sine_samples = arena_push_array_z(scratch.arena, R32, nframes);

  for(U32 frame_index = 0; frame_index < nframes; ++frame_index) {
    data->phasor += step;
    if(data->phasor > TAU32) {
      data->phasor -= TAU32;
    }

    R32 sample = 0.2f*sinf(data->phasor);
    sine_samples[frame_index] = sample;
  }

  CopyArray(out, sine_samples, R32, nframes);

  arena_release_scratch(scratch);
  return(result);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);
  
  if(jack_client_init(Str8Lit("jack_test"))) {
    JackProcessData user_data = {0};
    user_data.freq = 440.f;
    user_data.sample_period = 1.f/(R32)48000.f;
    jack_set_process_callback(jack_client_state.client, jack_process, &user_data);

    if(jack_client_connect_ports()) {
      while(1) {}
    }
    
    jack_client_close(jack_client_state.client);
  }
  
  return(0);
}
