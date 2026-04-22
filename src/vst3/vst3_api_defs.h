#if defined(VST3_INTERFACES)

#if !defined(X)
#  define X(name, id0, id1, id2, id3)
#endif
#if !defined(Z)
#  define Z(count)
#endif
#undef VST3_INTERFACES

#define VST3_METHODS
  X(FUnknown, 0x00000000, 0x00000000, 0xC0000000, 0x00000046)
    #define CLASS_NAME FUnknown
    #define VST3_FUNKNOWN_METHODS
    #include __FILE__
    #undef VST3_FUNKNOWN_METHODS
    #undef CLASS_NAME
  Z(VST3_FUNKNOWN_METHOD_COUNT)

  X(IPluginFactory, 0x7A4D811C, 0x52114A1F, 0xAED9D2EE, 0x0B43BF9F)
    #define CLASS_NAME IPluginFactory
    #define VST3_IPLUGINFACTORY_METHODS
    #include __FILE__
    #undef VST3_IPLUGINFACTORY_METHODS
    #undef CLASS_NAME
  Z(VST3_IPLUGINFACTORY_METHOD_COUNT)

  X(IPluginBase, 0x22888DDB, 0x156E45AE, 0x8358B348, 0x08190625)
    #define CLASS_NAME IPluginBase
    #define VST3_IPLUGINBASE_METHODS
    #include __FILE__
    #undef VST3_IPLUGINBASE_METHODS
    #undef CLASS_NAME
  Z(VST3_IPLUGINBASE_METHOD_COUNT)

  X(IEditController, 0xDCD7BBE3, 0x7742448D, 0xA874AACC, 0x979C759E)
    #define CLASS_NAME IEditController
    #define VST3_IEDITCONTROLLER_METHODS
    #include __FILE__
    #undef VST3_IEDITCONTROLLER_METHODS
    #undef CLASS_NAME
  Z(VST3_IEDITCONTROLLER_METHOD_COUNT)

  X(IComponent, 0xE831FF31, 0xF2D54301, 0x928EBBEE, 0x25697802)
    #define CLASS_NAME IComponent
    #define VST3_ICOMPONENT_METHODS
    #include __FILE__
    #undef VST3_ICOMPONENT_METHODS
    #undef CLASS_NAME
  Z(VST3_ICOMPONENT_METHOD_COUNT)

  X(IAudioProcessor, 0x42043F99, 0xB7DA453C, 0xA569E79D, 0x9AAEC33D)
    #define CLASS_NAME IAudioProcessor
    #define VST3_IAUDIOPROCESSOR_METHODS
    #include __FILE__
    #undef VST3_IAUDIOPROCESSOR_METHODS
    #undef CLASS_NAME
  Z(VST3_IAUDIOPROCESSOR_METHOD_COUNT)

  X(IHostApplication, 0x58E595CC, 0xDB2D4969, 0x8B6AAF8C, 0x36A664E5)
    #define CLASS_NAME IHostApplication
    #define VST3_IHOSTAPPLICATION_METHODS
    #include __FILE__
    #undef VST3_IHOSTAPPLICATION_METHODS
    #undef CLASS_NAME
  Z(VST3_IHOSTAPPLICATION_METHOD_COUNT)

  X(IConnectionPoint, 0x70A4156F, 0x6E6E4026, 0x989148BF, 0xAA60D8D1)
    #define CLASS_NAME IConnectionPoint
    #define VST3_ICONNECTIONPOINT_METHODS
    #include __FILE__
    #undef VST3_ICONNECTIONPOINT_METHODS
    #undef CLASS_NAME
  Z(VST3_ICONNECTIONPOINT_METHOD_COUNT)

  X(IParameterChanges, 0xA4779663, 0x0BB64A56, 0xB44384A8, 0x466FEB9D)
    #define CLASS_NAME IParameterChanges
    #define VST3_IPARAMETERCHANGES_METHODS
    #include __FILE__
    #undef VST3_IPARAMETERCHANGES_METHODS
    #undef CLASS_NAME
  Z(VST3_IPARAMETERCHANGES_METHOD_COUNT)

  X(IEventList, 0x3A2C4214, 0x346349FE, 0xB2C4F397, 0xB9695A44)
    #define CLASS_NAME IEventList
    #define VST3_IEVENTLIST_METHODS
    #include __FILE__
    #undef VST3_IEVENTLIST_METHODS
    #undef CLASS_NAME
  Z(VST3_IEVENTLIST_METHOD_COUNT)

  X(IBStream, 0xC3BF6EA2, 0x30994752, 0x9B6BF990, 0x1EE33E9B)
    #define CLASS_NAME IBStream
    #define VST3_IBSTREAM_METHODS
    #include __FILE__
    #undef VST3_IBSTREAM_METHODS
    #undef CLASS_NAME
  Z(VST3_IBSTREAM_METHOD_COUNT)

  X(IComponentHandler, 0x93A0BEA3, 0x0BD045DB, 0x8E890B0C, 0xC1E46AC6)
    #define CLASS_NAME IComponentHandler
    #define VST3_ICOMPONENTHANDLER_METHODS
    #include __FILE__
    #undef VST3_ICOMPONENTHANDLER_METHODS
    #undef CLASS_NAME
  Z(VST3_ICOMPONENTHANDLER_METHOD_COUNT)

  X(IPlugFrame, 0x367FAF01, 0xAFA94693, 0x8D4DA2A0, 0xED0882A3)
    #define CLASS_NAME IPlugFrame
    #define VST3_IPLUGFRAME_METHODS
    #include __FILE__
    #undef VST3_IPLUGFRAME_METHODS
    #undef CLASS_NAME
  Z(VST3_IPLUGFRAME_METHOD_COUNT)

  X(IPlugView, 0x5BC32507, 0xD06049EA, 0xA6151B52, 0x2B755B29)
    #define CLASS_NAME IPlugView
    #define VST3_IPLUGVIEW_METHODS
    #include __FILE__
    #undef VST3_IPLUGVIEW_METHODS
    #undef CLASS_NAME
  Z(VST3_IPLUGVIEW_METHOD_COUNT)

  X(IAttributeList, 0x1E5F0AEB, 0xCC7F4533, 0xA2544011, 0x38AD5EE4)
    #define CLASS_NAME IAttributeList
    #define VST3_IATTRIBUTELIST_METHODS
    #include __FILE__
    #undef VST3_IATTRIBUTELIST_METHODS
    #undef CLASS_NAME
  Z(VST3_IATTRIBUTELIST_METHOD_COUNT)

  X(IMessage, 0x936F033B, 0xC6C047DB, 0xBB0882F8, 0x13C1E613)
    #define CLASS_NAME IMessage
    #define VST3_IMESSAGE_METHODS
    #include __FILE__
    #undef VST3_IMESSAGE_METHODS
    #undef CLASS_NAME
  Z(VST3_IMESSAGE_METHOD_COUNT)

  #undef VST3_METHODS
