// NOTE: jack functions
proc B32
jack_init(String8 client_name)
{
  B32 result = 1;

  jack_state.client = jack_client_open((const char*)client_name.string, JackNullOption, &jack_state.status);
  if(jack_state.client) {

    jack_state.arena = arena_alloc();
    
    if(jack_state.status & JackNameNotUnique) {
      client_name = Str8Cstr(jack_get_client_name(jack_state.client));
    }

    jack_state.name = arena_push_str8_copy(jack_state.arena, client_name);
    jack_state.sample_rate = jack_get_sample_rate(jack_state.client);
    jack_set_process_callback(jack_state.client, jack_process, &global_process_data);

    // NOTE: connect ports
    jack_state.midi_port      = jack_port_register(jack_state.client, "midi",
						   JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
    jack_state.input_port     = jack_port_register(jack_state.client, "input",
						   JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    jack_state.output_port[0] = jack_port_register(jack_state.client, "outputL",
						   JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    jack_state.output_port[1] = jack_port_register(jack_state.client, "outputR",
						   JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    if(jack_state.midi_port && jack_state.input_port && jack_state.output_port[0] && jack_state.output_port[1]) {
      if(jack_activate(jack_state.client) == 0) {
	const char **input_ports =
	  jack_get_ports(jack_state.client, 0, 0, JackPortIsInput | JackPortIsPhysical);

	// TODO: better device enumeration
	if(input_ports) {
	  if(jack_connect(jack_state.client, jack_port_name(jack_state.output_port[0]), input_ports[0]) == 0 &&
	     jack_connect(jack_state.client, jack_port_name(jack_state.output_port[1]), input_ports[1]) == 0) {
	    //fprintf(stderr, "output connected\n");
	  }

	  for(char **input_port = (char**)input_ports; *input_port; ++input_port) {
	    char *port_name = *input_port;
	    //fprintf(stderr, "input port: %s\n", port_name);
	    str8_list_push(jack_state.arena, &jack_state.input_ports, Str8Cstr(port_name));	  
	  }

	  free(input_ports);
	}

	const char **output_ports =
	  jack_get_ports(jack_state.client, 0, 0, JackPortIsOutput | JackPortIsPhysical);
	if(output_ports) {
	  if(jack_connect(jack_state.client, output_ports[0], jack_port_name(jack_state.input_port)) == 0) {
	    //fprintf(stderr, "input connected\n");
	  }
	
	  for(char **output_port = (char**)output_ports; *output_port; ++output_port) {
	    char *port_name = *output_port;
	    //fprintf(stderr, "output port: %s\n", port_name);
	    str8_list_push(jack_state.arena, &jack_state.output_ports, Str8Cstr(port_name));

	    // TODO: get name of midi device to connect to somehow
	    if(str8_contains(Str8Cstr(port_name), Str8Lit("Arturia MiniLab mkII"))) {
	      if(jack_connect(jack_state.client, port_name, jack_port_name(jack_state.midi_port)) == 0) {
		//fprintf(stderr, "midi connected\n");
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
  }else {
    result = 0;
  }

  return(result);
}

proc int
jack_process(jack_nframes_t nframes, void *arg)
{
  int result = 0;
  ArenaTemp scratch = arena_get_scratch(0, 0);

  U8  *midi  = jack_port_get_buffer(jack_state.midi_port,      nframes);
  R32 *in    = jack_port_get_buffer(jack_state.input_port,     nframes);
  R32 *outL  = jack_port_get_buffer(jack_state.output_port[0], nframes);
  R32 *outR  = jack_port_get_buffer(jack_state.output_port[1], nframes);

  Audio_MidiMessage *first_midi_msg = 0;
  Audio_MidiMessage *last_midi_msg = 0;
  U64 midi_msg_count = 0;

  jack_midi_event_t midi_event;
  jack_nframes_t midi_event_count = jack_midi_get_event_count(midi);
  for(U32 event_idx = 0; event_idx < midi_event_count; ++event_idx) {

    jack_midi_event_get(&midi_event, midi, event_idx);

    Audio_MidiMessage *new_midi_msg = arena_push_struct(scratch.arena, Audio_MidiMessage);
    new_midi_msg->sample_idx = midi_event.time;
    new_midi_msg->opcode = midi_event.buffer[0] & 0xF0;
    new_midi_msg->channel = midi_event.buffer[0] & 0x0F;
    new_midi_msg->body.size = midi_event.size - 1;
    new_midi_msg->body.mem = arena_push_array(scratch.arena, U8, new_midi_msg->body.size);
    CopyArray(new_midi_msg->body.mem, midi_event.buffer + 1, U8, new_midi_msg->body.size);

    SLLQueuePush(first_midi_msg, last_midi_msg, new_midi_msg);
    ++midi_msg_count;
    // TODO: keep track of total size of all messages?
  }

  R32 sample_period = 1.f/(R32)jack_state.sample_rate;
  U32 sample_count = nframes;
  R32 *input_l = in;
  R32 *input_r = in;
  R32 *output_l = outL;
  R32 *output_r = outR;
  
  // TODO: set up process data
  Audio_ProcessData *process_data = arg;
  process_data->input[0] = input_l;
  process_data->input[1] = input_r;
  process_data->output[0] = output_l;
  process_data->output[1] = output_r;
  process_data->first_midi_msg = first_midi_msg;
  process_data->last_midi_msg = last_midi_msg;
  process_data->midi_msg_count = midi_msg_count;
  process_data->sample_period = sample_period;
  process_data->sample_count = sample_count;
  
  audio_process(process_data);

  arena_release_scratch(scratch);
  return(result);
}

// NOTE: implement common functions to call jack functions
proc B32
audio_init(String8 client_name)
{
  B32 result = jack_init(client_name);
  return(result);
}

proc void
audio_uninit(void)
{
  if(jack_state.client) {
    jack_deactivate(jack_state.client);
  }
}

proc void
audio_set_user_data(void *data)
{
  global_process_data.user_data = data;
}

proc U32
audio_get_sample_rate(void)
{
  U32 result = jack_get_sample_rate(jack_state.client);
  return(result);
}
