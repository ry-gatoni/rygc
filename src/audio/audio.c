#include "audio/audio_common.c"

// TODO: implement audio backends for mac and windows (and maybe other linux ones)
#if OS_LINUX
#  include "audio/JACK/jack.c"
#elif OS_WINDOWS
#  include "audio/WASAPI/wasapi.c"
#else
#  error ERROR: no audio backend for this platform yet :(
#endif
