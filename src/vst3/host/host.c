// -----------------------------------------------------------------------------
// init/uninit layer

proc void
vst3_host_init(void)
{
  Arena *arena = arena_alloc();
  vst3_host_state = arena_push_struct(arena, Vst3_HostState);
  vst3_host_state->arena = arena;

  // NOTE: host name strings
  vst3_host_state->name_str8 = Str8Lit("rygcigarette123");
  vst3_host_state->name_str16 = str16_from_str8(arena, vst3_host_state->name_str8);

  // NOTE: host application vtable
  vst3_host_state->ihost_application.v = (void*)arena_push_array(arena, void*, ArrayCount(vst3_host_state->ihost_application.v->table));
  vst3_host_state->ihost_application.v->query_interface = IHostApplication_QueryInterface;
  vst3_host_state->ihost_application.v->add_ref = IHostApplication_AddRef;
  vst3_host_state->ihost_application.v->release = IHostApplication_Release;
  vst3_host_state->ihost_application.v->get_name = IHostApplication_GetName;
  vst3_host_state->ihost_application.v->create_instance = IHostApplication_CreateInstance;
}

proc void
vst3_host_uninit(void)
{
  arena_release(vst3_host_state->arena);
  vst3_host_state = 0;
}

// -----------------------------------------------------------------------------
// load plugin

proc Vst3_PluginImage*
vst3_host_plugin_load(String8 path_to_plugin_binary)
{
  Arena *arena = vst3_host_state->arena;
  Vst3_PluginImage *result = arena_push_struct(arena, Vst3_PluginImage);

  Os_Handle handle = os_lib_open(path_to_plugin_binary);
  Vst3_GetFactoryProc *get_factory_proc = os_lib_get_sym(handle, Str8Lit("GetPluginFactory"));
  Vst3_InitModuleProc *init_proc = os_lib_get_sym(handle, vst3_init_proc_name_for_os);
  Vst3_ExitModuleProc *exit_proc = os_lib_get_sym(handle, vst3_exit_proc_name_for_os);

  // NOTE: init the module if it has an init function
  if(init_proc)
  {
#if OS_MAC || OS_LINUX
    init_proc(handle.handle);
#elif OS_WINDOWS
    init_proc();
#endif
  }
  else
  {
    // TODO: error on mac and linux
  }

  // NOTE: enumerate plugin classes
  IPluginFactory *plugin_factory = get_factory_proc();
  U64 class_count = IPluginFactory_CountClasses(plugin_factory);

  PClassInfo *class_infos = arena_push_array(arena, PClassInfo, class_count);
  for(U64 class_idx = 0; class_idx < class_count; ++class_idx)
  {
    PClassInfo *info = class_infos + class_idx;
    IPluginFactory_GetClassInfo(plugin_factory, class_idx, info);
  }

  result->handle = handle;
  result->get_factory_proc = get_factory_proc;
  result->init_proc = init_proc;
  result->exit_proc = exit_proc;
  result->factory = plugin_factory;
  result->class_count = class_count;
  result->class_infos = class_infos;
  vst3_host_state->image = result;
  return(result);
#if 0
  FUnknown **plugin_interfaces_unknown = arena_push_array(arena, FUnknown*, class_count);
  void **plugin_interfaces = vst3_host_state->plugin_interfaces.unknown;
  for(U64 class_idx = 0; class_idx < class_count; ++class_idx)
  {
    PClassInfo info = {0};
    IPluginFactory_GetClassInfo(plugin_factory,
				class_idx, &info);

    Vst3_InterfaceSet interfaces = vst3_interfaces_for_category(info.category);
    {
      Vst3_Interface const *i = interfaces.is;
      Vst3_Interface const *opl = i + interfaces.count;
      for(;i < opl; ++i)
      {
	Vst3_Interface plugin_interface = *i;
	FidString iid = vst3_fid_for_interface(plugin_interface);
	IPluginFactory_CreateInstance(plugin_factory,
				      info.cid.s, iid, (void**)&plugin_interfaces[plugin_interface]);
      }
    }

    FidString funknown_iid = vst3_fid_for_interface(Vst3_Interface_FUnknown);
    IPluginFactory_CreateInstance(plugin_factory,
				  info.cid.s, funknown_iid, (void**)&plugin_interfaces_unknown[class_idx]);
  }
  IPluginFactory_Release(plugin_factory);

  vst3_host_state->plugin_handle = handle;
  vst3_host_state->get_factory_proc = get_factory_proc;
  vst3_host_state->init_proc = init_proc;
  vst3_host_state->exit_proc = exit_proc;
  vst3_host_state->plugin_interface_count = class_count;
  vst3_host_state->plugin_interfaces_unknown = plugin_interfaces_unknown;

  return(handle);
  #endif
}

