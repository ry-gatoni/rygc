#include "base/base.h"
#include "os/os.h"

#include "base/base.c"
#include "os/os.c"

#include <math.h>

#include <jack/jack.h>

typedef struct JackProcessData
{
  R32 phasor;
  R32 freq;
  R32 sample_period;

  jack_port_t *input_port, *output_port;
} JackProcessData;

proc int
jack_process(jack_nframes_t nframes, void *arg)
{
  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);  

  JackProcessData *data = arg;
  R32 step = TAU32 * data->freq * data->sample_period;

  R32 *in  = jack_port_get_buffer(data->input_port, nframes);
  R32 *out = jack_port_get_buffer(data->output_port, nframes);
  Unused(in);

  R32 *sine_samples = arena_push_array_z(scratch.arena, R32, nframes);

  for(U32 frame_index = 0; frame_index < nframes; ++frame_index) {
    data->phasor += step;
    if(data->phasor > TAU32) {
      data->phasor -= TAU32;
    }

    R32 sample = 0.1f*sinf(data->phasor);
    sine_samples[frame_index] = sample;
  }

  memcpy(out, sine_samples, nframes*sizeof(R32));

  arena_release_scratch(scratch);
  return(result);
}

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);
  int result = 0;

  const char *jack_client_name = "jack_test";
  jack_status_t jack_status;
  jack_client_t *jack_client = jack_client_open(jack_client_name, JackNullOption, &jack_status);
  if(jack_client) {
    if(jack_status & JackNameNotUnique) {
      jack_client_name = jack_get_client_name(jack_client);
    }

    JackProcessData user_data = {0};
    user_data.freq = 440.f;
    user_data.sample_period = 1.f/(R32)48000.f;
    jack_set_process_callback(jack_client, jack_process, &user_data);

    jack_port_t *jack_input_port  = jack_port_register(jack_client, "input",
						      JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    jack_port_t *jack_output_port = jack_port_register(jack_client, "output",
						       JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    if(jack_input_port && jack_output_port) {
      if(jack_activate(jack_client) == 0) {
	const char **jack_ports = jack_get_ports(jack_client, 0, 0, JackPortIsOutput | JackPortIsPhysical);
	if(jack_ports) {
	  if(jack_connect(jack_client, jack_ports[0], jack_port_name(jack_input_port)) == 0) {
	    free(jack_ports);

	    jack_ports = jack_get_ports(jack_client, 0, 0, JackPortIsInput | JackPortIsPhysical);
	    if(jack_ports) {
	      if(jack_connect(jack_client, jack_port_name(jack_output_port), jack_ports[0]) == 0) {
		free(jack_ports);

		user_data.input_port  = jack_input_port;
		user_data.output_port = jack_output_port;
		
		while(1) {}

		jack_client_close(jack_client);
	      }else {
		fprintf(stderr, "ERROR: failed to connect to input port\n");
		result = 1;
	      }
	    }else {
	      fprintf(stderr, "ERROR: failed to get input ports\n");
	      result = 1;
	    }
	  }else {
	    fprintf(stderr, "ERROR: failed to connect to output port\n");
	    result = 1;
	  }
	}else {
	  fprintf(stderr, "ERROR: failed to get output ports\n");
	  result = 1;
	}
      }else {
	fprintf(stderr, "ERROR: failed to activate\n");
	result = 1;
      }
    }else {
      fprintf(stderr, "ERROR: failed to register ports\n");
      result = 1;
    }
  }else {
    fprintf(stderr, "ERROR: failed to open client\n");
    result = 1;
  }

  return(result);
}
