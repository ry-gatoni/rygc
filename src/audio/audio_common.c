proc Audio_MidiMessage*
audio_next_midi_message(Audio_ProcessData *data)
{
  Audio_MidiMessage *msg = data->first_midi_msg;
  return(msg);
}

proc void
audio_dequeue_midi_message(Audio_ProcessData *data)
{
  SLLQueuePop(data->first_midi_msg, data->last_midi_msg);
}

proc U32
audio_midi_get_note_number(Audio_MidiMessage *msg)
{
  Assert(msg->opcode == MidiOpcode_note_on || msg->opcode == MidiOpcode_note_off);
  U32 result = msg->body.mem[0];
  return(result);
}

proc R32
audio_midi_get_pitch_bend(Audio_MidiMessage *msg)
{
  Assert(msg->opcode == MidiOpcode_pitch_bend);
  U32 pitch_bend_max_neg = (1 << 13);
  S16 pitch_bend_int = ((msg->body.mem[1] & 0x7F) << 7 |
			(msg->body.mem[0] & 0x7F)) - pitch_bend_max_neg;
  R32 result = powf(2.f, (R32)(pitch_bend_int)/(R32)(pitch_bend_max_neg));
  return(result);
}

proc R32
audio_hertz_from_midi_note(U32 note)
{
  R32 result = 440.f*powf(2.f, (R32)((S32)note - 69)/12.f);  
  return(result);
}