proc void
vst3_host_plugin_unload(Vst3_PluginImage *plugin)
{
  IPluginFactory_Release(plugin->factory);
  plugin->factory = 0;

#if 0
  // NOTE: release interfaces
  {
    U64 interface_count = vst3_host_state->plugin_interface_count;
    FUnknown **interfaces = vst3_host_state->plugin_interfaces_unknown;
    for(U64 interface_idx = 0; interface_idx < interface_count; ++interface_idx)
    {
      FUnknown *interface = interfaces[interface_idx];
      interface->v->release(interface);
      interfaces[interface_idx] = 0;
    }
  }
  {
    void **interfaces = vst3_host_state->plugin_interfaces.unknown;
    for(U64 interface_idx = 0; interface_idx < Vst3_Interface_COUNT; ++interface_idx)
    {
      FUnknown *interface = (FUnknown*)interfaces[interface_idx];
      if(interface) interface->v->release(interface);
      interfaces[interface_idx] = 0;
    }
  }
#endif

  // NOTE: close plugin
  os_lib_close(plugin->handle);
  plugin->get_factory_proc = 0;
  plugin->init_proc = 0;
  plugin->exit_proc = 0;

  vst3_host_state->image = 0;
}

/// -----------------------------------------------------------------------------
// init/uninit plugin

proc void
vst3_host_plugin_init(Vst3_PluginImage *plugin);

proc void
vst3_host_plugin_uninit(Vst3_PluginImage *plugin);

// -----------------------------------------------------------------------------
// instantiate/uninstantiate plugin

proc Vst3_PluginInstance*
vst3_host_plugin_instantiate(Vst3_PluginImage *plugin)
{
  TResult ok = KResult_ok;

  Arena *arena = vst3_host_state->arena;
  Vst3_PluginInstance *result = arena_push_struct(arena, Vst3_PluginInstance);

  // NOTE: find audio processor class info
  U64 class_count = plugin->class_count;
  PClassInfo *class_info = plugin->class_infos;
  for(U64 class_idx = 0; class_idx < class_count; ++class_idx, ++class_info)
  {
    Vst3_Category category = vst3_category_from_string(class_info->category);
    if(category == Vst3_Category_audio_processor) break;
  }

  // NOTE: create processor component instance
  IPluginFactory *factory = plugin->factory;
  IComponent *component = 0;
  FidString component_iid = vst3_cid_for_interface(Vst3_Interface_IComponent);
  IPluginFactory_CreateInstance(factory, class_info->cid, component_iid, (void**)&component);

  // NOTE: initialize component
  IHostApplication *context = &vst3_host_state->ihost_application;
  IComponent_Initialize(component, (FUnknown*)context);

  // NOTE: get edit controller class id
  Fuid edit_controller_iid = vst3_iid_for_interface(Vst3_Interface_IEditController);
  IEditController *edit_controller = 0;
  ok = IComponent_QueryInterface(component, edit_controller_iid.data, (void**)&edit_controller);
  if(ok != KResult_ok)
  {
    // NOTE: the plugin implements separated processor and controller (expected)
    Tuid edit_controller_cid = {0};
    IComponent_GetControllerClassID(component, edit_controller_cid);
    IPluginFactory_CreateInstance(factory, edit_controller_cid, edit_controller_iid.data, (void**)&edit_controller);
    IEditController_Initialize(edit_controller, (FUnknown*)context);
  }
  else
  {
    // NOTE: the plugin implements processor and controller on the same class
  }

  // TODO: set component handler for edit controller

  // NOTE: get/set states for component and controller
  Vst3_HostStream *stream = vst3_host_stream_alloc(arena, VST3_HOST_STREAM_SIZE_DEFAULT);
  IComponent_GetState(component, &stream->i);
  IEditController_SetComponentState(edit_controller, &stream->i);

  // NOTE: get parameter info
  U64 param_count = IEditController_GetParameterCount(edit_controller);
  ParameterInfo *param_infos = arena_push_array(arena, ParameterInfo, param_count);
  for(U64 param_idx = 0; param_idx < param_count; ++param_idx)
  {
    ParameterInfo *param_info = param_infos + param_idx;
    IEditController_GetParameterInfo(edit_controller, param_idx, param_info);
  }

  // NOTE: initialize audio processor
  Fuid audio_processor_iid = vst3_iid_for_interface(Vst3_Interface_IAudioProcessor);
  IAudioProcessor *audio_processor = 0;
  ok = IComponent_QueryInterface(component, audio_processor_iid.data, (void**)&audio_processor);
  Assert(ok == KResult_ok); // TODO: actual error handling/logging

  result->image = plugin;
  result->component = component;
  result->edit_controller = edit_controller;
  result->audio_processor = audio_processor;
  result->param_count = param_count;
  result->param_infos = param_infos;
  return(result);
}

