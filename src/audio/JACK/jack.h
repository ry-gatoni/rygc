#include <jack/jack.h>
#include <jack/midiport.h>

typedef struct JackState
{
  Arena *arena;

  String8List input_ports;
  String8List output_ports; 

  jack_client_t *client;
  jack_status_t status;
  String8 name;

  U32 sample_rate;

  jack_port_t *midi_port;
  jack_port_t *input_port;
  jack_port_t *output_port[2];
} JackState;

global JackState jack_state = {0};
global Audio_ProcessData global_process_data = {0};

proc B32 jack_init(String8 client_name);
proc int jack_process(jack_nframes_t nframes, void *arg);
