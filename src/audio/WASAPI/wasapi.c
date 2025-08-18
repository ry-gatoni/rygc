// TODO: implement
proc B32
audio_init(String8 client_name)
{
  Unused(client_name);
  return(0);
}

proc void
audio_uninit(void)
{
}

proc void
audio_set_user_data(void *data)
{
  Unused(data);  
}

proc U32 audio_get_sample_rate(void)
{
  return(0);
}