proc void
vst3_host_plugin_uninstantiate(Vst3_PluginInstance *plugin)
{
  IComponent *component = plugin->component;
  IComponent_Terminate(component);
  IComponent_Release(component);
  plugin->component = 0;

  IEditController *edit_controller = plugin->edit_controller;
  IEditController_Terminate(edit_controller);
  IEditController_Release(edit_controller);
  plugin->edit_controller = 0;

  IAudioProcessor *audio_processor = plugin->audio_processor;
  IAudioProcessor_Release(audio_processor);
  plugin->audio_processor = 0;

  plugin->image = 0;
}

// -----------------------------------------------------------------------------
// helpers

proc Vst3_Category
vst3_category_from_string(U8 *category_cstr)
{
  String8 category_str8 = Str8Cstr((char*)category_cstr);
  Vst3_Category category = Vst3_Category_Count;
  if(str8s_are_equal(category_str8, Vst3_Category_audio_processor_string))
  {
    category = Vst3_Category_audio_processor;
  }
  else if(str8s_are_equal(category_str8, Vst3_Category_component_controller_string))
  {
    category = Vst3_Category_component_controller;
  }
  // TODO: more
  return(category);
}

proc Vst3_InterfaceSet
vst3_interfaces_for_category(U8 *category_cstr)
{
  Vst3_Category category = vst3_category_from_string(category_cstr);
  Vst3_InterfaceSet result = vst3_category_interfaces[category];
  return(result);
}

proc Fuid
vst3_iid_for_interface(Vst3_Interface i)
{
  Fuid result = vst3_fuids[i];
  return(result);
}

proc FidString
vst3_cid_for_interface(Vst3_Interface i)
{
  FidString result = vst3_fuids[i].data;
  return(result);
}

Vst3_HostApplicationImpl*
vst3_get_host_application_impl(IHostApplication *_this)
{
  Unused(_this);
  Vst3_HostApplicationImpl *result = &vst3_host_state->host_application_impl;
  return(result);
}

// -----------------------------------------------------------------------------
// IHostApplication implementations

proc TResult
IHostApplication_QueryInterface(IHostApplication *_this, const Tuid iid, void **obj)
{
  Unused(_this);
  Unused(iid);
  *obj = 0;

  TResult result = KResult_ok;
  return(result);
}

proc U32
IHostApplication_AddRef(IHostApplication *_this)
{
  Vst3_HostApplicationImpl *impl = vst3_get_host_application_impl(_this);
  ++impl->ref_count;

  U32 result = impl->ref_count;
  return(result);
}

proc U32
IHostApplication_Release(IHostApplication *_this)
{
  Vst3_HostApplicationImpl *impl = vst3_get_host_application_impl(_this);
  --impl->ref_count;

  U32 result = impl->ref_count;
  return(result);
}

proc TResult
IHostApplication_GetName(IHostApplication *_this, Vst3_String128 name)
{
  Unused(_this);
  char16_t *src = (char16_t*)vst3_host_state->name_str16.string;
  U64 count = vst3_host_state->name_str16.count;
  CopyArray(name, src, char16_t, count);

  TResult result = KResult_ok;
  return(result);
}

