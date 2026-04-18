#include "base/base.h"
#include "vst3/host/host.h"

#include "base/base.c"
#include "vst3/host/host.c"

int
main(int argc, char **argv)
{
  Unused(argc);
  Unused(argv);

  vst3_host_init();

  String8 plugin_path = Str8Lit("/home/rycurrier/.vst3/Granade.vst3/Contents/x86_64-linux/Granade.so");
  Os_Handle plugin_handle = vst3_host_load_plugin(plugin_path);
  vst3_host_unload_plugin(plugin_handle);

  vst3_host_uninit();

  return(0);
}
