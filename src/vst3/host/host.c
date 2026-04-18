// -----------------------------------------------------------------------------
// init/uninit layer

proc void
vst3_host_init(void)
{
  Arena *arena = arena_alloc();
  vst3_host_state = arena_push_struct(arena, Vst3_HostState);
  vst3_host_state->arena = arena;
}

proc void
vst3_host_uninit(void)
{
  arena_release(vst3_host_state->arena);
  vst3_host_state = 0;
}

// -----------------------------------------------------------------------------
// load plugin

proc Os_Handle
vst3_host_load_plugin(String8 path_to_plugin_binary)
{
  Os_Handle handle = os_lib_open(path_to_plugin_binary);
  Vst3_GetFactoryProc *get_factory_proc = os_lib_get_sym(handle, Str8Lit("GetPluginFactory"));
  void *init_proc = os_lib_get_sym(handle, vst3_init_proc_name_for_os);
  void *exit_proc = os_lib_get_sym(handle, vst3_exit_proc_name_for_os);

  // NOTE: enumerate plugin classes
  IPluginFactory *plugin_factory = get_factory_proc();
  U64 class_count = IPluginFactory_CountClasses(plugin_factory);

  Arena *arena = vst3_host_state->arena;
  FUnknown **plugin_interfaces = arena_push_array(arena, FUnknown*, class_count);
  for(U64 class_idx = 0; class_idx < class_count; ++class_idx)
  {
    PClassInfo info = {0};
    IPluginFactory_GetClassInfo(plugin_factory,
				class_idx, &info);

    IPluginFactory_CreateInstance(plugin_factory,
				  info.cid.s, vst3_funknown_iid, (void**)&plugin_interfaces[class_idx]);
  }
  IPluginFactory_Release(plugin_factory);

  vst3_host_state->plugin_handle = handle;
  vst3_host_state->get_factory_proc = get_factory_proc;
  vst3_host_state->init_proc = init_proc;
  vst3_host_state->exit_proc = exit_proc;
  vst3_host_state->plugin_interface_count = class_count;
  vst3_host_state->plugin_interfaces = plugin_interfaces;

  return(handle);
}

proc void
vst3_host_unload_plugin(Os_Handle plugin_handle)
{
  // NOTE: release interfaces
  U64 interface_count = vst3_host_state->plugin_interface_count;
  FUnknown **interfaces = vst3_host_state->plugin_interfaces;
  for(U64 interface_idx = 0; interface_idx < interface_count; ++interface_idx)
  {
    FUnknown *interface = interfaces[interface_idx];
    interface->v->release(interface);
    interfaces[interface_idx] = 0;
  }

  // NOTE: close plugin
  os_lib_close(plugin_handle);
  vst3_host_state->get_factory_proc = 0;
  vst3_host_state->init_proc = 0;
  vst3_host_state->exit_proc = 0;
}

/// -----------------------------------------------------------------------------
// init/uninit plugin

proc void vst3_host_init_plugin(void);
proc void vst3_host_uninit_plugin(void);