proc TResult
IHostApplication_CreateInstance(IHostApplication *_this, Tuid cid, Tuid iid, void **obj)
{
  Unused(_this);
  Unused(cid);
  Unused(iid);
  *obj = 0;

  TResult result = KResult_ok;
  return(result);
}

// -----------------------------------------------------------------------------
// IBStream implementations

proc Vst3_HostStream*
vst3_host_stream_alloc(Arena *arena, U64 size)
{
  Vst3_HostStream *result = arena_push_struct(arena, Vst3_HostStream);
  result->i.v = (void*)arena_push_array(arena, void*, ArrayCount(result->i.v->table));
  result->i.v->query_interface = vst3_host_stream_query_interface;
  result->i.v->add_ref = vst3_host_stream_add_ref;
  result->i.v->release = vst3_host_stream_release;
  result->i.v->read = vst3_host_stream_read;
  result->i.v->write = vst3_host_stream_write;
  result->i.v->seek = vst3_host_stream_seek;
  result->i.v->tell = vst3_host_stream_tell;
  result->size = size;
  result->pos = 0;
  result->data = arena_push_array(arena, U8, size);
  return(result);
}

proc TResult
vst3_host_stream_query_interface(IBStream *_this, const Tuid iid, void **obj)
{
  Unused(_this);
  Unused(iid);
  *obj = 0;

  TResult result = KResult_ok;
  return(result);
}

proc U32
vst3_host_stream_add_ref(IBStream *_this)
{
  Vst3_HostStream *stream = (Vst3_HostStream*)_this;
  ++stream->ref_count;

  U32 result = stream->ref_count;
  return(result);
}

proc U32
vst3_host_stream_release(IBStream *_this)
{
  Vst3_HostStream *stream = (Vst3_HostStream*)_this;
  --stream->ref_count;

  // TODO: free when ref count hits 0
  U32 result = stream->ref_count;
  return(result);
}

proc TResult
vst3_host_stream_read(IBStream *_this, void *buffer, S32 num_bytes, S32 *num_bytes_read)
{
  Vst3_HostStream *stream = (Vst3_HostStream*)_this;
  U8 *src = stream->data + stream->pos;
  U64 bytes_remaining = stream->size - stream->pos;

  U8 *dest = buffer;
  U64 bytes_to_read = Min((U64)num_bytes, bytes_remaining);
  CopyArray(dest, src, U8, bytes_to_read);
  stream->pos += bytes_to_read;
  *num_bytes_read = bytes_to_read;

  TResult result = KResult_ok;
  return(result);
}

proc TResult
vst3_host_stream_write(IBStream *_this, void *buffer, S32 num_bytes, S32 *num_bytes_written)
{
  Vst3_HostStream *stream = (Vst3_HostStream*)_this;
  U8 *dest = stream->data + stream->pos;
  U64 bytes_remaining = stream->size - stream->pos;

  U8 *src = buffer;
  U64 bytes_to_write = Min((U64)num_bytes, bytes_remaining);
  CopyArray(dest, src, U8, bytes_to_write);
  stream->pos += bytes_to_write;
  *num_bytes_written = bytes_to_write;

  TResult result = KResult_ok;
  return(result);
}

proc TResult
vst3_host_stream_seek(IBStream *_this, S64 pos, S32 mode, S64 *result)
{
  Vst3_HostStream *stream = (Vst3_HostStream*)_this;
  S64 new_pos = stream->pos;
  switch(mode)
  {
    case KIBStream_seek_set:
    {
      new_pos = pos;
    }break;
    case KIBStream_seek_cur:
    {
      new_pos = stream->pos + pos;
    }break;
    case KIBStream_seek_end:
    {
      new_pos = stream->size - pos;
    }break;
    // TODO: report error for unrecognized mode
  }

  stream->pos = new_pos;
  *result = new_pos;

  TResult status = KResult_ok;
  return(status);
}

proc TResult
vst3_host_stream_tell(IBStream *_this, S64 *pos)
{
  Vst3_HostStream *stream = (Vst3_HostStream*)_this;
  *pos = stream->pos;

  TResult result = KResult_ok;
  return(result);
}
