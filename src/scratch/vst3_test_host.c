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
  Vst3_PluginImage *plugin_image = vst3_host_plugin_load(plugin_path);

  Vst3_PluginInstance *plugin = vst3_host_plugin_instantiate(plugin_image);
  vst3_host_plugin_uninstantiate(plugin);

  vst3_host_plugin_unload(plugin_image);

  vst3_host_uninit();

  return(0);
}
