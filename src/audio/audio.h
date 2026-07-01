#include "audio_common.h"

#define AUDIO_BACKEND_NULL 0
#define AUDIO_BACKEND_JACK 1
#define AUDIO_BACKEND_WASAPI 2
#define AUDIO_BACKEND_CORE_AUDIO 3

#ifndef AUDIO_BACKEND
#  if OS_LINUX
#    define AUDIO_BACKEND AUDIO_BACKEND_JACK
#  elif OS_WINDOWS
#    define AUDIO_BACKEND AUDIO_BACKEND_WASAPI
#  elif OS_MAC
#    define AUDIO_BACKEND AUDIO_BACKEND_CORE_AUDIO
#  else
#    define AUDIO_BACKEND AUDIO_BACKEND_NULL
#  endif
#endif

#if AUDIO_BACKEND == AUDIO_BACKEND_JACK
#  include "audio/JACK/jack.h"
#elif AUDIO_BACKEND == AUDIO_BACKEND_WASAPI
#  include "audio/WASAPI/wasapi.h"
#elif AUDIO_BACKEND == AUDIO_BACKEND_CORE_AUDIO
#  include "audio/audio.core_audio.h"
#else
#  error ERROR: unrecognized value for `AUDIO_BACKEND`
#endif
