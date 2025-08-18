#include "audio/audio_common.h"

// TODO: implement audio backends for mac and windows (and maybe other linux ones)
#if OS_LINUX
#  include "audio/JACK/jack.h"
#elif OS_WINDOWS
#  include "audio/WASAPI/wasapi.h"
#else
#  error ERROR: no audio backend for this platform yet :(
#endif