#endif

// TODO: is it possible to generate the interface definitions as well, by
// somehow saying which functions should be in each interface's vtable? how to
// get the table count?
#if defined(VST3_METHODS)
#if !defined(Y)
#  define Y(ret, name, args)
#endif

#  if defined(VST3_IEDITCONTROLLER_METHODS)
#    define VST3_IPLUGINBASE_METHODS
#  endif
#  if defined(VST3_ICOMPONENT_METHODS)
#    define VST3_IPLUGINBASE_METHODS
#  endif
#  if defined(VST3_IPLUGINFACTORY_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IPLUGINBASE_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IAUDIOPROCESSOR_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IHOSTAPPLICATION_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_ICONNECTIONPOINT_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IPARAMETERCHANGES_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IEVENTLIST_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IBSTREAM_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_ICOMPONENTHANDLER_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IPLUGFRAME_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IPLUGVIEW_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IATTRIBUTELIST_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif
#  if defined(VST3_IMESSAGE_METHODS)
#    define VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_FUNKNOWN_METHODS) // FUnknown
  Y(TResult, query_interface, (CLASS_NAME *_this, const Tuid iid, void **obj))
  Y(U32, add_ref, (CLASS_NAME *_this))
  Y(U32, release, (CLASS_NAME *_this))
#    define VST3_FUNKNOWN_METHOD_COUNT 3
#  endif

#  if defined(VST3_IPLUGINFACTORY_METHODS) // IPluginFactory
  Y(TResult, get_factory_info, (CLASS_NAME *_this, PFactoryInfo *info))
  Y(S32, count_classes, (CLASS_NAME *_this))
  Y(TResult, get_class_info, (CLASS_NAME *_this, S32 index, PClassInfo *info))
  Y(TResult, create_instance, (CLASS_NAME *_this, FidString cid, FidString iid, void **obj))
#    define VST3_IPLUGINFACTORY_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 4
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IPLUGINBASE_METHODS) // IPluginBase
  Y(TResult, initialize, (CLASS_NAME *_this, FUnknown *context))
  Y(TResult, terminate, (CLASS_NAME *_this))
#    define VST3_IPLUGINBASE_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 2
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IEDITCONTROLLER_METHODS) // IEditController
  Y(TResult,  set_component_state, (CLASS_NAME *_this, IBStream *state))
  Y(TResult, set_state, (CLASS_NAME *_this, IBStream *state))
  Y(TResult, get_state, (CLASS_NAME *_this, IBStream *state))
  Y(S32, get_parameter_count, (CLASS_NAME *_this))
  Y(TResult, get_parameter_info, (CLASS_NAME *_this, S32 param_index, ParameterInfo *info))
  Y(TResult, get_param_string_by_value, (CLASS_NAME *_this, ParamId id, ParamValue value_normalized, Vst3_String128 string))
  Y(TResult, get_param_value_by_string, (CLASS_NAME *_this, ParamId id, TChar *string, ParamValue *value_normalized))
  Y(ParamValue, normalized_param_to_plain, (CLASS_NAME *_this, ParamId id, ParamValue value_normalized))
  Y(ParamValue, plain_param_to_normalized, (CLASS_NAME *_this, ParamId id, ParamValue plain_value))
  Y(ParamValue, get_param_normalized, (CLASS_NAME *_this, ParamId id))
  Y(TResult, set_param_normalized, (CLASS_NAME *_this, ParamId id, ParamValue value))
  Y(TResult, set_component_handler, (CLASS_NAME *_this, IComponentHandler *handler))
  Y(IPlugView, *create_view, (CLASS_NAME *_this, FidString name))
