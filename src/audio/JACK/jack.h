#include <jack/jack.h>
#include <jack/midiport.h>

typedef struct Jack_State
{
  Arena *arena;

  String8List sink_ports;
  String8List source_ports;

  jack_client_t *client;
  String8 name;

  U32 sample_rate;

  jack_port_t *midi_port;
  jack_port_t *input_port;
  jack_port_t *output_port[2];

  Log *log;
  Profiler *profiler;
} Jack_State;

global Jack_State *jack_state = 0;
global Audio_ProcessData global_process_data = {0};

proc B32 jack_init(String8 client_name);
proc int jack_process(jack_nframes_t nframes, void *arg);
