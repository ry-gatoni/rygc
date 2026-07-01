#include "audio_common.c"

#if AUDIO_BACKEND == AUDIO_BACKEND_JACK
#  include "audio/JACK/jack.c"
#elif AUDIO_BACKEND == AUDIO_BACKEND_WASAPI
#  include "audio/WASAPI/wasapi.c"
#elif AUDIO_BACKEND == AUDIO_BACKEND_CORE_AUDIO
#  include "audio/audio.core_audio.c"
#else
#  error ERROR: unsuppored audio backend
#endif