#    define VST3_IEDITCONTROLLER_METHOD_COUNT VST3_IPLUGINBASE_METHOD_COUNT + 13
#    undef VST3_IPLUGINBASE_METHODS
#  endif

#  if defined(VST3_ICOMPONENT_METHODS) // IComponent
  Y(TResult, get_controller_class_id, (CLASS_NAME *_this, Tuid class_id))
  Y(TResult, set_io_mode, (CLASS_NAME *_this, IoMode mode))
  Y(S32, get_bus_count, (CLASS_NAME *_this, MediaType type, BusDirection dir))
  Y(TResult, get_bus_info, (CLASS_NAME *_this, MediaType type, BusDirection dir, S32 index, BusInfo *bus))
  Y(TResult, get_routing_info, (CLASS_NAME *_this, RoutingInfo *in_info, RoutingInfo *out_info))
  Y(TResult, activate_bus, (CLASS_NAME *_this, MediaType type, BusDirection dir, S32 index, TBool state))
  Y(TResult, set_active, (CLASS_NAME *_this, TBool state))
  Y(TResult, set_state, (CLASS_NAME *_this, IBStream *state))
  Y(TResult, get_state, (CLASS_NAME *_this, IBStream *state))
#    define VST3_ICOMPONENT_METHOD_COUNT VST3_IPLUGINBASE_METHOD_COUNT + 9
#    undef VST3_IPLUGINBASE_METHODS
#  endif

#  if defined(VST3_IAUDIOPROCESSOR_METHODS) // IAudioProcessor
#    define VST3_IAUDIOPROCESSOR_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 8
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IHOSTAPPLICATION_METHODS) // IHostApplication
  Y(TResult, get_name, (CLASS_NAME *_this, Vst3_String128 name))
  Y(TResult, create_instance, (CLASS_NAME *_this, Tuid cid, Tuid iid, void **obj))
#    define VST3_IHOSTAPPLICATION_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 2
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_ICONNECTIONPOINT_METHODS) // IConnectionPoint
  Y(TResult, connect, (CLASS_NAME *_this, IConnectionPoint *other))
  Y(TResult, disconnect, (CLASS_NAME *_this, IConnectionPoint *other))
  Y(TResult, notify, (CLASS_NAME *_this, IMessage *message))
#    define VST3_ICONNECTIONPOINT_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 3
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IPARAMETERCHANGES_METHODS) // IParameterChanges
#    define VST3_IPARAMETERCHANGES_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 3
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IEVENTLIST_METHODS) // IEventList
#    define VST3_IEVENTLIST_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 3
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IBSTREAM_METHODS) // IBStream
  Y(TResult, read, (CLASS_NAME *_this, void *buffer, S32 num_bytes, S32 *num_bytes_read))
  Y(TResult, write, (CLASS_NAME *_this, void *buffer, S32 num_bytes, S32 *num_bytes_written))
  Y(TResult, seek, (CLASS_NAME *_this, S64 pos, S32 mode, S64 *result))
  Y(TResult, tell, (CLASS_NAME *_this, S64 *pos))
#    define VST3_IBSTREAM_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 4
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_ICOMPONENTHANDLER_METHODS) // IComponentHandler
  Y(TResult, begin_edit, (CLASS_NAME *_this, ParamId id))
  Y(TResult, perform_edit, (CLASS_NAME *_this, ParamId id, ParamValue value_normalized))
  Y(TResult, end_edit, (CLASS_NAME *_this, ParamId id))
  Y(TResult, restart_component, (CLASS_NAME *_this, S32 flags))
#    define VST3_ICOMPONENTHANDLER_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 4
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IPLUGFRAME_METHODS) // IPlugFrame
#    define VST3_IPLUGFRAME_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 1
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IPLUGVIEW_METHODS) // IPlugView
#    define VST3_IPLUGVIEW_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 12
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IATTRIBUTELIST_METHODS) // IAttributeList
#    define VST3_IATTRIBUTELIST_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 8
#    undef VST3_FUNKNOWN_METHODS
#  endif

#  if defined(VST3_IMESSAGE_METHODS) // IMessage
#    define VST3_IMESSAGE_METHOD_COUNT VST3_FUNKNOWN_METHOD_COUNT + 3
#    undef VST3_FUNKNOWN_METHODS
#  endif

#endif
