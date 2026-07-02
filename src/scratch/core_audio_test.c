#include "base/base.h"
#include "audio/audio.h"

#include "base/base.c"
#include "audio/audio.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  if(!os_init())
  { goto failure; }

  if(!audio_init())
  { goto failure; }

  for(Audio_DeviceIterator it = audio_device_iterator();
      !audio_device_iterator_done(it);
      it = audio_device_iterator_next(it))
  {
    String8 device_name = audio_device_name(it.device);
    printf("name: %.*s\n", (int)device_name.count, device_name.string);
  }

  String8 default_input_name = audio_device_name(audio_default_input_device());
  printf("input device: %.*s\n", (int)default_input_name.count, default_input_name.string);

  String8 default_output_name = audio_device_name(audio_default_output_device());
  printf("output device: %.*s\n", (int)default_output_name.count, default_output_name.string);

  audio_start();
  while(1) {}
  audio_stop();

  return 0;

failure:
  return 1;
}
