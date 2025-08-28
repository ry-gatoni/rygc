// generated from src/os/windows/win32_metagen.cs
#include <initguid.h>

// guids

DEFINE_GUID(IID_IAcousticEchoCancellationControl,                    0xf4ae25b5, 0xaaa3, 0x437d, 0xb6, 0xb3, 0xdb, 0xbe, 0x2d, 0x0e, 0x95, 0x49);
DEFINE_GUID(IID_IActivateAudioInterfaceAsyncOperation,               0x72a22d78, 0xcde4, 0x431d, 0xb8, 0xcc, 0x84, 0x3a, 0x71, 0x19, 0x9b, 0x6d);
DEFINE_GUID(IID_IActivateAudioInterfaceCompletionHandler,            0x41d949ab, 0x9862, 0x444a, 0x80, 0xf6, 0xc2, 0x61, 0x33, 0x4d, 0xa5, 0xeb);
DEFINE_GUID(IID_IAudioAmbisonicsControl,                             0x28724c91, 0xdf35, 0x4856, 0x9f, 0x76, 0xd6, 0xa2, 0x64, 0x13, 0xf3, 0xdf);
DEFINE_GUID(IID_IAudioAutoGainControl,                               0x85401fd4, 0x6de4, 0x4b9d, 0x98, 0x69, 0x2d, 0x67, 0x53, 0xa8, 0x2f, 0x3c);
DEFINE_GUID(IID_IAudioBass,                                          0xa2b1a1d9, 0x4db3, 0x425d, 0xa2, 0xb2, 0xbd, 0x33, 0x5c, 0xb3, 0xe2, 0xe5);
DEFINE_GUID(IID_IAudioCaptureClient,                                 0xc8adbd64, 0xe71e, 0x48a0, 0xa4, 0xde, 0x18, 0x5c, 0x39, 0x5c, 0xd3, 0x17);
DEFINE_GUID(IID_IAudioChannelConfig,                                 0xbb11c46f, 0xec28, 0x493c, 0xb8, 0x8a, 0x5d, 0xb8, 0x80, 0x62, 0xce, 0x98);
DEFINE_GUID(IID_IAudioClient,                                        0x1cb9ad4c, 0xdbfa, 0x4c32, 0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2);
DEFINE_GUID(IID_IAudioClient2,                                       0x726778cd, 0xf60a, 0x4eda, 0x82, 0xde, 0xe4, 0x76, 0x10, 0xcd, 0x78, 0xaa);
DEFINE_GUID(IID_IAudioClient3,                                       0x7ed4ee07, 0x8e67, 0x4cd4, 0x8c, 0x1a, 0x2b, 0x7a, 0x59, 0x87, 0xad, 0x42);
DEFINE_GUID(IID_IAudioClientDuckingControl,                          0xc789d381, 0xa28c, 0x4168, 0xb2, 0x8f, 0xd3, 0xa8, 0x37, 0x92, 0x4d, 0xc3);
DEFINE_GUID(IID_IAudioClock,                                         0xcd63314f, 0x3fba, 0x4a1b, 0x81, 0x2c, 0xef, 0x96, 0x35, 0x87, 0x28, 0xe7);
DEFINE_GUID(IID_IAudioClock2,                                        0x6f49ff73, 0x6727, 0x49ac, 0xa0, 0x08, 0xd9, 0x8c, 0xf5, 0xe7, 0x00, 0x48);
DEFINE_GUID(IID_IAudioClockAdjustment,                               0xf6e4c0a0, 0x46d9, 0x4fb8, 0xbe, 0x21, 0x57, 0xa3, 0xef, 0x2b, 0x62, 0x6c);
DEFINE_GUID(IID_IAudioEffectsChangedNotificationClient,              0xa5ded44f, 0x3c5d, 0x4b2b, 0xbd, 0x1e, 0x5d, 0xc1, 0xee, 0x20, 0xbb, 0xf6);
DEFINE_GUID(IID_IAudioEffectsManager,                                0x4460b3ae, 0x4b44, 0x4527, 0x86, 0x76, 0x75, 0x48, 0xa8, 0xac, 0xd2, 0x60);
DEFINE_GUID(IID_IAudioFormatEnumerator,                              0xdcdaa858, 0x895a, 0x4a22, 0xa5, 0xeb, 0x67, 0xbd, 0xa5, 0x06, 0x09, 0x6d);
DEFINE_GUID(IID_IAudioInputSelector,                                 0x4f03dc02, 0x5e6e, 0x4653, 0x8f, 0x72, 0xa0, 0x30, 0xc1, 0x23, 0xd5, 0x98);
DEFINE_GUID(IID_IAudioLoudness,                                      0x7d8b1437, 0xdd53, 0x4350, 0x9c, 0x1b, 0x1e, 0xe2, 0x89, 0x0b, 0xd9, 0x38);
DEFINE_GUID(IID_IAudioMidrange,                                      0x5e54b6d7, 0xb44b, 0x40d9, 0x9a, 0x9e, 0xe6, 0x91, 0xd9, 0xce, 0x6e, 0xdf);
DEFINE_GUID(IID_IAudioMute,                                          0xdf45aeea, 0xb74a, 0x4b6b, 0xaf, 0xad, 0x23, 0x66, 0xb6, 0xaa, 0x01, 0x2e);
DEFINE_GUID(IID_IAudioOutputSelector,                                0xbb515f69, 0x94a7, 0x429e, 0x8b, 0x9c, 0x27, 0x1b, 0x3f, 0x11, 0xa3, 0xab);
DEFINE_GUID(IID_IAudioPeakMeter,                                     0xdd79923c, 0x0599, 0x45e0, 0xb8, 0xb6, 0xc8, 0xdf, 0x7d, 0xb6, 0xe7, 0x96);
DEFINE_GUID(IID_IAudioRenderClient,                                  0xf294acfc, 0x3146, 0x4483, 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2);
DEFINE_GUID(IID_IAudioSessionControl,                                0xf4b1a599, 0x7266, 0x4319, 0xa8, 0xca, 0xe7, 0x0a, 0xcb, 0x11, 0xe8, 0xcd);
DEFINE_GUID(IID_IAudioSessionControl2,                               0xbfb7ff88, 0x7239, 0x4fc9, 0x8f, 0xa2, 0x07, 0xc9, 0x50, 0xbe, 0x9c, 0x6d);
DEFINE_GUID(IID_IAudioSessionEnumerator,                             0xe2f5bb11, 0x0570, 0x40ca, 0xac, 0xdd, 0x3a, 0xa0, 0x12, 0x77, 0xde, 0xe8);
DEFINE_GUID(IID_IAudioSessionEvents,                                 0x24918acc, 0x64b3, 0x37c1, 0x8c, 0xa9, 0x74, 0xa6, 0x6e, 0x99, 0x57, 0xa8);
DEFINE_GUID(IID_IAudioSessionManager,                                0xbfa971f1, 0x4d5e, 0x40bb, 0x93, 0x5e, 0x96, 0x70, 0x39, 0xbf, 0xbe, 0xe4);
DEFINE_GUID(IID_IAudioSessionManager2,                               0x77aa99a0, 0x1bd6, 0x484f, 0x8b, 0xc7, 0x2c, 0x65, 0x4c, 0x9a, 0x9b, 0x6f);
DEFINE_GUID(IID_IAudioSessionNotification,                           0x641dd20b, 0x4d41, 0x49cc, 0xab, 0xa3, 0x17, 0x4b, 0x94, 0x77, 0xbb, 0x08);
DEFINE_GUID(IID_IAudioStateMonitor,                                  0x63bd8738, 0xe30d, 0x4c77, 0xbf, 0x5c, 0x83, 0x4e, 0x87, 0xc6, 0x57, 0xe2);
DEFINE_GUID(IID_IAudioStreamVolume,                                  0x93014887, 0x242d, 0x4068, 0x8a, 0x15, 0xcf, 0x5e, 0x93, 0xb9, 0x0f, 0xe3);
DEFINE_GUID(IID_IAudioSystemEffectsPropertyChangeNotificationClient, 0x20049d40, 0x56d5, 0x400e, 0xa2, 0xef, 0x38, 0x55, 0x99, 0xfe, 0xed, 0x49);
DEFINE_GUID(IID_IAudioSystemEffectsPropertyStore,                    0x302ae7f9, 0xd7e0, 0x43e4, 0x97, 0x1b, 0x1f, 0x82, 0x93, 0x61, 0x3d, 0x2a);
DEFINE_GUID(IID_IAudioTreble,                                        0x0a717812, 0x694e, 0x4907, 0xb7, 0x4b, 0xba, 0xfa, 0x5c, 0xfd, 0xca, 0x7b);
DEFINE_GUID(IID_IAudioViewManagerService,                            0xa7a7ef10, 0x1f49, 0x45e0, 0xad, 0x35, 0x61, 0x20, 0x57, 0xcc, 0x8f, 0x74);
DEFINE_GUID(IID_IAudioVolumeDuckNotification,                        0xc3b284d4, 0x6d39, 0x4359, 0xb3, 0xcf, 0xb5, 0x6d, 0xdb, 0x3b, 0xb3, 0x9c);
DEFINE_GUID(IID_IAudioVolumeLevel,                                   0x7fb7b48f, 0x531d, 0x44a2, 0xbc, 0xb3, 0x5a, 0xd5, 0xa1, 0x34, 0xb3, 0xdc);
DEFINE_GUID(IID_IChannelAudioVolume,                                 0x1c158861, 0xb533, 0x4b30, 0xb1, 0xcf, 0xe8, 0x53, 0xe5, 0x1c, 0x59, 0xb8);
DEFINE_GUID(IID_IConnector,                                          0x9c2c4058, 0x23f5, 0x41de, 0x87, 0x7a, 0xdf, 0x3a, 0xf2, 0x36, 0xa0, 0x9e);
DEFINE_GUID(IID_IControlChangeNotify,                                0xa09513ed, 0xc709, 0x4d21, 0xbd, 0x7b, 0x5f, 0x34, 0xc4, 0x7f, 0x39, 0x47);
DEFINE_GUID(IID_IControlInterface,                                   0x45d37c3f, 0x5140, 0x444a, 0xae, 0x24, 0x40, 0x07, 0x89, 0xf3, 0xcb, 0xf3);
DEFINE_GUID(IID_IDeviceSpecificProperty,                             0x3b22bcbf, 0x2586, 0x4af0, 0x85, 0x83, 0x20, 0x5d, 0x39, 0x1b, 0x80, 0x7c);
DEFINE_GUID(IID_IDeviceTopology,                                     0x2a07407e, 0x6497, 0x4a18, 0x97, 0x87, 0x32, 0xf7, 0x9b, 0xd0, 0xd9, 0x8f);
DEFINE_GUID(IID_IMessageFilter,                                      0x00000016, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
DEFINE_GUID(IID_IMMDevice,                                           0xd666063f, 0x1587, 0x4e43, 0x81, 0xf1, 0xb9, 0x48, 0xe8, 0x07, 0x36, 0x3f);
DEFINE_GUID(IID_IMMDeviceActivator,                                  0x3b0d0ea4, 0xd0a9, 0x4b0e, 0x93, 0x5b, 0x09, 0x51, 0x67, 0x46, 0xfa, 0xc0);
DEFINE_GUID(IID_IMMDeviceCollection,                                 0x0bd7a1be, 0x7a1a, 0x44db, 0x83, 0x97, 0xcc, 0x53, 0x92, 0x38, 0x7b, 0x5e);
DEFINE_GUID(IID_IMMDeviceEnumerator,                                 0xa95664d2, 0x9614, 0x4f35, 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6);
DEFINE_GUID(IID_IMMEndpoint,                                         0x1be09788, 0x6894, 0x4089, 0x85, 0x86, 0x9a, 0x2a, 0x6c, 0x26, 0x5a, 0xc5);
DEFINE_GUID(IID_IMMNotificationClient,                               0x7991eec9, 0x7e89, 0x4d85, 0x83, 0x90, 0x6c, 0x70, 0x3c, 0xec, 0x60, 0xc0);
DEFINE_GUID(IID_IPart,                                               0xae2de0e4, 0x5bca, 0x4f2d, 0xaa, 0x46, 0x5d, 0x13, 0xf8, 0xfd, 0xb3, 0xa9);
DEFINE_GUID(IID_IPartsList,                                          0x6daa848c, 0x5eb0, 0x45cc, 0xae, 0xa5, 0x99, 0x8a, 0x2c, 0xda, 0x1f, 0xfb);
DEFINE_GUID(IID_IPerChannelDbLevel,                                  0xc2f8e001, 0xf205, 0x4bc9, 0x99, 0xbc, 0xc1, 0x3b, 0x1e, 0x04, 0x8c, 0xcb);
DEFINE_GUID(IID_ISimpleAudioVolume,                                  0x87ce5498, 0x68d6, 0x44e5, 0x92, 0x15, 0x6d, 0xa4, 0x7e, 0xf8, 0x83, 0xd8);
DEFINE_GUID(IID_ISpatialAudioClient,                                 0xbbf8e066, 0xaaaa, 0x49be, 0x9a, 0x4d, 0xfd, 0x2a, 0x85, 0x8e, 0xa2, 0x7f);
DEFINE_GUID(IID_ISpatialAudioClient2,                                0xcaabe452, 0xa66a, 0x4bee, 0xa9, 0x3e, 0xe3, 0x20, 0x46, 0x3f, 0x6a, 0x53);
DEFINE_GUID(IID_ISpatialAudioMetadataClient,                         0x777d4a3b, 0xf6ff, 0x4a26, 0x85, 0xdc, 0x68, 0xd7, 0xcd, 0xed, 0xa1, 0xd4);
DEFINE_GUID(IID_ISpatialAudioMetadataCopier,                         0xd224b233, 0xe251, 0x4fd0, 0x9c, 0xa2, 0xd5, 0xec, 0xf9, 0xa6, 0x84, 0x04);
DEFINE_GUID(IID_ISpatialAudioMetadataItems,                          0xbcd7c78f, 0x3098, 0x4f22, 0xb5, 0x47, 0xa2, 0xf2, 0x5a, 0x38, 0x12, 0x69);
DEFINE_GUID(IID_ISpatialAudioMetadataItemsBuffer,                    0x42640a16, 0xe1bd, 0x42d9, 0x9f, 0xf6, 0x03, 0x1a, 0xb7, 0x1a, 0x2d, 0xba);
DEFINE_GUID(IID_ISpatialAudioMetadataReader,                         0xb78e86a2, 0x31d9, 0x4c32, 0x94, 0xd2, 0x7d, 0xf4, 0x0f, 0xc7, 0xeb, 0xec);
DEFINE_GUID(IID_ISpatialAudioMetadataWriter,                         0x1b17ca01, 0x2955, 0x444d, 0xa4, 0x30, 0x53, 0x7d, 0xc5, 0x89, 0xa8, 0x44);
DEFINE_GUID(IID_ISpatialAudioObject,                                 0xdde28967, 0x521b, 0x46e5, 0x8f, 0x00, 0xbd, 0x6f, 0x2b, 0xc8, 0xab, 0x1d);
DEFINE_GUID(IID_ISpatialAudioObjectBase,                             0xcce0b8f2, 0x8d4d, 0x4efb, 0xa8, 0xcf, 0x3d, 0x6e, 0xcf, 0x1c, 0x30, 0xe0);
DEFINE_GUID(IID_ISpatialAudioObjectForHrtf,                          0xd7436ade, 0x1978, 0x4e14, 0xab, 0xa0, 0x55, 0x5b, 0xd8, 0xeb, 0x83, 0xb4);
DEFINE_GUID(IID_ISpatialAudioObjectForMetadataCommands,              0x0df2c94b, 0xf5f9, 0x472d, 0xaf, 0x6b, 0xc4, 0x6e, 0x0a, 0xc9, 0xcd, 0x05);
DEFINE_GUID(IID_ISpatialAudioObjectForMetadataItems,                 0xddea49ff, 0x3bc0, 0x4377, 0x8a, 0xad, 0x9f, 0xbc, 0xfd, 0x80, 0x85, 0x66);
DEFINE_GUID(IID_ISpatialAudioObjectRenderStream,                     0xbab5f473, 0xb423, 0x477b, 0x85, 0xf5, 0xb5, 0xa3, 0x32, 0xa0, 0x41, 0x53);
DEFINE_GUID(IID_ISpatialAudioObjectRenderStreamBase,                 0xfeaaf403, 0xc1d8, 0x450d, 0xaa, 0x05, 0xe0, 0xcc, 0xee, 0x75, 0x02, 0xa8);
DEFINE_GUID(IID_ISpatialAudioObjectRenderStreamForHrtf,              0xe08deef9, 0x5363, 0x406e, 0x9f, 0xdc, 0x08, 0x0e, 0xe2, 0x47, 0xbb, 0xe0);
DEFINE_GUID(IID_ISpatialAudioObjectRenderStreamForMetadata,          0xbbc9c907, 0x48d5, 0x4a2e, 0xa0, 0xc7, 0xf7, 0xf0, 0xd6, 0x7c, 0x1f, 0xb1);
DEFINE_GUID(IID_ISpatialAudioObjectRenderStreamNotify,               0xdddf83e6, 0x68d7, 0x4c70, 0x88, 0x3f, 0xa1, 0x83, 0x6a, 0xfb, 0x4a, 0x50);
DEFINE_GUID(IID_ISubunit,                                            0x82149a85, 0xdba6, 0x4487, 0x86, 0xbb, 0xea, 0x8f, 0x7f, 0xef, 0xcc, 0x71);

// interfaces

typedef struct IAcousticEchoCancellationControl                     { struct { void *tbl[]; } *v; } IAcousticEchoCancellationControl;
typedef struct IActivateAudioInterfaceAsyncOperation                { struct { void *tbl[]; } *v; } IActivateAudioInterfaceAsyncOperation;
typedef struct IActivateAudioInterfaceCompletionHandler             { struct { void *tbl[]; } *v; } IActivateAudioInterfaceCompletionHandler;
typedef struct IAudioAmbisonicsControl                              { struct { void *tbl[]; } *v; } IAudioAmbisonicsControl;
typedef struct IAudioAutoGainControl                                { struct { void *tbl[]; } *v; } IAudioAutoGainControl;
typedef struct IAudioBass                                           { struct { void *tbl[]; } *v; } IAudioBass;
typedef struct IAudioCaptureClient                                  { struct { void *tbl[]; } *v; } IAudioCaptureClient;
typedef struct IAudioChannelConfig                                  { struct { void *tbl[]; } *v; } IAudioChannelConfig;
typedef struct IAudioClient                                         { struct { void *tbl[]; } *v; } IAudioClient;
typedef struct IAudioClient2                                        { struct { void *tbl[]; } *v; } IAudioClient2;
typedef struct IAudioClient3                                        { struct { void *tbl[]; } *v; } IAudioClient3;
typedef struct IAudioClientDuckingControl                           { struct { void *tbl[]; } *v; } IAudioClientDuckingControl;
typedef struct IAudioClock                                          { struct { void *tbl[]; } *v; } IAudioClock;
typedef struct IAudioClock2                                         { struct { void *tbl[]; } *v; } IAudioClock2;
typedef struct IAudioClockAdjustment                                { struct { void *tbl[]; } *v; } IAudioClockAdjustment;
typedef struct IAudioEffectsChangedNotificationClient               { struct { void *tbl[]; } *v; } IAudioEffectsChangedNotificationClient;
typedef struct IAudioEffectsManager                                 { struct { void *tbl[]; } *v; } IAudioEffectsManager;
typedef struct IAudioFormatEnumerator                               { struct { void *tbl[]; } *v; } IAudioFormatEnumerator;
typedef struct IAudioInputSelector                                  { struct { void *tbl[]; } *v; } IAudioInputSelector;
typedef struct IAudioLoudness                                       { struct { void *tbl[]; } *v; } IAudioLoudness;
typedef struct IAudioMidrange                                       { struct { void *tbl[]; } *v; } IAudioMidrange;
typedef struct IAudioMute                                           { struct { void *tbl[]; } *v; } IAudioMute;
typedef struct IAudioOutputSelector                                 { struct { void *tbl[]; } *v; } IAudioOutputSelector;
typedef struct IAudioPeakMeter                                      { struct { void *tbl[]; } *v; } IAudioPeakMeter;
typedef struct IAudioRenderClient                                   { struct { void *tbl[]; } *v; } IAudioRenderClient;
typedef struct IAudioSessionControl                                 { struct { void *tbl[]; } *v; } IAudioSessionControl;
typedef struct IAudioSessionControl2                                { struct { void *tbl[]; } *v; } IAudioSessionControl2;
typedef struct IAudioSessionEnumerator                              { struct { void *tbl[]; } *v; } IAudioSessionEnumerator;
typedef struct IAudioSessionEvents                                  { struct { void *tbl[]; } *v; } IAudioSessionEvents;
typedef struct IAudioSessionManager                                 { struct { void *tbl[]; } *v; } IAudioSessionManager;
typedef struct IAudioSessionManager2                                { struct { void *tbl[]; } *v; } IAudioSessionManager2;
typedef struct IAudioSessionNotification                            { struct { void *tbl[]; } *v; } IAudioSessionNotification;
typedef struct IAudioStateMonitor                                   { struct { void *tbl[]; } *v; } IAudioStateMonitor;
typedef struct IAudioStreamVolume                                   { struct { void *tbl[]; } *v; } IAudioStreamVolume;
typedef struct IAudioSystemEffectsPropertyChangeNotificationClient  { struct { void *tbl[]; } *v; } IAudioSystemEffectsPropertyChangeNotificationClient;
typedef struct IAudioSystemEffectsPropertyStore                     { struct { void *tbl[]; } *v; } IAudioSystemEffectsPropertyStore;
typedef struct IAudioTreble                                         { struct { void *tbl[]; } *v; } IAudioTreble;
typedef struct IAudioViewManagerService                             { struct { void *tbl[]; } *v; } IAudioViewManagerService;
typedef struct IAudioVolumeDuckNotification                         { struct { void *tbl[]; } *v; } IAudioVolumeDuckNotification;
typedef struct IAudioVolumeLevel                                    { struct { void *tbl[]; } *v; } IAudioVolumeLevel;
typedef struct IChannelAudioVolume                                  { struct { void *tbl[]; } *v; } IChannelAudioVolume;
typedef struct IConnector                                           { struct { void *tbl[]; } *v; } IConnector;
typedef struct IControlChangeNotify                                 { struct { void *tbl[]; } *v; } IControlChangeNotify;
typedef struct IControlInterface                                    { struct { void *tbl[]; } *v; } IControlInterface;
typedef struct IDeviceSpecificProperty                              { struct { void *tbl[]; } *v; } IDeviceSpecificProperty;
typedef struct IDeviceTopology                                      { struct { void *tbl[]; } *v; } IDeviceTopology;
typedef struct IMessageFilter                                       { struct { void *tbl[]; } *v; } IMessageFilter;
typedef struct IMMDevice                                            { struct { void *tbl[]; } *v; } IMMDevice;
typedef struct IMMDeviceActivator                                   { struct { void *tbl[]; } *v; } IMMDeviceActivator;
typedef struct IMMDeviceCollection                                  { struct { void *tbl[]; } *v; } IMMDeviceCollection;
typedef struct IMMDeviceEnumerator                                  { struct { void *tbl[]; } *v; } IMMDeviceEnumerator;
typedef struct IMMEndpoint                                          { struct { void *tbl[]; } *v; } IMMEndpoint;
typedef struct IMMNotificationClient                                { struct { void *tbl[]; } *v; } IMMNotificationClient;
typedef struct IPart                                                { struct { void *tbl[]; } *v; } IPart;
typedef struct IPartsList                                           { struct { void *tbl[]; } *v; } IPartsList;
typedef struct IPerChannelDbLevel                                   { struct { void *tbl[]; } *v; } IPerChannelDbLevel;
typedef struct ISimpleAudioVolume                                   { struct { void *tbl[]; } *v; } ISimpleAudioVolume;
typedef struct ISpatialAudioClient                                  { struct { void *tbl[]; } *v; } ISpatialAudioClient;
typedef struct ISpatialAudioClient2                                 { struct { void *tbl[]; } *v; } ISpatialAudioClient2;
typedef struct ISpatialAudioMetadataClient                          { struct { void *tbl[]; } *v; } ISpatialAudioMetadataClient;
typedef struct ISpatialAudioMetadataCopier                          { struct { void *tbl[]; } *v; } ISpatialAudioMetadataCopier;
typedef struct ISpatialAudioMetadataItems                           { struct { void *tbl[]; } *v; } ISpatialAudioMetadataItems;
typedef struct ISpatialAudioMetadataItemsBuffer                     { struct { void *tbl[]; } *v; } ISpatialAudioMetadataItemsBuffer;
typedef struct ISpatialAudioMetadataReader                          { struct { void *tbl[]; } *v; } ISpatialAudioMetadataReader;
typedef struct ISpatialAudioMetadataWriter                          { struct { void *tbl[]; } *v; } ISpatialAudioMetadataWriter;
typedef struct ISpatialAudioObject                                  { struct { void *tbl[]; } *v; } ISpatialAudioObject;
typedef struct ISpatialAudioObjectBase                              { struct { void *tbl[]; } *v; } ISpatialAudioObjectBase;
typedef struct ISpatialAudioObjectForHrtf                           { struct { void *tbl[]; } *v; } ISpatialAudioObjectForHrtf;
typedef struct ISpatialAudioObjectForMetadataCommands               { struct { void *tbl[]; } *v; } ISpatialAudioObjectForMetadataCommands;
typedef struct ISpatialAudioObjectForMetadataItems                  { struct { void *tbl[]; } *v; } ISpatialAudioObjectForMetadataItems;
typedef struct ISpatialAudioObjectRenderStream                      { struct { void *tbl[]; } *v; } ISpatialAudioObjectRenderStream;
typedef struct ISpatialAudioObjectRenderStreamBase                  { struct { void *tbl[]; } *v; } ISpatialAudioObjectRenderStreamBase;
typedef struct ISpatialAudioObjectRenderStreamForHrtf               { struct { void *tbl[]; } *v; } ISpatialAudioObjectRenderStreamForHrtf;
typedef struct ISpatialAudioObjectRenderStreamForMetadata           { struct { void *tbl[]; } *v; } ISpatialAudioObjectRenderStreamForMetadata;
typedef struct ISpatialAudioObjectRenderStreamNotify                { struct { void *tbl[]; } *v; } ISpatialAudioObjectRenderStreamNotify;
typedef struct ISubunit                                             { struct { void *tbl[]; } *v; } ISubunit;

// enums

typedef enum _AUDCLNT_BUFFERFLAGS
{
  AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY = 1,
  AUDCLNT_BUFFERFLAGS_SILENT             = 2,
  AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR    = 4,
} _AUDCLNT_BUFFERFLAGS;

typedef enum AMBISONICS_CHANNEL_ORDERING
{
  AMBISONICS_CHANNEL_ORDERING_ACN = 0,
} AMBISONICS_CHANNEL_ORDERING;

typedef enum AMBISONICS_NORMALIZATION
{
  AMBISONICS_NORMALIZATION_SN3D = 0,
  AMBISONICS_NORMALIZATION_N3D  = 1,
} AMBISONICS_NORMALIZATION;

typedef enum AMBISONICS_TYPE
{
  AMBISONICS_TYPE_FULL3D = 0,
} AMBISONICS_TYPE;

typedef enum AUDCLNT_SHAREMODE
{
  AUDCLNT_SHAREMODE_SHARED    = 0,
  AUDCLNT_SHAREMODE_EXCLUSIVE = 1,
} AUDCLNT_SHAREMODE;

typedef enum AUDCLNT_STREAMOPTIONS
{
  AUDCLNT_STREAMOPTIONS_NONE         = 0,
  AUDCLNT_STREAMOPTIONS_RAW          = 1,
  AUDCLNT_STREAMOPTIONS_MATCH_FORMAT = 2,
  AUDCLNT_STREAMOPTIONS_AMBISONICS   = 4,
} AUDCLNT_STREAMOPTIONS;

typedef enum AUDIO_DUCKING_OPTIONS
{
  AUDIO_DUCKING_OPTIONS_DEFAULT                   = 0,
  AUDIO_DUCKING_OPTIONS_DO_NOT_DUCK_OTHER_STREAMS = 1,
} AUDIO_DUCKING_OPTIONS;

typedef enum AUDIO_EFFECT_STATE
{
  AUDIO_EFFECT_STATE_OFF = 0,
  AUDIO_EFFECT_STATE_ON  = 1,
} AUDIO_EFFECT_STATE;

typedef enum AUDIO_STREAM_CATEGORY
{
  AudioCategory_Other               = 0,
  AudioCategory_ForegroundOnlyMedia = 1,
  AudioCategory_Communications      = 3,
  AudioCategory_Alerts              = 4,
  AudioCategory_SoundEffects        = 5,
  AudioCategory_GameEffects         = 6,
  AudioCategory_GameMedia           = 7,
  AudioCategory_GameChat            = 8,
  AudioCategory_Speech              = 9,
  AudioCategory_Movie               = 10,
  AudioCategory_Media               = 11,
  AudioCategory_FarFieldSpeech      = 12,
  AudioCategory_UniformSpeech       = 13,
  AudioCategory_VoiceTyping         = 14,
} AUDIO_STREAM_CATEGORY;

typedef enum AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE
{
  AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE_DEFAULT    = 0,
  AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE_USER       = 1,
  AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE_VOLATILE   = 2,
  AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE_ENUM_COUNT = 3,
} AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE;

typedef enum AUDIOCLIENT_ACTIVATION_TYPE
{
  AUDIOCLIENT_ACTIVATION_TYPE_DEFAULT          = 0,
  AUDIOCLIENT_ACTIVATION_TYPE_PROCESS_LOOPBACK = 1,
} AUDIOCLIENT_ACTIVATION_TYPE;

typedef enum AudioObjectType
{
  AudioObjectType_None             = 0,
  AudioObjectType_Dynamic          = 1,
  AudioObjectType_FrontLeft        = 2,
  AudioObjectType_FrontRight       = 4,
  AudioObjectType_FrontCenter      = 8,
  AudioObjectType_LowFrequency     = 16,
  AudioObjectType_SideLeft         = 32,
  AudioObjectType_SideRight        = 64,
  AudioObjectType_BackLeft         = 128,
  AudioObjectType_BackRight        = 256,
  AudioObjectType_TopFrontLeft     = 512,
  AudioObjectType_TopFrontRight    = 1024,
  AudioObjectType_TopBackLeft      = 2048,
  AudioObjectType_TopBackRight     = 4096,
  AudioObjectType_BottomFrontLeft  = 8192,
  AudioObjectType_BottomFrontRight = 16384,
  AudioObjectType_BottomBackLeft   = 32768,
  AudioObjectType_BottomBackRight  = 65536,
  AudioObjectType_BackCenter       = 131072,
} AudioObjectType;

typedef enum AudioSessionDisconnectReason
{
  DisconnectReasonDeviceRemoval         = 0,
  DisconnectReasonServerShutdown        = 1,
  DisconnectReasonFormatChanged         = 2,
  DisconnectReasonSessionLogoff         = 3,
  DisconnectReasonSessionDisconnected   = 4,
  DisconnectReasonExclusiveModeOverride = 5,
} AudioSessionDisconnectReason;

typedef enum AudioSessionState
{
  AudioSessionStateInactive = 0,
  AudioSessionStateActive   = 1,
  AudioSessionStateExpired  = 2,
} AudioSessionState;

typedef enum AudioStateMonitorSoundLevel
{
  Muted = 0,
  Low   = 1,
  Full  = 2,
} AudioStateMonitorSoundLevel;

typedef enum ConnectorType
{
  Unknown_Connector = 0,
  Physical_Internal = 1,
  Physical_External = 2,
  Software_IO       = 3,
  Software_Fixed    = 4,
  Network           = 5,
} ConnectorType;

typedef enum DataFlow
{
  In  = 0,
  Out = 1,
} DataFlow;

typedef enum DEVICE_STATE
{
  DEVICE_STATE_ACTIVE     = 0x00000001,
  DEVICE_STATE_DISABLED   = 0x00000002,
  DEVICE_STATE_NOTPRESENT = 0x00000004,
  DEVICE_STATE_UNPLUGGED  = 0x00000008,
} DEVICE_STATE;

typedef enum EDataFlow
{
  eRender              = 0,
  eCapture             = 1,
  eAll                 = 2,
  EDataFlow_enum_count = 3,
} EDataFlow;

typedef enum EndpointFormFactor
{
  RemoteNetworkDevice           = 0,
  Speakers                      = 1,
  LineLevel                     = 2,
  Headphones                    = 3,
  Microphone                    = 4,
  Headset                       = 5,
  Handset                       = 6,
  UnknownDigitalPassthrough     = 7,
  SPDIF                         = 8,
  DigitalAudioDisplayDevice     = 9,
  UnknownFormFactor             = 10,
  EndpointFormFactor_enum_count = 11,
} EndpointFormFactor;

typedef enum ERole
{
  eConsole         = 0,
  eMultimedia      = 1,
  eCommunications  = 2,
  ERole_enum_count = 3,
} ERole;

typedef enum MIDI_WAVE_OPEN_TYPE
{
  CALLBACK_TYPEMASK                        = 0x00070000,
  CALLBACK_NULL                            = 0x00000000,
  CALLBACK_WINDOW                          = 0x00010000,
  CALLBACK_TASK                            = 0x00020000,
  CALLBACK_FUNCTION                        = 0x00030000,
  CALLBACK_THREAD                          = 0x00020000,
  CALLBACK_EVENT                           = 0x00050000,
  WAVE_FORMAT_QUERY                        = 0x00000001,
  WAVE_ALLOWSYNC                           = 0x00000002,
  WAVE_MAPPED                              = 0x00000004,
  WAVE_FORMAT_DIRECT                       = 0x00000008,
  WAVE_FORMAT_DIRECT_QUERY                 = 0x00000009,
  WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE = 0x00000010,
  MIDI_IO_STATUS                           = 0x00000020,
} MIDI_WAVE_OPEN_TYPE;

typedef enum MIXERLINE_COMPONENTTYPE
{
  MIXERLINE_COMPONENTTYPE_DST_DIGITAL     = 0x00000001,
  MIXERLINE_COMPONENTTYPE_DST_HEADPHONES  = 0x00000005,
  MIXERLINE_COMPONENTTYPE_DST_LINE        = 0x00000002,
  MIXERLINE_COMPONENTTYPE_DST_MONITOR     = 0x00000003,
  MIXERLINE_COMPONENTTYPE_DST_SPEAKERS    = 0x00000004,
  MIXERLINE_COMPONENTTYPE_DST_TELEPHONE   = 0x00000006,
  MIXERLINE_COMPONENTTYPE_DST_UNDEFINED   = 0x00000000,
  MIXERLINE_COMPONENTTYPE_DST_VOICEIN     = 0x00000008,
  MIXERLINE_COMPONENTTYPE_DST_WAVEIN      = 0x00000007,
  MIXERLINE_COMPONENTTYPE_SRC_ANALOG      = 0x0000100a,
  MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY   = 0x00001009,
  MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC = 0x00001005,
  MIXERLINE_COMPONENTTYPE_SRC_DIGITAL     = 0x00001001,
  MIXERLINE_COMPONENTTYPE_SRC_LINE        = 0x00001002,
  MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE  = 0x00001003,
  MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER   = 0x00001007,
  MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER = 0x00001004,
  MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE   = 0x00001006,
  MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED   = 0x00001000,
  MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT     = 0x00001008,
} MIXERLINE_COMPONENTTYPE;

typedef enum PartType
{
  Connector = 0,
  Subunit   = 1,
} PartType;

typedef enum PROCESS_LOOPBACK_MODE
{
  PROCESS_LOOPBACK_MODE_INCLUDE_TARGET_PROCESS_TREE = 0,
  PROCESS_LOOPBACK_MODE_EXCLUDE_TARGET_PROCESS_TREE = 1,
} PROCESS_LOOPBACK_MODE;

typedef enum SND_FLAGS
{
  SND_APPLICATION = 0x00000080,
  SND_ALIAS       = 0x00010000,
  SND_ALIAS_ID    = 0x00110000,
  SND_FILENAME    = 0x00020000,
  SND_RESOURCE    = 0x00040004,
  SND_ASYNC       = 0x00000001,
  SND_NODEFAULT   = 0x00000002,
  SND_LOOP        = 0x00000008,
  SND_MEMORY      = 0x00000004,
  SND_NOSTOP      = 0x00000010,
  SND_NOWAIT      = 0x00002000,
  SND_PURGE       = 0x00000040,
  SND_SENTRY      = 0x00080000,
  SND_SYNC        = 0x00000000,
  SND_SYSTEM      = 0x00200000,
} SND_FLAGS;

typedef enum SPATIAL_AUDIO_STREAM_OPTIONS
{
  SPATIAL_AUDIO_STREAM_OPTIONS_NONE    = 0,
  SPATIAL_AUDIO_STREAM_OPTIONS_OFFLOAD = 1,
} SPATIAL_AUDIO_STREAM_OPTIONS;

typedef enum SpatialAudioHrtfDirectivityType
{
  SpatialAudioHrtfDirectivity_OmniDirectional = 0,
  SpatialAudioHrtfDirectivity_Cardioid        = 1,
  SpatialAudioHrtfDirectivity_Cone            = 2,
} SpatialAudioHrtfDirectivityType;

typedef enum SpatialAudioHrtfDistanceDecayType
{
  SpatialAudioHrtfDistanceDecay_NaturalDecay = 0,
  SpatialAudioHrtfDistanceDecay_CustomDecay  = 1,
} SpatialAudioHrtfDistanceDecayType;

typedef enum SpatialAudioHrtfEnvironmentType
{
  SpatialAudioHrtfEnvironment_Small    = 0,
  SpatialAudioHrtfEnvironment_Medium   = 1,
  SpatialAudioHrtfEnvironment_Large    = 2,
  SpatialAudioHrtfEnvironment_Outdoors = 3,
  SpatialAudioHrtfEnvironment_Average  = 4,
} SpatialAudioHrtfEnvironmentType;

typedef enum SpatialAudioMetadataCopyMode
{
  SpatialAudioMetadataCopy_Overwrite            = 0,
  SpatialAudioMetadataCopy_Append               = 1,
  SpatialAudioMetadataCopy_AppendMergeWithLast  = 2,
  SpatialAudioMetadataCopy_AppendMergeWithFirst = 3,
} SpatialAudioMetadataCopyMode;

typedef enum SpatialAudioMetadataWriterOverflowMode
{
  SpatialAudioMetadataWriterOverflow_Fail          = 0,
  SpatialAudioMetadataWriterOverflow_MergeWithNew  = 1,
  SpatialAudioMetadataWriterOverflow_MergeWithLast = 2,
} SpatialAudioMetadataWriterOverflowMode;

// structs

typedef struct ACMDRIVERDETAILSA
{
  UINT32        cbStruct;
  UINT32        fccType;
  UINT32        fccComp;
  UINT16        wMid;
  UINT16        wPid;
  UINT32        vdwACM;
  UINT32        vdwDriver;
  UINT32        fdwSupport;
  UINT32        cFormatTags;
  UINT32        cFilterTags;
  HICON         hicon;
  CHAR          szShortName[32];
  CHAR          szLongName[128];
  CHAR          szCopyright[80];
  CHAR          szLicensing[128];
  CHAR          szFeatures[512];
} ACMDRIVERDETAILSA;

typedef struct ACMDRIVERDETAILSW
{
  UINT32        cbStruct;
  UINT32        fccType;
  UINT32        fccComp;
  UINT16        wMid;
  UINT16        wPid;
  UINT32        vdwACM;
  UINT32        vdwDriver;
  UINT32        fdwSupport;
  UINT32        cFormatTags;
  UINT32        cFilterTags;
  HICON         hicon;
  CHAR          szShortName[32];
  CHAR          szLongName[128];
  CHAR          szCopyright[80];
  CHAR          szLicensing[128];
  CHAR          szFeatures[512];
} ACMDRIVERDETAILSW;

typedef struct ACMDRVFORMATSUGGEST
{
  UINT32        cbStruct;
  UINT32        fdwSuggest;
  WAVEFORMATEX* pwfxSrc;
  UINT32        cbwfxSrc;
  WAVEFORMATEX* pwfxDst;
  UINT32        cbwfxDst;
} ACMDRVFORMATSUGGEST;

typedef struct ACMDRVOPENDESCA
{
  UINT32 cbStruct;
  UINT32 fccType;
  UINT32 fccComp;
  UINT32 dwVersion;
  UINT32 dwFlags;
  UINT32 dwError;
  PSTR   pszSectionName;
  PSTR   pszAliasName;
  UINT32 dnDevNode;
} ACMDRVOPENDESCA;

typedef struct ACMDRVOPENDESCW
{
  UINT32 cbStruct;
  UINT32 fccType;
  UINT32 fccComp;
  UINT32 dwVersion;
  UINT32 dwFlags;
  UINT32 dwError;
  WCHAR* pszSectionName;
  WCHAR* pszAliasName;
  UINT32 dnDevNode;
} ACMDRVOPENDESCW;

typedef struct ACMDRVSTREAMHEADER
{
  UINT32              cbStruct;
  UINT32              fdwStatus;
  DWORD_PTR           dwUser;
  UINT8*              pbSrc;
  UINT32              cbSrcLength;
  UINT32              cbSrcLengthUsed;
  DWORD_PTR           dwSrcUser;
  UINT8*              pbDst;
  UINT32              cbDstLength;
  UINT32              cbDstLengthUsed;
  DWORD_PTR           dwDstUser;
  UINT32              fdwConvert;
  ACMDRVSTREAMHEADER* padshNext;
  UINT32              fdwDriver;
  DWORD_PTR           dwDriver;
  UINT32              fdwPrepared;
  DWORD_PTR           dwPrepared;
  UINT8*              pbPreparedSrc;
  UINT32              cbPreparedSrcLength;
  UINT8*              pbPreparedDst;
  UINT32              cbPreparedDstLength;
} ACMDRVSTREAMHEADER;

typedef struct ACMDRVSTREAMINSTANCE
{
  UINT32        cbStruct;
  WAVEFORMATEX* pwfxSrc;
  WAVEFORMATEX* pwfxDst;
  WAVEFILTER*   pwfltr;
  DWORD_PTR     dwCallback;
  DWORD_PTR     dwInstance;
  UINT32        fdwOpen;
  UINT32        fdwDriver;
  DWORD_PTR     dwDriver;
  HACMSTREAM    has;
} ACMDRVSTREAMINSTANCE;

typedef struct ACMDRVSTREAMSIZE
{
  UINT32 cbStruct;
  UINT32 fdwSize;
  UINT32 cbSrcLength;
  UINT32 cbDstLength;
} ACMDRVSTREAMSIZE;

typedef struct ACMFILTERCHOOSEA
{
  UINT32                   cbStruct;
  UINT32                   fdwStyle;
  HWND                     hwndOwner;
  WAVEFILTER*              pwfltr;
  UINT32                   cbwfltr;
  PSTR                     pszTitle;
  CHAR                     szFilterTag[48];
  CHAR                     szFilter[128];
  PSTR                     pszName;
  UINT32                   cchName;
  UINT32                   fdwEnum;
  WAVEFILTER*              pwfltrEnum;
  HINSTANCE                hInstance;
  PSTR                     pszTemplateName;
  LPARAM                   lCustData;
  ACMFILTERCHOOSEHOOKPROCA pfnHook;
} ACMFILTERCHOOSEA;

typedef struct ACMFILTERCHOOSEW
{
  UINT32                   cbStruct;
  UINT32                   fdwStyle;
  HWND                     hwndOwner;
  WAVEFILTER*              pwfltr;
  UINT32                   cbwfltr;
  WCHAR*                   pszTitle;
  CHAR                     szFilterTag[48];
  CHAR                     szFilter[128];
  WCHAR*                   pszName;
  UINT32                   cchName;
  UINT32                   fdwEnum;
  WAVEFILTER*              pwfltrEnum;
  HINSTANCE                hInstance;
  WCHAR*                   pszTemplateName;
  LPARAM                   lCustData;
  ACMFILTERCHOOSEHOOKPROCW pfnHook;
} ACMFILTERCHOOSEW;

typedef struct ACMFILTERDETAILSA
{
  UINT32        cbStruct;
  UINT32        dwFilterIndex;
  UINT32        dwFilterTag;
  UINT32        fdwSupport;
  WAVEFILTER*   pwfltr;
  UINT32        cbwfltr;
  CHAR          szFilter[128];
} ACMFILTERDETAILSA;

typedef struct ACMFILTERDETAILSW
{
  UINT32        cbStruct;
  UINT32        dwFilterIndex;
  UINT32        dwFilterTag;
  UINT32        fdwSupport;
  WAVEFILTER*   pwfltr;
  UINT32        cbwfltr;
  CHAR          szFilter[128];
} ACMFILTERDETAILSW;

typedef struct ACMFILTERTAGDETAILSA
{
  UINT32       cbStruct;
  UINT32       dwFilterTagIndex;
  UINT32       dwFilterTag;
  UINT32       cbFilterSize;
  UINT32       fdwSupport;
  UINT32       cStandardFilters;
  CHAR         szFilterTag[48];
} ACMFILTERTAGDETAILSA;

typedef struct ACMFILTERTAGDETAILSW
{
  UINT32       cbStruct;
  UINT32       dwFilterTagIndex;
  UINT32       dwFilterTag;
  UINT32       cbFilterSize;
  UINT32       fdwSupport;
  UINT32       cStandardFilters;
  CHAR         szFilterTag[48];
} ACMFILTERTAGDETAILSW;

typedef struct ACMFORMATCHOOSEA
{
  UINT32                   cbStruct;
  UINT32                   fdwStyle;
  HWND                     hwndOwner;
  WAVEFORMATEX*            pwfx;
  UINT32                   cbwfx;
  PSTR                     pszTitle;
  CHAR                     szFormatTag[48];
  CHAR                     szFormat[128];
  PSTR                     pszName;
  UINT32                   cchName;
  UINT32                   fdwEnum;
  WAVEFORMATEX*            pwfxEnum;
  HINSTANCE                hInstance;
  PSTR                     pszTemplateName;
  LPARAM                   lCustData;
  ACMFORMATCHOOSEHOOKPROCA pfnHook;
} ACMFORMATCHOOSEA;

typedef struct ACMFORMATCHOOSEW
{
  UINT32                   cbStruct;
  UINT32                   fdwStyle;
  HWND                     hwndOwner;
  WAVEFORMATEX*            pwfx;
  UINT32                   cbwfx;
  WCHAR*                   pszTitle;
  CHAR                     szFormatTag[48];
  CHAR                     szFormat[128];
  WCHAR*                   pszName;
  UINT32                   cchName;
  UINT32                   fdwEnum;
  WAVEFORMATEX*            pwfxEnum;
  HINSTANCE                hInstance;
  WCHAR*                   pszTemplateName;
  LPARAM                   lCustData;
  ACMFORMATCHOOSEHOOKPROCW pfnHook;
} ACMFORMATCHOOSEW;

typedef struct ACMFORMATDETAILSA
{
  UINT32        cbStruct;
  UINT32        dwFormatIndex;
  UINT32        dwFormatTag;
  UINT32        fdwSupport;
  WAVEFORMATEX* pwfx;
  UINT32        cbwfx;
  CHAR          szFormat[128];
} ACMFORMATDETAILSA;

typedef struct ACMFORMATTAGDETAILSA
{
  UINT32       cbStruct;
  UINT32       dwFormatTagIndex;
  UINT32       dwFormatTag;
  UINT32       cbFormatSize;
  UINT32       fdwSupport;
  UINT32       cStandardFormats;
  CHAR         szFormatTag[48];
} ACMFORMATTAGDETAILSA;

typedef struct ACMFORMATTAGDETAILSW
{
  UINT32       cbStruct;
  UINT32       dwFormatTagIndex;
  UINT32       dwFormatTag;
  UINT32       cbFormatSize;
  UINT32       fdwSupport;
  UINT32       cStandardFormats;
  CHAR         szFormatTag[48];
} ACMFORMATTAGDETAILSW;

typedef struct ACMSTREAMHEADER
{
  UINT32         cbStruct;
  UINT32         fdwStatus;
  DWORD_PTR      dwUser;
  UINT8*         pbSrc;
  UINT32         cbSrcLength;
  UINT32         cbSrcLengthUsed;
  DWORD_PTR      dwSrcUser;
  UINT8*         pbDst;
  UINT32         cbDstLength;
  UINT32         cbDstLengthUsed;
  DWORD_PTR      dwDstUser;
  UINT32         dwReservedDriver[15];
} ACMSTREAMHEADER;

typedef struct ACMSTREAMHEADER
{
  UINT32        cbStruct;
  UINT32        fdwStatus;
  DWORD_PTR     dwUser;
  UINT8*        pbSrc;
  UINT32        cbSrcLength;
  UINT32        cbSrcLengthUsed;
  DWORD_PTR     dwSrcUser;
  UINT8*        pbDst;
  UINT32        cbDstLength;
  UINT32        cbDstLengthUsed;
  DWORD_PTR     dwDstUser;
  UINT32        dwReservedDriver[10];
} ACMSTREAMHEADER;

typedef struct AMBISONICS_PARAMS
{
  UINT32                      u32Size;
  UINT32                      u32Version;
  AMBISONICS_TYPE             u32Type;
  AMBISONICS_CHANNEL_ORDERING u32ChannelOrdering;
  AMBISONICS_NORMALIZATION    u32Normalization;
  UINT32                      u32Order;
  UINT32                      u32NumChannels;
  UINT32*                     pu32ChannelMap;
} AMBISONICS_PARAMS;

typedef struct AUDIO_EFFECT
{
  Guid               id;
  BOOL               canSetState;
  AUDIO_EFFECT_STATE state;
} AUDIO_EFFECT;

typedef struct AUDIO_VOLUME_NOTIFICATION_DATA
{
  Guid          guidEventContext;
  BOOL          bMuted;
  FLOAT         fMasterVolume;
  UINT32        nChannels;
  FLOAT         afChannelVolumes[1];
} AUDIO_VOLUME_NOTIFICATION_DATA;

typedef struct AUDIOCLIENT_ACTIVATION_PARAMS
{
  AUDIOCLIENT_ACTIVATION_TYPE ActivationType;
  struct
  {
    AUDIOCLIENT_PROCESS_LOOPBACK_PARAMS ProcessLoopbackParams;
  } Anonymous;
} AUDIOCLIENT_ACTIVATION_PARAMS;

typedef struct AUDIOCLIENT_PROCESS_LOOPBACK_PARAMS
{
  UINT32                TargetProcessId;
  PROCESS_LOOPBACK_MODE ProcessLoopbackMode;
} AUDIOCLIENT_PROCESS_LOOPBACK_PARAMS;

typedef struct AudioClient3ActivationParams
{
  Guid tracingContextId;
} AudioClient3ActivationParams;

typedef struct AudioClientProperties
{
  UINT32                cbSize;
  BOOL                  bIsOffload;
  AUDIO_STREAM_CATEGORY eCategory;
  AUDCLNT_STREAMOPTIONS Options;
} AudioClientProperties;

typedef struct AudioExtensionParams
{
  LPARAM     AddPageParam;
  IMMDevice* pEndpoint;
  IMMDevice* pPnpInterface;
  IMMDevice* pPnpDevnode;
} AudioExtensionParams;

typedef struct AUXCAPS2A
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wReserved1;
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} AUXCAPS2A;

typedef struct AUXCAPS2W
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wReserved1;
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} AUXCAPS2W;

typedef struct AUXCAPSA
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wReserved1;
  UINT32       dwSupport;
} AUXCAPSA;

typedef struct AUXCAPSW
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wReserved1;
  UINT32       dwSupport;
} AUXCAPSW;

typedef struct DeviceTopology
{
} DeviceTopology;

typedef struct DIRECTX_AUDIO_ACTIVATION_PARAMS
{
  UINT32 cbDirectXAudioActivationParams;
  Guid   guidAudioSession;
  UINT32 dwAudioStreamFlags;
} DIRECTX_AUDIO_ACTIVATION_PARAMS;

typedef struct ECHOWAVEFILTER
{
  WAVEFILTER wfltr;
  UINT32     dwVolume;
  UINT32     dwDelay;
} ECHOWAVEFILTER;

typedef struct HACMDRIVER
{
  void* Value;
} HACMDRIVER;

typedef struct HACMDRIVERID
{
  void* Value;
} HACMDRIVERID;

typedef struct HACMOBJ
{
  void* Value;
} HACMOBJ;

typedef struct HACMSTREAM
{
  void* Value;
} HACMSTREAM;

typedef struct HMIDI
{
  void* Value;
} HMIDI;

typedef struct HMIDIIN
{
  void* Value;
} HMIDIIN;

typedef struct HMIDIOUT
{
  void* Value;
} HMIDIOUT;

typedef struct HMIDISTRM
{
  void* Value;
} HMIDISTRM;

typedef struct HMIXER
{
  void* Value;
} HMIXER;

typedef struct HMIXEROBJ
{
  void* Value;
} HMIXEROBJ;

typedef struct HWAVE
{
  void* Value;
} HWAVE;

typedef struct HWAVEIN
{
  void* Value;
} HWAVEIN;

typedef struct HWAVEOUT
{
  void* Value;
} HWAVEOUT;

typedef struct MIDIEVENT
{
  UINT32        dwDeltaTime;
  UINT32        dwStreamID;
  UINT32        dwEvent;
  UINT32        dwParms[1];
} MIDIEVENT;

typedef struct MIDIHDR
{
  PSTR           lpData;
  UINT32         dwBufferLength;
  UINT32         dwBytesRecorded;
  DWORD_PTR      dwUser;
  UINT32         dwFlags;
  MIDIHDR*       lpNext;
  DWORD_PTR      reserved;
  UINT32         dwOffset;
  DWORD_PTR      dwReserved[8];
} MIDIHDR;

typedef struct MIDIINCAPS2A
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} MIDIINCAPS2A;

typedef struct MIDIINCAPS2W
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} MIDIINCAPS2W;

typedef struct MIDIINCAPSA
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwSupport;
} MIDIINCAPSA;

typedef struct MIDIINCAPSW
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwSupport;
} MIDIINCAPSW;

typedef struct MIDIOUTCAPS2A
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wVoices;
  UINT16       wNotes;
  UINT16       wChannelMask;
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} MIDIOUTCAPS2A;

typedef struct MIDIOUTCAPS2W
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wVoices;
  UINT16       wNotes;
  UINT16       wChannelMask;
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} MIDIOUTCAPS2W;

typedef struct MIDIOUTCAPSA
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wVoices;
  UINT16       wNotes;
  UINT16       wChannelMask;
  UINT32       dwSupport;
} MIDIOUTCAPSA;

typedef struct MIDIOUTCAPSW
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT16       wTechnology;
  UINT16       wVoices;
  UINT16       wNotes;
  UINT16       wChannelMask;
  UINT32       dwSupport;
} MIDIOUTCAPSW;

typedef struct MIDIPROPTEMPO
{
  UINT32 cbStruct;
  UINT32 dwTempo;
} MIDIPROPTEMPO;

typedef struct MIDIPROPTIMEDIV
{
  UINT32 cbStruct;
  UINT32 dwTimeDiv;
} MIDIPROPTIMEDIV;

typedef struct MIDISTRMBUFFVER
{
  UINT32 dwVersion;
  UINT32 dwMid;
  UINT32 dwOEMVersion;
} MIDISTRMBUFFVER;

typedef struct MIXERCAPS2A
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       fdwSupport;
  UINT32       cDestinations;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} MIXERCAPS2A;

typedef struct MIXERCAPS2W
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       fdwSupport;
  UINT32       cDestinations;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} MIXERCAPS2W;

typedef struct MIXERCAPSA
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       fdwSupport;
  UINT32       cDestinations;
} MIXERCAPSA;

typedef struct MIXERCAPSW
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       fdwSupport;
  UINT32       cDestinations;
} MIXERCAPSW;

typedef struct MIXERCONTROLA
{
  UINT32       cbStruct;
  UINT32       dwControlID;
  UINT32       dwControlType;
  UINT32       fdwControl;
  UINT32       cMultipleItems;
  CHAR         szShortName[16];
  CHAR         szName[64];
  struct
  {
    struct
    {
      INT32 lMinimum;
      INT32 lMaximum;
    } Anonymous1;
    struct
    {
      UINT32 dwMinimum;
      UINT32 dwMaximum;
    } Anonymous2;
    UINT32        dwReserved[6];
  } Bounds;
  struct
  {
    UINT32        cSteps;
    UINT32        cbCustomData;
    UINT32        dwReserved[6];
  } Metrics;
} MIXERCONTROLA;

typedef struct MIXERCONTROLDETAILS
{
  UINT32 cbStruct;
  UINT32 dwControlID;
  UINT32 cChannels;
  struct
  {
    HWND   hwndOwner;
    UINT32 cMultipleItems;
  } Anonymous;
  UINT32 cbDetails;
  void*  paDetails;
} MIXERCONTROLDETAILS;

typedef struct MIXERCONTROLDETAILS_BOOLEAN
{
  INT32 fValue;
} MIXERCONTROLDETAILS_BOOLEAN;

typedef struct MIXERCONTROLDETAILS_LISTTEXTA
{
  UINT32       dwParam1;
  UINT32       dwParam2;
  CHAR         szName[64];
} MIXERCONTROLDETAILS_LISTTEXTA;

typedef struct MIXERCONTROLDETAILS_LISTTEXTW
{
  UINT32       dwParam1;
  UINT32       dwParam2;
  CHAR         szName[64];
} MIXERCONTROLDETAILS_LISTTEXTW;

typedef struct MIXERCONTROLDETAILS_SIGNED
{
  INT32 lValue;
} MIXERCONTROLDETAILS_SIGNED;

typedef struct MIXERCONTROLDETAILS_UNSIGNED
{
  UINT32 dwValue;
} MIXERCONTROLDETAILS_UNSIGNED;

typedef struct MIXERCONTROLW
{
  UINT32       cbStruct;
  UINT32       dwControlID;
  UINT32       dwControlType;
  UINT32       fdwControl;
  UINT32       cMultipleItems;
  CHAR         szShortName[16];
  CHAR         szName[64];
  struct
  {
    struct
    {
      INT32 lMinimum;
      INT32 lMaximum;
    } Anonymous1;
    struct
    {
      UINT32 dwMinimum;
      UINT32 dwMaximum;
    } Anonymous2;
    UINT32        dwReserved[6];
  } Bounds;
  struct
  {
    UINT32        cSteps;
    UINT32        cbCustomData;
    UINT32        dwReserved[6];
  } Metrics;
} MIXERCONTROLW;

typedef struct MIXERLINEA
{
  UINT32                  cbStruct;
  UINT32                  dwDestination;
  UINT32                  dwSource;
  UINT32                  dwLineID;
  UINT32                  fdwLine;
  DWORD_PTR               dwUser;
  MIXERLINE_COMPONENTTYPE dwComponentType;
  UINT32                  cChannels;
  UINT32                  cConnections;
  UINT32                  cControls;
  CHAR                    szShortName[16];
  CHAR                    szName[64];
  struct
  {
    UINT32       dwType;
    UINT32       dwDeviceID;
    UINT16       wMid;
    UINT16       wPid;
    UINT32       vDriverVersion;
    CHAR         szPname[32];
  } Target;
} MIXERLINEA;

typedef struct MIXERLINECONTROLSA
{
  UINT32         cbStruct;
  UINT32         dwLineID;
  struct
  {
    UINT32 dwControlID;
    UINT32 dwControlType;
  } Anonymous;
  UINT32         cControls;
  UINT32         cbmxctrl;
  MIXERCONTROLA* pamxctrl;
} MIXERLINECONTROLSA;

typedef struct MIXERLINECONTROLSW
{
  UINT32         cbStruct;
  UINT32         dwLineID;
  struct
  {
    UINT32 dwControlID;
    UINT32 dwControlType;
  } Anonymous;
  UINT32         cControls;
  UINT32         cbmxctrl;
  MIXERCONTROLW* pamxctrl;
} MIXERLINECONTROLSW;

typedef struct MIXERLINEW
{
  UINT32                  cbStruct;
  UINT32                  dwDestination;
  UINT32                  dwSource;
  UINT32                  dwLineID;
  UINT32                  fdwLine;
  DWORD_PTR               dwUser;
  MIXERLINE_COMPONENTTYPE dwComponentType;
  UINT32                  cChannels;
  UINT32                  cConnections;
  UINT32                  cControls;
  CHAR                    szShortName[16];
  CHAR                    szName[64];
  struct
  {
    UINT32       dwType;
    UINT32       dwDeviceID;
    UINT16       wMid;
    UINT16       wPid;
    UINT32       vDriverVersion;
    CHAR         szPname[32];
  } Target;
} MIXERLINEW;

typedef struct MMDeviceEnumerator
{
} MMDeviceEnumerator;

typedef struct PCMWAVEFORMAT
{
  WAVEFORMAT wf;
  UINT16     wBitsPerSample;
} PCMWAVEFORMAT;

typedef struct SpatialAudioClientActivationParams
{
  Guid  tracingContextId;
  Guid  appId;
  INT32 majorVersion;
  INT32 minorVersion1;
  INT32 minorVersion2;
  INT32 minorVersion3;
} SpatialAudioClientActivationParams;

typedef struct SpatialAudioHrtfActivationParams
{
  WAVEFORMATEX*                          ObjectFormat;
  AudioObjectType                        StaticObjectTypeMask;
  UINT32                                 MinDynamicObjectCount;
  UINT32                                 MaxDynamicObjectCount;
  AUDIO_STREAM_CATEGORY                  Category;
  HANDLE                                 EventHandle;
  ISpatialAudioObjectRenderStreamNotify* NotifyObject;
  SpatialAudioHrtfDistanceDecay*         DistanceDecay;
  SpatialAudioHrtfDirectivityUnion*      Directivity;
  SpatialAudioHrtfEnvironmentType*       Environment;
  FLOAT*                                 Orientation;
} SpatialAudioHrtfActivationParams;

typedef struct SpatialAudioHrtfActivationParams2
{
  WAVEFORMATEX*                          ObjectFormat;
  AudioObjectType                        StaticObjectTypeMask;
  UINT32                                 MinDynamicObjectCount;
  UINT32                                 MaxDynamicObjectCount;
  AUDIO_STREAM_CATEGORY                  Category;
  HANDLE                                 EventHandle;
  ISpatialAudioObjectRenderStreamNotify* NotifyObject;
  SpatialAudioHrtfDistanceDecay*         DistanceDecay;
  SpatialAudioHrtfDirectivityUnion*      Directivity;
  SpatialAudioHrtfEnvironmentType*       Environment;
  FLOAT*                                 Orientation;
  SPATIAL_AUDIO_STREAM_OPTIONS           Options;
} SpatialAudioHrtfActivationParams2;

typedef struct SpatialAudioHrtfDirectivity
{
  SpatialAudioHrtfDirectivityType Type;
  FLOAT                           Scaling;
} SpatialAudioHrtfDirectivity;

typedef struct SpatialAudioHrtfDirectivityCardioid
{
  SpatialAudioHrtfDirectivity directivity;
  FLOAT                       Order;
} SpatialAudioHrtfDirectivityCardioid;

typedef struct SpatialAudioHrtfDirectivityCone
{
  SpatialAudioHrtfDirectivity directivity;
  FLOAT                       InnerAngle;
  FLOAT                       OuterAngle;
} SpatialAudioHrtfDirectivityCone;

typedef union SpatialAudioHrtfDirectivityUnion
{
  SpatialAudioHrtfDirectivityCone     Cone;
  SpatialAudioHrtfDirectivityCardioid Cardiod;
  SpatialAudioHrtfDirectivity         Omni;
} SpatialAudioHrtfDirectivityUnion;

typedef struct SpatialAudioHrtfDistanceDecay
{
  SpatialAudioHrtfDistanceDecayType Type;
  FLOAT                             MaxGain;
  FLOAT                             MinGain;
  FLOAT                             UnityGainDistance;
  FLOAT                             CutoffDistance;
} SpatialAudioHrtfDistanceDecay;

typedef struct SpatialAudioMetadataItemsInfo
{
  UINT16 FrameCount;
  UINT16 ItemCount;
  UINT16 MaxItemCount;
  UINT32 MaxValueBufferLength;
} SpatialAudioMetadataItemsInfo;

typedef struct SpatialAudioObjectRenderStreamActivationParams
{
  WAVEFORMATEX*                          ObjectFormat;
  AudioObjectType                        StaticObjectTypeMask;
  UINT32                                 MinDynamicObjectCount;
  UINT32                                 MaxDynamicObjectCount;
  AUDIO_STREAM_CATEGORY                  Category;
  HANDLE                                 EventHandle;
  ISpatialAudioObjectRenderStreamNotify* NotifyObject;
} SpatialAudioObjectRenderStreamActivationParams;

typedef struct SpatialAudioObjectRenderStreamActivationParams2
{
  WAVEFORMATEX*                          ObjectFormat;
  AudioObjectType                        StaticObjectTypeMask;
  UINT32                                 MinDynamicObjectCount;
  UINT32                                 MaxDynamicObjectCount;
  AUDIO_STREAM_CATEGORY                  Category;
  HANDLE                                 EventHandle;
  ISpatialAudioObjectRenderStreamNotify* NotifyObject;
  SPATIAL_AUDIO_STREAM_OPTIONS           Options;
} SpatialAudioObjectRenderStreamActivationParams2;

typedef struct SpatialAudioObjectRenderStreamForMetadataActivationParams
{
  WAVEFORMATEX*                          ObjectFormat;
  AudioObjectType                        StaticObjectTypeMask;
  UINT32                                 MinDynamicObjectCount;
  UINT32                                 MaxDynamicObjectCount;
  AUDIO_STREAM_CATEGORY                  Category;
  HANDLE                                 EventHandle;
  Guid                                   MetadataFormatId;
  UINT16                                 MaxMetadataItemCount;
  PROPVARIANT*                           MetadataActivationParams;
  ISpatialAudioObjectRenderStreamNotify* NotifyObject;
} SpatialAudioObjectRenderStreamForMetadataActivationParams;

typedef struct SpatialAudioObjectRenderStreamForMetadataActivationParams2
{
  WAVEFORMATEX*                          ObjectFormat;
  AudioObjectType                        StaticObjectTypeMask;
  UINT32                                 MinDynamicObjectCount;
  UINT32                                 MaxDynamicObjectCount;
  AUDIO_STREAM_CATEGORY                  Category;
  HANDLE                                 EventHandle;
  Guid                                   MetadataFormatId;
  UINT32                                 MaxMetadataItemCount;
  PROPVARIANT*                           MetadataActivationParams;
  ISpatialAudioObjectRenderStreamNotify* NotifyObject;
  SPATIAL_AUDIO_STREAM_OPTIONS           Options;
} SpatialAudioObjectRenderStreamForMetadataActivationParams2;

typedef struct tACMFORMATDETAILSW
{
  UINT32        cbStruct;
  UINT32        dwFormatIndex;
  UINT32        dwFormatTag;
  UINT32        fdwSupport;
  WAVEFORMATEX* pwfx;
  UINT32        cbwfx;
  CHAR          szFormat[128];
} tACMFORMATDETAILSW;

typedef struct VOLUMEWAVEFILTER
{
  WAVEFILTER wfltr;
  UINT32     dwVolume;
} VOLUMEWAVEFILTER;

typedef struct WAVEFILTER
{
  UINT32        cbStruct;
  UINT32        dwFilterTag;
  UINT32        fdwFilter;
  UINT32        dwReserved[5];
} WAVEFILTER;

typedef struct WAVEFORMAT
{
  UINT16 wFormatTag;
  UINT16 nChannels;
  UINT32 nSamplesPerSec;
  UINT32 nAvgBytesPerSec;
  UINT16 nBlockAlign;
} WAVEFORMAT;

typedef struct WAVEFORMATEX
{
  UINT16 wFormatTag;
  UINT16 nChannels;
  UINT32 nSamplesPerSec;
  UINT32 nAvgBytesPerSec;
  UINT16 nBlockAlign;
  UINT16 wBitsPerSample;
  UINT16 cbSize;
} WAVEFORMATEX;

typedef struct WAVEFORMATEXTENSIBLE
{
  WAVEFORMATEX Format;
  struct
  {
    UINT16 wValidBitsPerSample;
    UINT16 wSamplesPerBlock;
    UINT16 wReserved;
  } Samples;
  UINT32       dwChannelMask;
  Guid         SubFormat;
} WAVEFORMATEXTENSIBLE;

typedef struct WAVEHDR
{
  PSTR      lpData;
  UINT32    dwBufferLength;
  UINT32    dwBytesRecorded;
  DWORD_PTR dwUser;
  UINT32    dwFlags;
  UINT32    dwLoops;
  WAVEHDR*  lpNext;
  DWORD_PTR reserved;
} WAVEHDR;

typedef struct WAVEINCAPS2A
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} WAVEINCAPS2A;

typedef struct WAVEINCAPS2W
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} WAVEINCAPS2W;

typedef struct WAVEINCAPSA
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
} WAVEINCAPSA;

typedef struct WAVEINCAPSW
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
} WAVEINCAPSW;

typedef struct WAVEOUTCAPS2A
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} WAVEOUTCAPS2A;

typedef struct WAVEOUTCAPS2W
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
  UINT32       dwSupport;
  Guid         ManufacturerGuid;
  Guid         ProductGuid;
  Guid         NameGuid;
} WAVEOUTCAPS2W;

typedef struct WAVEOUTCAPSA
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
  UINT32       dwSupport;
} WAVEOUTCAPSA;

typedef struct WAVEOUTCAPSW
{
  UINT16       wMid;
  UINT16       wPid;
  UINT32       vDriverVersion;
  CHAR         szPname[32];
  UINT32       dwFormats;
  UINT16       wChannels;
  UINT16       wReserved1;
  UINT32       dwSupport;
} WAVEOUTCAPSW;

// methods

static inline HRESULT IAcousticEchoCancellationControl_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAcousticEchoCancellationControl*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAcousticEchoCancellationControl_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAcousticEchoCancellationControl*))this_->v->tbl[1](this_)); }
static inline UINT32 IAcousticEchoCancellationControl_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAcousticEchoCancellationControl*))this_->v->tbl[2](this_)); }
static inline HRESULT IAcousticEchoCancellationControl_SetEchoCancellationRenderEndpoint(base_type *this_, const WCHAR* endpointId) { return(((HRESULT (WINAPI*)(IAcousticEchoCancellationControl*, const WCHAR*))this_->v->tbl[3](this_, endpointId)); }

static inline HRESULT IActivateAudioInterfaceAsyncOperation_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IActivateAudioInterfaceAsyncOperation*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IActivateAudioInterfaceAsyncOperation_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IActivateAudioInterfaceAsyncOperation*))this_->v->tbl[1](this_)); }
static inline UINT32 IActivateAudioInterfaceAsyncOperation_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IActivateAudioInterfaceAsyncOperation*))this_->v->tbl[2](this_)); }
static inline HRESULT IActivateAudioInterfaceAsyncOperation_GetActivateResult(base_type *this_, HRESULT* activateResult, IUnknown** activatedInterface) { return(((HRESULT (WINAPI*)(IActivateAudioInterfaceAsyncOperation*, HRESULT*, IUnknown**))this_->v->tbl[3](this_, activateResult, activatedInterface)); }

static inline HRESULT IActivateAudioInterfaceCompletionHandler_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IActivateAudioInterfaceCompletionHandler*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IActivateAudioInterfaceCompletionHandler_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IActivateAudioInterfaceCompletionHandler*))this_->v->tbl[1](this_)); }
static inline UINT32 IActivateAudioInterfaceCompletionHandler_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IActivateAudioInterfaceCompletionHandler*))this_->v->tbl[2](this_)); }
static inline HRESULT IActivateAudioInterfaceCompletionHandler_ActivateCompleted(base_type *this_, IActivateAudioInterfaceAsyncOperation* activateOperation) { return(((HRESULT (WINAPI*)(IActivateAudioInterfaceCompletionHandler*, IActivateAudioInterfaceAsyncOperation*))this_->v->tbl[3](this_, activateOperation)); }

static inline HRESULT IAudioAmbisonicsControl_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioAmbisonicsControl*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioAmbisonicsControl_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioAmbisonicsControl*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioAmbisonicsControl_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioAmbisonicsControl*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioAmbisonicsControl_SetData(base_type *this_, const AMBISONICS_PARAMS* pAmbisonicsParams, UINT32 cbAmbisonicsParams) { return(((HRESULT (WINAPI*)(IAudioAmbisonicsControl*, const AMBISONICS_PARAMS*, UINT32))this_->v->tbl[3](this_, pAmbisonicsParams, cbAmbisonicsParams)); }
static inline HRESULT IAudioAmbisonicsControl_SetHeadTracking(base_type *this_, BOOL bEnableHeadTracking) { return(((HRESULT (WINAPI*)(IAudioAmbisonicsControl*, BOOL))this_->v->tbl[4](this_, bEnableHeadTracking)); }
static inline HRESULT IAudioAmbisonicsControl_GetHeadTracking(base_type *this_, BOOL* pbEnableHeadTracking) { return(((HRESULT (WINAPI*)(IAudioAmbisonicsControl*, BOOL*))this_->v->tbl[5](this_, pbEnableHeadTracking)); }
static inline HRESULT IAudioAmbisonicsControl_SetRotation(base_type *this_, FLOAT X, FLOAT Y, FLOAT Z, FLOAT W) { return(((HRESULT (WINAPI*)(IAudioAmbisonicsControl*, FLOAT, FLOAT, FLOAT, FLOAT))this_->v->tbl[6](this_, X, Y, Z, W)); }

static inline HRESULT IAudioAutoGainControl_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioAutoGainControl*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioAutoGainControl_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioAutoGainControl*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioAutoGainControl_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioAutoGainControl*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioAutoGainControl_GetEnabled(base_type *this_, BOOL* pbEnabled) { return(((HRESULT (WINAPI*)(IAudioAutoGainControl*, BOOL*))this_->v->tbl[3](this_, pbEnabled)); }
static inline HRESULT IAudioAutoGainControl_SetEnabled(base_type *this_, BOOL bEnable, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioAutoGainControl*, BOOL, const Guid*))this_->v->tbl[4](this_, bEnable, pguidEventContext)); }

static inline HRESULT IAudioBass_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioBass*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioBass_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioBass*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioBass_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioBass*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioBass_GetChannelCount(base_type *this_, UINT32* pcChannels) { return(((HRESULT (WINAPI*)(IAudioBass*, UINT32*))this_->v->tbl[3](this_, pcChannels)); }
static inline HRESULT IAudioBass_GetLevelRange(base_type *this_, UINT32 nChannel, FLOAT* pfMinLevelDB, FLOAT* pfMaxLevelDB, FLOAT* pfStepping) { return(((HRESULT (WINAPI*)(IAudioBass*, UINT32, FLOAT*, FLOAT*, FLOAT*))this_->v->tbl[4](this_, nChannel, pfMinLevelDB, pfMaxLevelDB, pfStepping)); }
static inline HRESULT IAudioBass_GetLevel(base_type *this_, UINT32 nChannel, FLOAT* pfLevelDB) { return(((HRESULT (WINAPI*)(IAudioBass*, UINT32, FLOAT*))this_->v->tbl[5](this_, nChannel, pfLevelDB)); }
static inline HRESULT IAudioBass_SetLevel(base_type *this_, UINT32 nChannel, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioBass*, UINT32, FLOAT, const Guid*))this_->v->tbl[6](this_, nChannel, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioBass_SetLevelUniform(base_type *this_, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioBass*, FLOAT, const Guid*))this_->v->tbl[7](this_, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioBass_SetLevelAllChannels(base_type *this_, FLOAT* aLevelsDB, UINT32 cChannels, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioBass*, FLOAT*, UINT32, const Guid*))this_->v->tbl[8](this_, aLevelsDB, cChannels, pguidEventContext)); }

static inline HRESULT IAudioCaptureClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioCaptureClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioCaptureClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioCaptureClient*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioCaptureClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioCaptureClient*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioCaptureClient_GetBuffer(base_type *this_, UINT8** ppData, UINT32* pNumFramesToRead, UINT32* pdwFlags, UINT64* pu64DevicePosition, UINT64* pu64QPCPosition) { return(((HRESULT (WINAPI*)(IAudioCaptureClient*, UINT8**, UINT32*, UINT32*, UINT64*, UINT64*))this_->v->tbl[3](this_, ppData, pNumFramesToRead, pdwFlags, pu64DevicePosition, pu64QPCPosition)); }
static inline HRESULT IAudioCaptureClient_ReleaseBuffer(base_type *this_, UINT32 NumFramesRead) { return(((HRESULT (WINAPI*)(IAudioCaptureClient*, UINT32))this_->v->tbl[4](this_, NumFramesRead)); }
static inline HRESULT IAudioCaptureClient_GetNextPacketSize(base_type *this_, UINT32* pNumFramesInNextPacket) { return(((HRESULT (WINAPI*)(IAudioCaptureClient*, UINT32*))this_->v->tbl[5](this_, pNumFramesInNextPacket)); }

static inline HRESULT IAudioChannelConfig_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioChannelConfig*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioChannelConfig_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioChannelConfig*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioChannelConfig_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioChannelConfig*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioChannelConfig_SetChannelConfig(base_type *this_, UINT32 dwConfig, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioChannelConfig*, UINT32, const Guid*))this_->v->tbl[3](this_, dwConfig, pguidEventContext)); }
static inline HRESULT IAudioChannelConfig_GetChannelConfig(base_type *this_, UINT32* pdwConfig) { return(((HRESULT (WINAPI*)(IAudioChannelConfig*, UINT32*))this_->v->tbl[4](this_, pdwConfig)); }

static inline HRESULT IAudioClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClient*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClient*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioClient_Initialize(base_type *this_, AUDCLNT_SHAREMODE ShareMode, UINT32 StreamFlags, INT64 hnsBufferDuration, INT64 hnsPeriodicity, const WAVEFORMATEX* pFormat, const Guid* AudioSessionGuid) { return(((HRESULT (WINAPI*)(IAudioClient*, AUDCLNT_SHAREMODE, UINT32, INT64, INT64, const WAVEFORMATEX*, const Guid*))this_->v->tbl[3](this_, ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid)); }
static inline HRESULT IAudioClient_GetBufferSize(base_type *this_, UINT32* pNumBufferFrames) { return(((HRESULT (WINAPI*)(IAudioClient*, UINT32*))this_->v->tbl[4](this_, pNumBufferFrames)); }
static inline HRESULT IAudioClient_GetStreamLatency(base_type *this_, INT64* phnsLatency) { return(((HRESULT (WINAPI*)(IAudioClient*, INT64*))this_->v->tbl[5](this_, phnsLatency)); }
static inline HRESULT IAudioClient_GetCurrentPadding(base_type *this_, UINT32* pNumPaddingFrames) { return(((HRESULT (WINAPI*)(IAudioClient*, UINT32*))this_->v->tbl[6](this_, pNumPaddingFrames)); }
static inline HRESULT IAudioClient_IsFormatSupported(base_type *this_, AUDCLNT_SHAREMODE ShareMode, const WAVEFORMATEX* pFormat, WAVEFORMATEX** ppClosestMatch) { return(((HRESULT (WINAPI*)(IAudioClient*, AUDCLNT_SHAREMODE, const WAVEFORMATEX*, WAVEFORMATEX**))this_->v->tbl[7](this_, ShareMode, pFormat, ppClosestMatch)); }
static inline HRESULT IAudioClient_GetMixFormat(base_type *this_, WAVEFORMATEX** ppDeviceFormat) { return(((HRESULT (WINAPI*)(IAudioClient*, WAVEFORMATEX**))this_->v->tbl[8](this_, ppDeviceFormat)); }
static inline HRESULT IAudioClient_GetDevicePeriod(base_type *this_, INT64* phnsDefaultDevicePeriod, INT64* phnsMinimumDevicePeriod) { return(((HRESULT (WINAPI*)(IAudioClient*, INT64*, INT64*))this_->v->tbl[9](this_, phnsDefaultDevicePeriod, phnsMinimumDevicePeriod)); }
static inline HRESULT IAudioClient_Start(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient*))this_->v->tbl[10](this_)); }
static inline HRESULT IAudioClient_Stop(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient*))this_->v->tbl[11](this_)); }
static inline HRESULT IAudioClient_Reset(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient*))this_->v->tbl[12](this_)); }
static inline HRESULT IAudioClient_SetEventHandle(base_type *this_, HANDLE eventHandle) { return(((HRESULT (WINAPI*)(IAudioClient*, HANDLE))this_->v->tbl[13](this_, eventHandle)); }
static inline HRESULT IAudioClient_GetService(base_type *this_, const Guid* riid, void** ppv) { return(((HRESULT (WINAPI*)(IAudioClient*, const Guid*, void**))this_->v->tbl[14](this_, riid, ppv)); }

static inline HRESULT IAudioClient2_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioClient2*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioClient2_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClient2*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioClient2_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClient2*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioClient2_Initialize(base_type *this_, AUDCLNT_SHAREMODE ShareMode, UINT32 StreamFlags, INT64 hnsBufferDuration, INT64 hnsPeriodicity, const WAVEFORMATEX* pFormat, const Guid* AudioSessionGuid) { return(((HRESULT (WINAPI*)(IAudioClient2*, AUDCLNT_SHAREMODE, UINT32, INT64, INT64, const WAVEFORMATEX*, const Guid*))this_->v->tbl[3](this_, ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid)); }
static inline HRESULT IAudioClient2_GetBufferSize(base_type *this_, UINT32* pNumBufferFrames) { return(((HRESULT (WINAPI*)(IAudioClient2*, UINT32*))this_->v->tbl[4](this_, pNumBufferFrames)); }
static inline HRESULT IAudioClient2_GetStreamLatency(base_type *this_, INT64* phnsLatency) { return(((HRESULT (WINAPI*)(IAudioClient2*, INT64*))this_->v->tbl[5](this_, phnsLatency)); }
static inline HRESULT IAudioClient2_GetCurrentPadding(base_type *this_, UINT32* pNumPaddingFrames) { return(((HRESULT (WINAPI*)(IAudioClient2*, UINT32*))this_->v->tbl[6](this_, pNumPaddingFrames)); }
static inline HRESULT IAudioClient2_IsFormatSupported(base_type *this_, AUDCLNT_SHAREMODE ShareMode, const WAVEFORMATEX* pFormat, WAVEFORMATEX** ppClosestMatch) { return(((HRESULT (WINAPI*)(IAudioClient2*, AUDCLNT_SHAREMODE, const WAVEFORMATEX*, WAVEFORMATEX**))this_->v->tbl[7](this_, ShareMode, pFormat, ppClosestMatch)); }
static inline HRESULT IAudioClient2_GetMixFormat(base_type *this_, WAVEFORMATEX** ppDeviceFormat) { return(((HRESULT (WINAPI*)(IAudioClient2*, WAVEFORMATEX**))this_->v->tbl[8](this_, ppDeviceFormat)); }
static inline HRESULT IAudioClient2_GetDevicePeriod(base_type *this_, INT64* phnsDefaultDevicePeriod, INT64* phnsMinimumDevicePeriod) { return(((HRESULT (WINAPI*)(IAudioClient2*, INT64*, INT64*))this_->v->tbl[9](this_, phnsDefaultDevicePeriod, phnsMinimumDevicePeriod)); }
static inline HRESULT IAudioClient2_Start(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient2*))this_->v->tbl[10](this_)); }
static inline HRESULT IAudioClient2_Stop(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient2*))this_->v->tbl[11](this_)); }
static inline HRESULT IAudioClient2_Reset(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient2*))this_->v->tbl[12](this_)); }
static inline HRESULT IAudioClient2_SetEventHandle(base_type *this_, HANDLE eventHandle) { return(((HRESULT (WINAPI*)(IAudioClient2*, HANDLE))this_->v->tbl[13](this_, eventHandle)); }
static inline HRESULT IAudioClient2_GetService(base_type *this_, const Guid* riid, void** ppv) { return(((HRESULT (WINAPI*)(IAudioClient2*, const Guid*, void**))this_->v->tbl[14](this_, riid, ppv)); }
static inline HRESULT IAudioClient2_IsOffloadCapable(base_type *this_, AUDIO_STREAM_CATEGORY Category, BOOL* pbOffloadCapable) { return(((HRESULT (WINAPI*)(IAudioClient2*, AUDIO_STREAM_CATEGORY, BOOL*))this_->v->tbl[15](this_, Category, pbOffloadCapable)); }
static inline HRESULT IAudioClient2_SetClientProperties(base_type *this_, const AudioClientProperties* pProperties) { return(((HRESULT (WINAPI*)(IAudioClient2*, const AudioClientProperties*))this_->v->tbl[16](this_, pProperties)); }
static inline HRESULT IAudioClient2_GetBufferSizeLimits(base_type *this_, const WAVEFORMATEX* pFormat, BOOL bEventDriven, INT64* phnsMinBufferDuration, INT64* phnsMaxBufferDuration) { return(((HRESULT (WINAPI*)(IAudioClient2*, const WAVEFORMATEX*, BOOL, INT64*, INT64*))this_->v->tbl[17](this_, pFormat, bEventDriven, phnsMinBufferDuration, phnsMaxBufferDuration)); }

static inline HRESULT IAudioClient3_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioClient3*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioClient3_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClient3*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioClient3_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClient3*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioClient3_Initialize(base_type *this_, AUDCLNT_SHAREMODE ShareMode, UINT32 StreamFlags, INT64 hnsBufferDuration, INT64 hnsPeriodicity, const WAVEFORMATEX* pFormat, const Guid* AudioSessionGuid) { return(((HRESULT (WINAPI*)(IAudioClient3*, AUDCLNT_SHAREMODE, UINT32, INT64, INT64, const WAVEFORMATEX*, const Guid*))this_->v->tbl[3](this_, ShareMode, StreamFlags, hnsBufferDuration, hnsPeriodicity, pFormat, AudioSessionGuid)); }
static inline HRESULT IAudioClient3_GetBufferSize(base_type *this_, UINT32* pNumBufferFrames) { return(((HRESULT (WINAPI*)(IAudioClient3*, UINT32*))this_->v->tbl[4](this_, pNumBufferFrames)); }
static inline HRESULT IAudioClient3_GetStreamLatency(base_type *this_, INT64* phnsLatency) { return(((HRESULT (WINAPI*)(IAudioClient3*, INT64*))this_->v->tbl[5](this_, phnsLatency)); }
static inline HRESULT IAudioClient3_GetCurrentPadding(base_type *this_, UINT32* pNumPaddingFrames) { return(((HRESULT (WINAPI*)(IAudioClient3*, UINT32*))this_->v->tbl[6](this_, pNumPaddingFrames)); }
static inline HRESULT IAudioClient3_IsFormatSupported(base_type *this_, AUDCLNT_SHAREMODE ShareMode, const WAVEFORMATEX* pFormat, WAVEFORMATEX** ppClosestMatch) { return(((HRESULT (WINAPI*)(IAudioClient3*, AUDCLNT_SHAREMODE, const WAVEFORMATEX*, WAVEFORMATEX**))this_->v->tbl[7](this_, ShareMode, pFormat, ppClosestMatch)); }
static inline HRESULT IAudioClient3_GetMixFormat(base_type *this_, WAVEFORMATEX** ppDeviceFormat) { return(((HRESULT (WINAPI*)(IAudioClient3*, WAVEFORMATEX**))this_->v->tbl[8](this_, ppDeviceFormat)); }
static inline HRESULT IAudioClient3_GetDevicePeriod(base_type *this_, INT64* phnsDefaultDevicePeriod, INT64* phnsMinimumDevicePeriod) { return(((HRESULT (WINAPI*)(IAudioClient3*, INT64*, INT64*))this_->v->tbl[9](this_, phnsDefaultDevicePeriod, phnsMinimumDevicePeriod)); }
static inline HRESULT IAudioClient3_Start(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient3*))this_->v->tbl[10](this_)); }
static inline HRESULT IAudioClient3_Stop(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient3*))this_->v->tbl[11](this_)); }
static inline HRESULT IAudioClient3_Reset(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioClient3*))this_->v->tbl[12](this_)); }
static inline HRESULT IAudioClient3_SetEventHandle(base_type *this_, HANDLE eventHandle) { return(((HRESULT (WINAPI*)(IAudioClient3*, HANDLE))this_->v->tbl[13](this_, eventHandle)); }
static inline HRESULT IAudioClient3_GetService(base_type *this_, const Guid* riid, void** ppv) { return(((HRESULT (WINAPI*)(IAudioClient3*, const Guid*, void**))this_->v->tbl[14](this_, riid, ppv)); }
static inline HRESULT IAudioClient3_IsOffloadCapable(base_type *this_, AUDIO_STREAM_CATEGORY Category, BOOL* pbOffloadCapable) { return(((HRESULT (WINAPI*)(IAudioClient3*, AUDIO_STREAM_CATEGORY, BOOL*))this_->v->tbl[15](this_, Category, pbOffloadCapable)); }
static inline HRESULT IAudioClient3_SetClientProperties(base_type *this_, const AudioClientProperties* pProperties) { return(((HRESULT (WINAPI*)(IAudioClient3*, const AudioClientProperties*))this_->v->tbl[16](this_, pProperties)); }
static inline HRESULT IAudioClient3_GetBufferSizeLimits(base_type *this_, const WAVEFORMATEX* pFormat, BOOL bEventDriven, INT64* phnsMinBufferDuration, INT64* phnsMaxBufferDuration) { return(((HRESULT (WINAPI*)(IAudioClient3*, const WAVEFORMATEX*, BOOL, INT64*, INT64*))this_->v->tbl[17](this_, pFormat, bEventDriven, phnsMinBufferDuration, phnsMaxBufferDuration)); }
static inline HRESULT IAudioClient3_GetSharedModeEnginePeriod(base_type *this_, const WAVEFORMATEX* pFormat, UINT32* pDefaultPeriodInFrames, UINT32* pFundamentalPeriodInFrames, UINT32* pMinPeriodInFrames, UINT32* pMaxPeriodInFrames) { return(((HRESULT (WINAPI*)(IAudioClient3*, const WAVEFORMATEX*, UINT32*, UINT32*, UINT32*, UINT32*))this_->v->tbl[18](this_, pFormat, pDefaultPeriodInFrames, pFundamentalPeriodInFrames, pMinPeriodInFrames, pMaxPeriodInFrames)); }
static inline HRESULT IAudioClient3_GetCurrentSharedModeEnginePeriod(base_type *this_, WAVEFORMATEX** ppFormat, UINT32* pCurrentPeriodInFrames) { return(((HRESULT (WINAPI*)(IAudioClient3*, WAVEFORMATEX**, UINT32*))this_->v->tbl[19](this_, ppFormat, pCurrentPeriodInFrames)); }
static inline HRESULT IAudioClient3_InitializeSharedAudioStream(base_type *this_, UINT32 StreamFlags, UINT32 PeriodInFrames, const WAVEFORMATEX* pFormat, const Guid* AudioSessionGuid) { return(((HRESULT (WINAPI*)(IAudioClient3*, UINT32, UINT32, const WAVEFORMATEX*, const Guid*))this_->v->tbl[20](this_, StreamFlags, PeriodInFrames, pFormat, AudioSessionGuid)); }

static inline HRESULT IAudioClientDuckingControl_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioClientDuckingControl*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioClientDuckingControl_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClientDuckingControl*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioClientDuckingControl_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClientDuckingControl*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioClientDuckingControl_SetDuckingOptionsForCurrentStream(base_type *this_, AUDIO_DUCKING_OPTIONS options) { return(((HRESULT (WINAPI*)(IAudioClientDuckingControl*, AUDIO_DUCKING_OPTIONS))this_->v->tbl[3](this_, options)); }

static inline HRESULT IAudioClock_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioClock*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioClock_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClock*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioClock_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClock*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioClock_GetFrequency(base_type *this_, UINT64* pu64Frequency) { return(((HRESULT (WINAPI*)(IAudioClock*, UINT64*))this_->v->tbl[3](this_, pu64Frequency)); }
static inline HRESULT IAudioClock_GetPosition(base_type *this_, UINT64* pu64Position, UINT64* pu64QPCPosition) { return(((HRESULT (WINAPI*)(IAudioClock*, UINT64*, UINT64*))this_->v->tbl[4](this_, pu64Position, pu64QPCPosition)); }
static inline HRESULT IAudioClock_GetCharacteristics(base_type *this_, UINT32* pdwCharacteristics) { return(((HRESULT (WINAPI*)(IAudioClock*, UINT32*))this_->v->tbl[5](this_, pdwCharacteristics)); }

static inline HRESULT IAudioClock2_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioClock2*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioClock2_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClock2*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioClock2_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClock2*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioClock2_GetDevicePosition(base_type *this_, UINT64* DevicePosition, UINT64* QPCPosition) { return(((HRESULT (WINAPI*)(IAudioClock2*, UINT64*, UINT64*))this_->v->tbl[3](this_, DevicePosition, QPCPosition)); }

static inline HRESULT IAudioClockAdjustment_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioClockAdjustment*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioClockAdjustment_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClockAdjustment*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioClockAdjustment_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioClockAdjustment*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioClockAdjustment_SetSampleRate(base_type *this_, FLOAT flSampleRate) { return(((HRESULT (WINAPI*)(IAudioClockAdjustment*, FLOAT))this_->v->tbl[3](this_, flSampleRate)); }

static inline HRESULT IAudioEffectsChangedNotificationClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioEffectsChangedNotificationClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioEffectsChangedNotificationClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioEffectsChangedNotificationClient*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioEffectsChangedNotificationClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioEffectsChangedNotificationClient*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioEffectsChangedNotificationClient_OnAudioEffectsChanged(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioEffectsChangedNotificationClient*))this_->v->tbl[3](this_)); }

static inline HRESULT IAudioEffectsManager_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioEffectsManager*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioEffectsManager_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioEffectsManager*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioEffectsManager_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioEffectsManager*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioEffectsManager_RegisterAudioEffectsChangedNotificationCallback(base_type *this_, IAudioEffectsChangedNotificationClient* client) { return(((HRESULT (WINAPI*)(IAudioEffectsManager*, IAudioEffectsChangedNotificationClient*))this_->v->tbl[3](this_, client)); }
static inline HRESULT IAudioEffectsManager_UnregisterAudioEffectsChangedNotificationCallback(base_type *this_, IAudioEffectsChangedNotificationClient* client) { return(((HRESULT (WINAPI*)(IAudioEffectsManager*, IAudioEffectsChangedNotificationClient*))this_->v->tbl[4](this_, client)); }
static inline HRESULT IAudioEffectsManager_GetAudioEffects(base_type *this_, AUDIO_EFFECT** effects, UINT32* numEffects) { return(((HRESULT (WINAPI*)(IAudioEffectsManager*, AUDIO_EFFECT**, UINT32*))this_->v->tbl[5](this_, effects, numEffects)); }
static inline HRESULT IAudioEffectsManager_SetAudioEffectState(base_type *this_, Guid effectId, AUDIO_EFFECT_STATE state) { return(((HRESULT (WINAPI*)(IAudioEffectsManager*, Guid, AUDIO_EFFECT_STATE))this_->v->tbl[6](this_, effectId, state)); }

static inline HRESULT IAudioFormatEnumerator_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioFormatEnumerator*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioFormatEnumerator_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioFormatEnumerator*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioFormatEnumerator_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioFormatEnumerator*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioFormatEnumerator_GetCount(base_type *this_, UINT32* count) { return(((HRESULT (WINAPI*)(IAudioFormatEnumerator*, UINT32*))this_->v->tbl[3](this_, count)); }
static inline HRESULT IAudioFormatEnumerator_GetFormat(base_type *this_, UINT32 index, WAVEFORMATEX** format) { return(((HRESULT (WINAPI*)(IAudioFormatEnumerator*, UINT32, WAVEFORMATEX**))this_->v->tbl[4](this_, index, format)); }

static inline HRESULT IAudioInputSelector_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioInputSelector*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioInputSelector_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioInputSelector*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioInputSelector_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioInputSelector*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioInputSelector_GetSelection(base_type *this_, UINT32* pnIdSelected) { return(((HRESULT (WINAPI*)(IAudioInputSelector*, UINT32*))this_->v->tbl[3](this_, pnIdSelected)); }
static inline HRESULT IAudioInputSelector_SetSelection(base_type *this_, UINT32 nIdSelect, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioInputSelector*, UINT32, const Guid*))this_->v->tbl[4](this_, nIdSelect, pguidEventContext)); }

static inline HRESULT IAudioLoudness_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioLoudness*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioLoudness_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioLoudness*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioLoudness_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioLoudness*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioLoudness_GetEnabled(base_type *this_, BOOL* pbEnabled) { return(((HRESULT (WINAPI*)(IAudioLoudness*, BOOL*))this_->v->tbl[3](this_, pbEnabled)); }
static inline HRESULT IAudioLoudness_SetEnabled(base_type *this_, BOOL bEnable, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioLoudness*, BOOL, const Guid*))this_->v->tbl[4](this_, bEnable, pguidEventContext)); }

static inline HRESULT IAudioMidrange_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioMidrange*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioMidrange_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioMidrange*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioMidrange_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioMidrange*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioMidrange_GetChannelCount(base_type *this_, UINT32* pcChannels) { return(((HRESULT (WINAPI*)(IAudioMidrange*, UINT32*))this_->v->tbl[3](this_, pcChannels)); }
static inline HRESULT IAudioMidrange_GetLevelRange(base_type *this_, UINT32 nChannel, FLOAT* pfMinLevelDB, FLOAT* pfMaxLevelDB, FLOAT* pfStepping) { return(((HRESULT (WINAPI*)(IAudioMidrange*, UINT32, FLOAT*, FLOAT*, FLOAT*))this_->v->tbl[4](this_, nChannel, pfMinLevelDB, pfMaxLevelDB, pfStepping)); }
static inline HRESULT IAudioMidrange_GetLevel(base_type *this_, UINT32 nChannel, FLOAT* pfLevelDB) { return(((HRESULT (WINAPI*)(IAudioMidrange*, UINT32, FLOAT*))this_->v->tbl[5](this_, nChannel, pfLevelDB)); }
static inline HRESULT IAudioMidrange_SetLevel(base_type *this_, UINT32 nChannel, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioMidrange*, UINT32, FLOAT, const Guid*))this_->v->tbl[6](this_, nChannel, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioMidrange_SetLevelUniform(base_type *this_, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioMidrange*, FLOAT, const Guid*))this_->v->tbl[7](this_, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioMidrange_SetLevelAllChannels(base_type *this_, FLOAT* aLevelsDB, UINT32 cChannels, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioMidrange*, FLOAT*, UINT32, const Guid*))this_->v->tbl[8](this_, aLevelsDB, cChannels, pguidEventContext)); }

static inline HRESULT IAudioMute_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioMute*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioMute_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioMute*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioMute_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioMute*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioMute_SetMute(base_type *this_, BOOL bMuted, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioMute*, BOOL, const Guid*))this_->v->tbl[3](this_, bMuted, pguidEventContext)); }
static inline HRESULT IAudioMute_GetMute(base_type *this_, BOOL* pbMuted) { return(((HRESULT (WINAPI*)(IAudioMute*, BOOL*))this_->v->tbl[4](this_, pbMuted)); }

static inline HRESULT IAudioOutputSelector_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioOutputSelector*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioOutputSelector_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioOutputSelector*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioOutputSelector_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioOutputSelector*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioOutputSelector_GetSelection(base_type *this_, UINT32* pnIdSelected) { return(((HRESULT (WINAPI*)(IAudioOutputSelector*, UINT32*))this_->v->tbl[3](this_, pnIdSelected)); }
static inline HRESULT IAudioOutputSelector_SetSelection(base_type *this_, UINT32 nIdSelect, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioOutputSelector*, UINT32, const Guid*))this_->v->tbl[4](this_, nIdSelect, pguidEventContext)); }

static inline HRESULT IAudioPeakMeter_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioPeakMeter*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioPeakMeter_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioPeakMeter*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioPeakMeter_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioPeakMeter*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioPeakMeter_GetChannelCount(base_type *this_, UINT32* pcChannels) { return(((HRESULT (WINAPI*)(IAudioPeakMeter*, UINT32*))this_->v->tbl[3](this_, pcChannels)); }
static inline HRESULT IAudioPeakMeter_GetLevel(base_type *this_, UINT32 nChannel, FLOAT* pfLevel) { return(((HRESULT (WINAPI*)(IAudioPeakMeter*, UINT32, FLOAT*))this_->v->tbl[4](this_, nChannel, pfLevel)); }

static inline HRESULT IAudioRenderClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioRenderClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioRenderClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioRenderClient*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioRenderClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioRenderClient*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioRenderClient_GetBuffer(base_type *this_, UINT32 NumFramesRequested, UINT8** ppData) { return(((HRESULT (WINAPI*)(IAudioRenderClient*, UINT32, UINT8**))this_->v->tbl[3](this_, NumFramesRequested, ppData)); }
static inline HRESULT IAudioRenderClient_ReleaseBuffer(base_type *this_, UINT32 NumFramesWritten, UINT32 dwFlags) { return(((HRESULT (WINAPI*)(IAudioRenderClient*, UINT32, UINT32))this_->v->tbl[4](this_, NumFramesWritten, dwFlags)); }

static inline HRESULT IAudioSessionControl_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSessionControl_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionControl*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSessionControl_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionControl*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSessionControl_GetState(base_type *this_, AudioSessionState* pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, AudioSessionState*))this_->v->tbl[3](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl_GetDisplayName(base_type *this_, WCHAR** pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, WCHAR**))this_->v->tbl[4](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl_SetDisplayName(base_type *this_, const WCHAR* Value, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, const WCHAR*, const Guid*))this_->v->tbl[5](this_, Value, EventContext)); }
static inline HRESULT IAudioSessionControl_GetIconPath(base_type *this_, WCHAR** pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, WCHAR**))this_->v->tbl[6](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl_SetIconPath(base_type *this_, const WCHAR* Value, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, const WCHAR*, const Guid*))this_->v->tbl[7](this_, Value, EventContext)); }
static inline HRESULT IAudioSessionControl_GetGroupingParam(base_type *this_, Guid* pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, Guid*))this_->v->tbl[8](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl_SetGroupingParam(base_type *this_, const Guid* Override, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, const Guid*, const Guid*))this_->v->tbl[9](this_, Override, EventContext)); }
static inline HRESULT IAudioSessionControl_RegisterAudioSessionNotification(base_type *this_, IAudioSessionEvents* NewNotifications) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, IAudioSessionEvents*))this_->v->tbl[10](this_, NewNotifications)); }
static inline HRESULT IAudioSessionControl_UnregisterAudioSessionNotification(base_type *this_, IAudioSessionEvents* NewNotifications) { return(((HRESULT (WINAPI*)(IAudioSessionControl*, IAudioSessionEvents*))this_->v->tbl[11](this_, NewNotifications)); }

static inline HRESULT IAudioSessionControl2_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSessionControl2_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionControl2*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSessionControl2_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionControl2*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSessionControl2_GetState(base_type *this_, AudioSessionState* pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, AudioSessionState*))this_->v->tbl[3](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl2_GetDisplayName(base_type *this_, WCHAR** pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, WCHAR**))this_->v->tbl[4](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl2_SetDisplayName(base_type *this_, const WCHAR* Value, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, const WCHAR*, const Guid*))this_->v->tbl[5](this_, Value, EventContext)); }
static inline HRESULT IAudioSessionControl2_GetIconPath(base_type *this_, WCHAR** pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, WCHAR**))this_->v->tbl[6](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl2_SetIconPath(base_type *this_, const WCHAR* Value, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, const WCHAR*, const Guid*))this_->v->tbl[7](this_, Value, EventContext)); }
static inline HRESULT IAudioSessionControl2_GetGroupingParam(base_type *this_, Guid* pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, Guid*))this_->v->tbl[8](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl2_SetGroupingParam(base_type *this_, const Guid* Override, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, const Guid*, const Guid*))this_->v->tbl[9](this_, Override, EventContext)); }
static inline HRESULT IAudioSessionControl2_RegisterAudioSessionNotification(base_type *this_, IAudioSessionEvents* NewNotifications) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, IAudioSessionEvents*))this_->v->tbl[10](this_, NewNotifications)); }
static inline HRESULT IAudioSessionControl2_UnregisterAudioSessionNotification(base_type *this_, IAudioSessionEvents* NewNotifications) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, IAudioSessionEvents*))this_->v->tbl[11](this_, NewNotifications)); }
static inline HRESULT IAudioSessionControl2_GetSessionIdentifier(base_type *this_, WCHAR** pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, WCHAR**))this_->v->tbl[12](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl2_GetSessionInstanceIdentifier(base_type *this_, WCHAR** pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, WCHAR**))this_->v->tbl[13](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl2_GetProcessId(base_type *this_, UINT32* pRetVal) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, UINT32*))this_->v->tbl[14](this_, pRetVal)); }
static inline HRESULT IAudioSessionControl2_IsSystemSoundsSession(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*))this_->v->tbl[15](this_)); }
static inline HRESULT IAudioSessionControl2_SetDuckingPreference(base_type *this_, BOOL optOut) { return(((HRESULT (WINAPI*)(IAudioSessionControl2*, BOOL))this_->v->tbl[16](this_, optOut)); }

static inline HRESULT IAudioSessionEnumerator_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSessionEnumerator*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSessionEnumerator_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionEnumerator*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSessionEnumerator_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionEnumerator*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSessionEnumerator_GetCount(base_type *this_, INT32* SessionCount) { return(((HRESULT (WINAPI*)(IAudioSessionEnumerator*, INT32*))this_->v->tbl[3](this_, SessionCount)); }
static inline HRESULT IAudioSessionEnumerator_GetSession(base_type *this_, INT32 SessionCount, IAudioSessionControl** Session) { return(((HRESULT (WINAPI*)(IAudioSessionEnumerator*, INT32, IAudioSessionControl**))this_->v->tbl[4](this_, SessionCount, Session)); }

static inline HRESULT IAudioSessionEvents_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSessionEvents_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionEvents*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSessionEvents_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionEvents*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSessionEvents_OnDisplayNameChanged(base_type *this_, const WCHAR* NewDisplayName, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, const WCHAR*, const Guid*))this_->v->tbl[3](this_, NewDisplayName, EventContext)); }
static inline HRESULT IAudioSessionEvents_OnIconPathChanged(base_type *this_, const WCHAR* NewIconPath, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, const WCHAR*, const Guid*))this_->v->tbl[4](this_, NewIconPath, EventContext)); }
static inline HRESULT IAudioSessionEvents_OnSimpleVolumeChanged(base_type *this_, FLOAT NewVolume, BOOL NewMute, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, FLOAT, BOOL, const Guid*))this_->v->tbl[5](this_, NewVolume, NewMute, EventContext)); }
static inline HRESULT IAudioSessionEvents_OnChannelVolumeChanged(base_type *this_, UINT32 ChannelCount, FLOAT* NewChannelVolumeArray, UINT32 ChangedChannel, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, UINT32, FLOAT*, UINT32, const Guid*))this_->v->tbl[6](this_, ChannelCount, NewChannelVolumeArray, ChangedChannel, EventContext)); }
static inline HRESULT IAudioSessionEvents_OnGroupingParamChanged(base_type *this_, const Guid* NewGroupingParam, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, const Guid*, const Guid*))this_->v->tbl[7](this_, NewGroupingParam, EventContext)); }
static inline HRESULT IAudioSessionEvents_OnStateChanged(base_type *this_, AudioSessionState NewState) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, AudioSessionState))this_->v->tbl[8](this_, NewState)); }
static inline HRESULT IAudioSessionEvents_OnSessionDisconnected(base_type *this_, AudioSessionDisconnectReason DisconnectReason) { return(((HRESULT (WINAPI*)(IAudioSessionEvents*, AudioSessionDisconnectReason))this_->v->tbl[9](this_, DisconnectReason)); }

static inline HRESULT IAudioSessionManager_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSessionManager*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSessionManager_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionManager*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSessionManager_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionManager*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSessionManager_GetAudioSessionControl(base_type *this_, const Guid* AudioSessionGuid, UINT32 StreamFlags, IAudioSessionControl** SessionControl) { return(((HRESULT (WINAPI*)(IAudioSessionManager*, const Guid*, UINT32, IAudioSessionControl**))this_->v->tbl[3](this_, AudioSessionGuid, StreamFlags, SessionControl)); }
static inline HRESULT IAudioSessionManager_GetSimpleAudioVolume(base_type *this_, const Guid* AudioSessionGuid, UINT32 StreamFlags, ISimpleAudioVolume** AudioVolume) { return(((HRESULT (WINAPI*)(IAudioSessionManager*, const Guid*, UINT32, ISimpleAudioVolume**))this_->v->tbl[4](this_, AudioSessionGuid, StreamFlags, AudioVolume)); }

static inline HRESULT IAudioSessionManager2_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSessionManager2_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionManager2*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSessionManager2_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionManager2*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSessionManager2_GetAudioSessionControl(base_type *this_, const Guid* AudioSessionGuid, UINT32 StreamFlags, IAudioSessionControl** SessionControl) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, const Guid*, UINT32, IAudioSessionControl**))this_->v->tbl[3](this_, AudioSessionGuid, StreamFlags, SessionControl)); }
static inline HRESULT IAudioSessionManager2_GetSimpleAudioVolume(base_type *this_, const Guid* AudioSessionGuid, UINT32 StreamFlags, ISimpleAudioVolume** AudioVolume) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, const Guid*, UINT32, ISimpleAudioVolume**))this_->v->tbl[4](this_, AudioSessionGuid, StreamFlags, AudioVolume)); }
static inline HRESULT IAudioSessionManager2_GetSessionEnumerator(base_type *this_, IAudioSessionEnumerator** SessionEnum) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, IAudioSessionEnumerator**))this_->v->tbl[5](this_, SessionEnum)); }
static inline HRESULT IAudioSessionManager2_RegisterSessionNotification(base_type *this_, IAudioSessionNotification* SessionNotification) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, IAudioSessionNotification*))this_->v->tbl[6](this_, SessionNotification)); }
static inline HRESULT IAudioSessionManager2_UnregisterSessionNotification(base_type *this_, IAudioSessionNotification* SessionNotification) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, IAudioSessionNotification*))this_->v->tbl[7](this_, SessionNotification)); }
static inline HRESULT IAudioSessionManager2_RegisterDuckNotification(base_type *this_, const WCHAR* sessionID, IAudioVolumeDuckNotification* duckNotification) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, const WCHAR*, IAudioVolumeDuckNotification*))this_->v->tbl[8](this_, sessionID, duckNotification)); }
static inline HRESULT IAudioSessionManager2_UnregisterDuckNotification(base_type *this_, IAudioVolumeDuckNotification* duckNotification) { return(((HRESULT (WINAPI*)(IAudioSessionManager2*, IAudioVolumeDuckNotification*))this_->v->tbl[9](this_, duckNotification)); }

static inline HRESULT IAudioSessionNotification_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSessionNotification*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSessionNotification_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionNotification*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSessionNotification_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSessionNotification*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSessionNotification_OnSessionCreated(base_type *this_, IAudioSessionControl* NewSession) { return(((HRESULT (WINAPI*)(IAudioSessionNotification*, IAudioSessionControl*))this_->v->tbl[3](this_, NewSession)); }

static inline HRESULT IAudioStateMonitor_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioStateMonitor*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioStateMonitor_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioStateMonitor*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioStateMonitor_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioStateMonitor*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioStateMonitor_RegisterCallback(base_type *this_, PAudioStateMonitorCallback callback, void* context, INT64* registration) { return(((HRESULT (WINAPI*)(IAudioStateMonitor*, PAudioStateMonitorCallback, void*, INT64*))this_->v->tbl[3](this_, callback, context, registration)); }
static inline void IAudioStateMonitor_UnregisterCallback(base_type *this_, INT64 registration) { ((void (WINAPI*)(IAudioStateMonitor*, INT64))this_->v->tbl[4](this_, registration)); }
static inline AudioStateMonitorSoundLevel IAudioStateMonitor_GetSoundLevel(base_type *this_) { return(((AudioStateMonitorSoundLevel (WINAPI*)(IAudioStateMonitor*))this_->v->tbl[5](this_)); }

static inline HRESULT IAudioStreamVolume_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioStreamVolume*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioStreamVolume_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioStreamVolume*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioStreamVolume_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioStreamVolume*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioStreamVolume_GetChannelCount(base_type *this_, UINT32* pdwCount) { return(((HRESULT (WINAPI*)(IAudioStreamVolume*, UINT32*))this_->v->tbl[3](this_, pdwCount)); }
static inline HRESULT IAudioStreamVolume_SetChannelVolume(base_type *this_, UINT32 dwIndex, const FLOAT fLevel) { return(((HRESULT (WINAPI*)(IAudioStreamVolume*, UINT32, const FLOAT))this_->v->tbl[4](this_, dwIndex, fLevel)); }
static inline HRESULT IAudioStreamVolume_GetChannelVolume(base_type *this_, UINT32 dwIndex, FLOAT* pfLevel) { return(((HRESULT (WINAPI*)(IAudioStreamVolume*, UINT32, FLOAT*))this_->v->tbl[5](this_, dwIndex, pfLevel)); }
static inline HRESULT IAudioStreamVolume_SetAllVolumes(base_type *this_, UINT32 dwCount, const FLOAT* pfVolumes) { return(((HRESULT (WINAPI*)(IAudioStreamVolume*, UINT32, const FLOAT*))this_->v->tbl[6](this_, dwCount, pfVolumes)); }
static inline HRESULT IAudioStreamVolume_GetAllVolumes(base_type *this_, UINT32 dwCount, FLOAT* pfVolumes) { return(((HRESULT (WINAPI*)(IAudioStreamVolume*, UINT32, FLOAT*))this_->v->tbl[7](this_, dwCount, pfVolumes)); }

static inline HRESULT IAudioSystemEffectsPropertyChangeNotificationClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyChangeNotificationClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSystemEffectsPropertyChangeNotificationClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSystemEffectsPropertyChangeNotificationClient*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSystemEffectsPropertyChangeNotificationClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSystemEffectsPropertyChangeNotificationClient*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSystemEffectsPropertyChangeNotificationClient_OnPropertyChanged(base_type *this_, AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE type, const PROPERTYKEY key) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyChangeNotificationClient*, AUDIO_SYSTEMEFFECTS_PROPERTYSTORE_TYPE, const PROPERTYKEY))this_->v->tbl[3](this_, type, key)); }

static inline HRESULT IAudioSystemEffectsPropertyStore_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioSystemEffectsPropertyStore_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSystemEffectsPropertyStore*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioSystemEffectsPropertyStore_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioSystemEffectsPropertyStore*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioSystemEffectsPropertyStore_OpenDefaultPropertyStore(base_type *this_, UINT32 stgmAccess, IPropertyStore** propStore) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*, UINT32, IPropertyStore**))this_->v->tbl[3](this_, stgmAccess, propStore)); }
static inline HRESULT IAudioSystemEffectsPropertyStore_OpenUserPropertyStore(base_type *this_, UINT32 stgmAccess, IPropertyStore** propStore) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*, UINT32, IPropertyStore**))this_->v->tbl[4](this_, stgmAccess, propStore)); }
static inline HRESULT IAudioSystemEffectsPropertyStore_OpenVolatilePropertyStore(base_type *this_, UINT32 stgmAccess, IPropertyStore** propStore) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*, UINT32, IPropertyStore**))this_->v->tbl[5](this_, stgmAccess, propStore)); }
static inline HRESULT IAudioSystemEffectsPropertyStore_ResetUserPropertyStore(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*))this_->v->tbl[6](this_)); }
static inline HRESULT IAudioSystemEffectsPropertyStore_ResetVolatilePropertyStore(base_type *this_) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*))this_->v->tbl[7](this_)); }
static inline HRESULT IAudioSystemEffectsPropertyStore_RegisterPropertyChangeNotification(base_type *this_, IAudioSystemEffectsPropertyChangeNotificationClient* callback) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*, IAudioSystemEffectsPropertyChangeNotificationClient*))this_->v->tbl[8](this_, callback)); }
static inline HRESULT IAudioSystemEffectsPropertyStore_UnregisterPropertyChangeNotification(base_type *this_, IAudioSystemEffectsPropertyChangeNotificationClient* callback) { return(((HRESULT (WINAPI*)(IAudioSystemEffectsPropertyStore*, IAudioSystemEffectsPropertyChangeNotificationClient*))this_->v->tbl[9](this_, callback)); }

static inline HRESULT IAudioTreble_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioTreble*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioTreble_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioTreble*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioTreble_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioTreble*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioTreble_GetChannelCount(base_type *this_, UINT32* pcChannels) { return(((HRESULT (WINAPI*)(IAudioTreble*, UINT32*))this_->v->tbl[3](this_, pcChannels)); }
static inline HRESULT IAudioTreble_GetLevelRange(base_type *this_, UINT32 nChannel, FLOAT* pfMinLevelDB, FLOAT* pfMaxLevelDB, FLOAT* pfStepping) { return(((HRESULT (WINAPI*)(IAudioTreble*, UINT32, FLOAT*, FLOAT*, FLOAT*))this_->v->tbl[4](this_, nChannel, pfMinLevelDB, pfMaxLevelDB, pfStepping)); }
static inline HRESULT IAudioTreble_GetLevel(base_type *this_, UINT32 nChannel, FLOAT* pfLevelDB) { return(((HRESULT (WINAPI*)(IAudioTreble*, UINT32, FLOAT*))this_->v->tbl[5](this_, nChannel, pfLevelDB)); }
static inline HRESULT IAudioTreble_SetLevel(base_type *this_, UINT32 nChannel, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioTreble*, UINT32, FLOAT, const Guid*))this_->v->tbl[6](this_, nChannel, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioTreble_SetLevelUniform(base_type *this_, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioTreble*, FLOAT, const Guid*))this_->v->tbl[7](this_, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioTreble_SetLevelAllChannels(base_type *this_, FLOAT* aLevelsDB, UINT32 cChannels, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioTreble*, FLOAT*, UINT32, const Guid*))this_->v->tbl[8](this_, aLevelsDB, cChannels, pguidEventContext)); }

static inline HRESULT IAudioViewManagerService_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioViewManagerService*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioViewManagerService_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioViewManagerService*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioViewManagerService_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioViewManagerService*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioViewManagerService_SetAudioStreamWindow(base_type *this_, HWND hwnd) { return(((HRESULT (WINAPI*)(IAudioViewManagerService*, HWND))this_->v->tbl[3](this_, hwnd)); }

static inline HRESULT IAudioVolumeDuckNotification_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioVolumeDuckNotification*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioVolumeDuckNotification_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioVolumeDuckNotification*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioVolumeDuckNotification_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioVolumeDuckNotification*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioVolumeDuckNotification_OnVolumeDuckNotification(base_type *this_, const WCHAR* sessionID, UINT32 countCommunicationSessions) { return(((HRESULT (WINAPI*)(IAudioVolumeDuckNotification*, const WCHAR*, UINT32))this_->v->tbl[3](this_, sessionID, countCommunicationSessions)); }
static inline HRESULT IAudioVolumeDuckNotification_OnVolumeUnduckNotification(base_type *this_, const WCHAR* sessionID) { return(((HRESULT (WINAPI*)(IAudioVolumeDuckNotification*, const WCHAR*))this_->v->tbl[4](this_, sessionID)); }

static inline HRESULT IAudioVolumeLevel_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IAudioVolumeLevel*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IAudioVolumeLevel_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioVolumeLevel*))this_->v->tbl[1](this_)); }
static inline UINT32 IAudioVolumeLevel_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IAudioVolumeLevel*))this_->v->tbl[2](this_)); }
static inline HRESULT IAudioVolumeLevel_GetChannelCount(base_type *this_, UINT32* pcChannels) { return(((HRESULT (WINAPI*)(IAudioVolumeLevel*, UINT32*))this_->v->tbl[3](this_, pcChannels)); }
static inline HRESULT IAudioVolumeLevel_GetLevelRange(base_type *this_, UINT32 nChannel, FLOAT* pfMinLevelDB, FLOAT* pfMaxLevelDB, FLOAT* pfStepping) { return(((HRESULT (WINAPI*)(IAudioVolumeLevel*, UINT32, FLOAT*, FLOAT*, FLOAT*))this_->v->tbl[4](this_, nChannel, pfMinLevelDB, pfMaxLevelDB, pfStepping)); }
static inline HRESULT IAudioVolumeLevel_GetLevel(base_type *this_, UINT32 nChannel, FLOAT* pfLevelDB) { return(((HRESULT (WINAPI*)(IAudioVolumeLevel*, UINT32, FLOAT*))this_->v->tbl[5](this_, nChannel, pfLevelDB)); }
static inline HRESULT IAudioVolumeLevel_SetLevel(base_type *this_, UINT32 nChannel, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioVolumeLevel*, UINT32, FLOAT, const Guid*))this_->v->tbl[6](this_, nChannel, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioVolumeLevel_SetLevelUniform(base_type *this_, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioVolumeLevel*, FLOAT, const Guid*))this_->v->tbl[7](this_, fLevelDB, pguidEventContext)); }
static inline HRESULT IAudioVolumeLevel_SetLevelAllChannels(base_type *this_, FLOAT* aLevelsDB, UINT32 cChannels, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IAudioVolumeLevel*, FLOAT*, UINT32, const Guid*))this_->v->tbl[8](this_, aLevelsDB, cChannels, pguidEventContext)); }

static inline HRESULT IChannelAudioVolume_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IChannelAudioVolume*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IChannelAudioVolume_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IChannelAudioVolume*))this_->v->tbl[1](this_)); }
static inline UINT32 IChannelAudioVolume_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IChannelAudioVolume*))this_->v->tbl[2](this_)); }
static inline HRESULT IChannelAudioVolume_GetChannelCount(base_type *this_, UINT32* pdwCount) { return(((HRESULT (WINAPI*)(IChannelAudioVolume*, UINT32*))this_->v->tbl[3](this_, pdwCount)); }
static inline HRESULT IChannelAudioVolume_SetChannelVolume(base_type *this_, UINT32 dwIndex, const FLOAT fLevel, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IChannelAudioVolume*, UINT32, const FLOAT, const Guid*))this_->v->tbl[4](this_, dwIndex, fLevel, EventContext)); }
static inline HRESULT IChannelAudioVolume_GetChannelVolume(base_type *this_, UINT32 dwIndex, FLOAT* pfLevel) { return(((HRESULT (WINAPI*)(IChannelAudioVolume*, UINT32, FLOAT*))this_->v->tbl[5](this_, dwIndex, pfLevel)); }
static inline HRESULT IChannelAudioVolume_SetAllVolumes(base_type *this_, UINT32 dwCount, const FLOAT* pfVolumes, const Guid* EventContext) { return(((HRESULT (WINAPI*)(IChannelAudioVolume*, UINT32, const FLOAT*, const Guid*))this_->v->tbl[6](this_, dwCount, pfVolumes, EventContext)); }
static inline HRESULT IChannelAudioVolume_GetAllVolumes(base_type *this_, UINT32 dwCount, FLOAT* pfVolumes) { return(((HRESULT (WINAPI*)(IChannelAudioVolume*, UINT32, FLOAT*))this_->v->tbl[7](this_, dwCount, pfVolumes)); }

static inline HRESULT IConnector_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IConnector*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IConnector_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IConnector*))this_->v->tbl[1](this_)); }
static inline UINT32 IConnector_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IConnector*))this_->v->tbl[2](this_)); }
static inline HRESULT IConnector_GetType(base_type *this_, ConnectorType* pType) { return(((HRESULT (WINAPI*)(IConnector*, ConnectorType*))this_->v->tbl[3](this_, pType)); }
static inline HRESULT IConnector_GetDataFlow(base_type *this_, DataFlow* pFlow) { return(((HRESULT (WINAPI*)(IConnector*, DataFlow*))this_->v->tbl[4](this_, pFlow)); }
static inline HRESULT IConnector_ConnectTo(base_type *this_, IConnector* pConnectTo) { return(((HRESULT (WINAPI*)(IConnector*, IConnector*))this_->v->tbl[5](this_, pConnectTo)); }
static inline HRESULT IConnector_Disconnect(base_type *this_) { return(((HRESULT (WINAPI*)(IConnector*))this_->v->tbl[6](this_)); }
static inline HRESULT IConnector_IsConnected(base_type *this_, BOOL* pbConnected) { return(((HRESULT (WINAPI*)(IConnector*, BOOL*))this_->v->tbl[7](this_, pbConnected)); }
static inline HRESULT IConnector_GetConnectedTo(base_type *this_, IConnector** ppConTo) { return(((HRESULT (WINAPI*)(IConnector*, IConnector**))this_->v->tbl[8](this_, ppConTo)); }
static inline HRESULT IConnector_GetConnectorIdConnectedTo(base_type *this_, WCHAR** ppwstrConnectorId) { return(((HRESULT (WINAPI*)(IConnector*, WCHAR**))this_->v->tbl[9](this_, ppwstrConnectorId)); }
static inline HRESULT IConnector_GetDeviceIdConnectedTo(base_type *this_, WCHAR** ppwstrDeviceId) { return(((HRESULT (WINAPI*)(IConnector*, WCHAR**))this_->v->tbl[10](this_, ppwstrDeviceId)); }

static inline HRESULT IControlChangeNotify_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IControlChangeNotify*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IControlChangeNotify_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IControlChangeNotify*))this_->v->tbl[1](this_)); }
static inline UINT32 IControlChangeNotify_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IControlChangeNotify*))this_->v->tbl[2](this_)); }
static inline HRESULT IControlChangeNotify_OnNotify(base_type *this_, UINT32 dwSenderProcessId, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IControlChangeNotify*, UINT32, const Guid*))this_->v->tbl[3](this_, dwSenderProcessId, pguidEventContext)); }

static inline HRESULT IControlInterface_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IControlInterface*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IControlInterface_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IControlInterface*))this_->v->tbl[1](this_)); }
static inline UINT32 IControlInterface_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IControlInterface*))this_->v->tbl[2](this_)); }
static inline HRESULT IControlInterface_GetName(base_type *this_, WCHAR** ppwstrName) { return(((HRESULT (WINAPI*)(IControlInterface*, WCHAR**))this_->v->tbl[3](this_, ppwstrName)); }
static inline HRESULT IControlInterface_GetIID(base_type *this_, Guid* pIID) { return(((HRESULT (WINAPI*)(IControlInterface*, Guid*))this_->v->tbl[4](this_, pIID)); }

static inline HRESULT IDeviceSpecificProperty_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IDeviceSpecificProperty*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IDeviceSpecificProperty_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IDeviceSpecificProperty*))this_->v->tbl[1](this_)); }
static inline UINT32 IDeviceSpecificProperty_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IDeviceSpecificProperty*))this_->v->tbl[2](this_)); }
static inline HRESULT IDeviceSpecificProperty_GetType(base_type *this_, UINT16* pVType) { return(((HRESULT (WINAPI*)(IDeviceSpecificProperty*, UINT16*))this_->v->tbl[3](this_, pVType)); }
static inline HRESULT IDeviceSpecificProperty_GetValue(base_type *this_, void* pvValue, UINT32* pcbValue) { return(((HRESULT (WINAPI*)(IDeviceSpecificProperty*, void*, UINT32*))this_->v->tbl[4](this_, pvValue, pcbValue)); }
static inline HRESULT IDeviceSpecificProperty_SetValue(base_type *this_, void* pvValue, UINT32 cbValue, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IDeviceSpecificProperty*, void*, UINT32, const Guid*))this_->v->tbl[5](this_, pvValue, cbValue, pguidEventContext)); }
static inline HRESULT IDeviceSpecificProperty_Get4BRange(base_type *this_, INT32* plMin, INT32* plMax, INT32* plStepping) { return(((HRESULT (WINAPI*)(IDeviceSpecificProperty*, INT32*, INT32*, INT32*))this_->v->tbl[6](this_, plMin, plMax, plStepping)); }

static inline HRESULT IDeviceTopology_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IDeviceTopology*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IDeviceTopology_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IDeviceTopology*))this_->v->tbl[1](this_)); }
static inline UINT32 IDeviceTopology_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IDeviceTopology*))this_->v->tbl[2](this_)); }
static inline HRESULT IDeviceTopology_GetConnectorCount(base_type *this_, UINT32* pCount) { return(((HRESULT (WINAPI*)(IDeviceTopology*, UINT32*))this_->v->tbl[3](this_, pCount)); }
static inline HRESULT IDeviceTopology_GetConnector(base_type *this_, UINT32 nIndex, IConnector** ppConnector) { return(((HRESULT (WINAPI*)(IDeviceTopology*, UINT32, IConnector**))this_->v->tbl[4](this_, nIndex, ppConnector)); }
static inline HRESULT IDeviceTopology_GetSubunitCount(base_type *this_, UINT32* pCount) { return(((HRESULT (WINAPI*)(IDeviceTopology*, UINT32*))this_->v->tbl[5](this_, pCount)); }
static inline HRESULT IDeviceTopology_GetSubunit(base_type *this_, UINT32 nIndex, ISubunit** ppSubunit) { return(((HRESULT (WINAPI*)(IDeviceTopology*, UINT32, ISubunit**))this_->v->tbl[6](this_, nIndex, ppSubunit)); }
static inline HRESULT IDeviceTopology_GetPartById(base_type *this_, UINT32 nId, IPart** ppPart) { return(((HRESULT (WINAPI*)(IDeviceTopology*, UINT32, IPart**))this_->v->tbl[7](this_, nId, ppPart)); }
static inline HRESULT IDeviceTopology_GetDeviceId(base_type *this_, WCHAR** ppwstrDeviceId) { return(((HRESULT (WINAPI*)(IDeviceTopology*, WCHAR**))this_->v->tbl[8](this_, ppwstrDeviceId)); }
static inline HRESULT IDeviceTopology_GetSignalPath(base_type *this_, IPart* pIPartFrom, IPart* pIPartTo, BOOL bRejectMixedPaths, IPartsList** ppParts) { return(((HRESULT (WINAPI*)(IDeviceTopology*, IPart*, IPart*, BOOL, IPartsList**))this_->v->tbl[9](this_, pIPartFrom, pIPartTo, bRejectMixedPaths, ppParts)); }

static inline HRESULT IMessageFilter_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IMessageFilter*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IMessageFilter_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IMessageFilter*))this_->v->tbl[1](this_)); }
static inline UINT32 IMessageFilter_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IMessageFilter*))this_->v->tbl[2](this_)); }
static inline UINT32 IMessageFilter_HandleInComingCall(base_type *this_, UINT32 dwCallType, HTASK htaskCaller, UINT32 dwTickCount, INTERFACEINFO* lpInterfaceInfo) { return(((UINT32 (WINAPI*)(IMessageFilter*, UINT32, HTASK, UINT32, INTERFACEINFO*))this_->v->tbl[3](this_, dwCallType, htaskCaller, dwTickCount, lpInterfaceInfo)); }
static inline UINT32 IMessageFilter_RetryRejectedCall(base_type *this_, HTASK htaskCallee, UINT32 dwTickCount, UINT32 dwRejectType) { return(((UINT32 (WINAPI*)(IMessageFilter*, HTASK, UINT32, UINT32))this_->v->tbl[4](this_, htaskCallee, dwTickCount, dwRejectType)); }
static inline UINT32 IMessageFilter_MessagePending(base_type *this_, HTASK htaskCallee, UINT32 dwTickCount, UINT32 dwPendingType) { return(((UINT32 (WINAPI*)(IMessageFilter*, HTASK, UINT32, UINT32))this_->v->tbl[5](this_, htaskCallee, dwTickCount, dwPendingType)); }

static inline HRESULT IMMDevice_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IMMDevice*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IMMDevice_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDevice*))this_->v->tbl[1](this_)); }
static inline UINT32 IMMDevice_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDevice*))this_->v->tbl[2](this_)); }
static inline HRESULT IMMDevice_Activate(base_type *this_, const Guid* iid, CLSCTX dwClsCtx, PROPVARIANT* pActivationParams, void** ppInterface) { return(((HRESULT (WINAPI*)(IMMDevice*, const Guid*, CLSCTX, PROPVARIANT*, void**))this_->v->tbl[3](this_, iid, dwClsCtx, pActivationParams, ppInterface)); }
static inline HRESULT IMMDevice_OpenPropertyStore(base_type *this_, STGM stgmAccess, IPropertyStore** ppProperties) { return(((HRESULT (WINAPI*)(IMMDevice*, STGM, IPropertyStore**))this_->v->tbl[4](this_, stgmAccess, ppProperties)); }
static inline HRESULT IMMDevice_GetId(base_type *this_, WCHAR** ppstrId) { return(((HRESULT (WINAPI*)(IMMDevice*, WCHAR**))this_->v->tbl[5](this_, ppstrId)); }
static inline HRESULT IMMDevice_GetState(base_type *this_, DEVICE_STATE* pdwState) { return(((HRESULT (WINAPI*)(IMMDevice*, DEVICE_STATE*))this_->v->tbl[6](this_, pdwState)); }

static inline HRESULT IMMDeviceActivator_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IMMDeviceActivator*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IMMDeviceActivator_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDeviceActivator*))this_->v->tbl[1](this_)); }
static inline UINT32 IMMDeviceActivator_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDeviceActivator*))this_->v->tbl[2](this_)); }
static inline HRESULT IMMDeviceActivator_Activate(base_type *this_, const Guid* iid, IMMDevice* pDevice, PROPVARIANT* pActivationParams, void** ppInterface) { return(((HRESULT (WINAPI*)(IMMDeviceActivator*, const Guid*, IMMDevice*, PROPVARIANT*, void**))this_->v->tbl[3](this_, iid, pDevice, pActivationParams, ppInterface)); }

static inline HRESULT IMMDeviceCollection_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IMMDeviceCollection*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IMMDeviceCollection_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDeviceCollection*))this_->v->tbl[1](this_)); }
static inline UINT32 IMMDeviceCollection_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDeviceCollection*))this_->v->tbl[2](this_)); }
static inline HRESULT IMMDeviceCollection_GetCount(base_type *this_, UINT32* pcDevices) { return(((HRESULT (WINAPI*)(IMMDeviceCollection*, UINT32*))this_->v->tbl[3](this_, pcDevices)); }
static inline HRESULT IMMDeviceCollection_Item(base_type *this_, UINT32 nDevice, IMMDevice** ppDevice) { return(((HRESULT (WINAPI*)(IMMDeviceCollection*, UINT32, IMMDevice**))this_->v->tbl[4](this_, nDevice, ppDevice)); }

static inline HRESULT IMMDeviceEnumerator_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IMMDeviceEnumerator*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IMMDeviceEnumerator_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDeviceEnumerator*))this_->v->tbl[1](this_)); }
static inline UINT32 IMMDeviceEnumerator_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IMMDeviceEnumerator*))this_->v->tbl[2](this_)); }
static inline HRESULT IMMDeviceEnumerator_EnumAudioEndpoints(base_type *this_, EDataFlow dataFlow, DEVICE_STATE dwStateMask, IMMDeviceCollection** ppDevices) { return(((HRESULT (WINAPI*)(IMMDeviceEnumerator*, EDataFlow, DEVICE_STATE, IMMDeviceCollection**))this_->v->tbl[3](this_, dataFlow, dwStateMask, ppDevices)); }
static inline HRESULT IMMDeviceEnumerator_GetDefaultAudioEndpoint(base_type *this_, EDataFlow dataFlow, ERole role, IMMDevice** ppEndpoint) { return(((HRESULT (WINAPI*)(IMMDeviceEnumerator*, EDataFlow, ERole, IMMDevice**))this_->v->tbl[4](this_, dataFlow, role, ppEndpoint)); }
static inline HRESULT IMMDeviceEnumerator_GetDevice(base_type *this_, const WCHAR* pwstrId, IMMDevice** ppDevice) { return(((HRESULT (WINAPI*)(IMMDeviceEnumerator*, const WCHAR*, IMMDevice**))this_->v->tbl[5](this_, pwstrId, ppDevice)); }
static inline HRESULT IMMDeviceEnumerator_RegisterEndpointNotificationCallback(base_type *this_, IMMNotificationClient* pClient) { return(((HRESULT (WINAPI*)(IMMDeviceEnumerator*, IMMNotificationClient*))this_->v->tbl[6](this_, pClient)); }
static inline HRESULT IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(base_type *this_, IMMNotificationClient* pClient) { return(((HRESULT (WINAPI*)(IMMDeviceEnumerator*, IMMNotificationClient*))this_->v->tbl[7](this_, pClient)); }

static inline HRESULT IMMEndpoint_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IMMEndpoint*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IMMEndpoint_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IMMEndpoint*))this_->v->tbl[1](this_)); }
static inline UINT32 IMMEndpoint_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IMMEndpoint*))this_->v->tbl[2](this_)); }
static inline HRESULT IMMEndpoint_GetDataFlow(base_type *this_, EDataFlow* pDataFlow) { return(((HRESULT (WINAPI*)(IMMEndpoint*, EDataFlow*))this_->v->tbl[3](this_, pDataFlow)); }

static inline HRESULT IMMNotificationClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IMMNotificationClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IMMNotificationClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IMMNotificationClient*))this_->v->tbl[1](this_)); }
static inline UINT32 IMMNotificationClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IMMNotificationClient*))this_->v->tbl[2](this_)); }
static inline HRESULT IMMNotificationClient_OnDeviceStateChanged(base_type *this_, const WCHAR* pwstrDeviceId, DEVICE_STATE dwNewState) { return(((HRESULT (WINAPI*)(IMMNotificationClient*, const WCHAR*, DEVICE_STATE))this_->v->tbl[3](this_, pwstrDeviceId, dwNewState)); }
static inline HRESULT IMMNotificationClient_OnDeviceAdded(base_type *this_, const WCHAR* pwstrDeviceId) { return(((HRESULT (WINAPI*)(IMMNotificationClient*, const WCHAR*))this_->v->tbl[4](this_, pwstrDeviceId)); }
static inline HRESULT IMMNotificationClient_OnDeviceRemoved(base_type *this_, const WCHAR* pwstrDeviceId) { return(((HRESULT (WINAPI*)(IMMNotificationClient*, const WCHAR*))this_->v->tbl[5](this_, pwstrDeviceId)); }
static inline HRESULT IMMNotificationClient_OnDefaultDeviceChanged(base_type *this_, EDataFlow flow, ERole role, const WCHAR* pwstrDefaultDeviceId) { return(((HRESULT (WINAPI*)(IMMNotificationClient*, EDataFlow, ERole, const WCHAR*))this_->v->tbl[6](this_, flow, role, pwstrDefaultDeviceId)); }
static inline HRESULT IMMNotificationClient_OnPropertyValueChanged(base_type *this_, const WCHAR* pwstrDeviceId, const PROPERTYKEY key) { return(((HRESULT (WINAPI*)(IMMNotificationClient*, const WCHAR*, const PROPERTYKEY))this_->v->tbl[7](this_, pwstrDeviceId, key)); }

static inline HRESULT IPart_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IPart*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IPart_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IPart*))this_->v->tbl[1](this_)); }
static inline UINT32 IPart_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IPart*))this_->v->tbl[2](this_)); }
static inline HRESULT IPart_GetName(base_type *this_, WCHAR** ppwstrName) { return(((HRESULT (WINAPI*)(IPart*, WCHAR**))this_->v->tbl[3](this_, ppwstrName)); }
static inline HRESULT IPart_GetLocalId(base_type *this_, UINT32* pnId) { return(((HRESULT (WINAPI*)(IPart*, UINT32*))this_->v->tbl[4](this_, pnId)); }
static inline HRESULT IPart_GetGlobalId(base_type *this_, WCHAR** ppwstrGlobalId) { return(((HRESULT (WINAPI*)(IPart*, WCHAR**))this_->v->tbl[5](this_, ppwstrGlobalId)); }
static inline HRESULT IPart_GetPartType(base_type *this_, PartType* pPartType) { return(((HRESULT (WINAPI*)(IPart*, PartType*))this_->v->tbl[6](this_, pPartType)); }
static inline HRESULT IPart_GetSubType(base_type *this_, Guid* pSubType) { return(((HRESULT (WINAPI*)(IPart*, Guid*))this_->v->tbl[7](this_, pSubType)); }
static inline HRESULT IPart_GetControlInterfaceCount(base_type *this_, UINT32* pCount) { return(((HRESULT (WINAPI*)(IPart*, UINT32*))this_->v->tbl[8](this_, pCount)); }
static inline HRESULT IPart_GetControlInterface(base_type *this_, UINT32 nIndex, IControlInterface** ppInterfaceDesc) { return(((HRESULT (WINAPI*)(IPart*, UINT32, IControlInterface**))this_->v->tbl[9](this_, nIndex, ppInterfaceDesc)); }
static inline HRESULT IPart_EnumPartsIncoming(base_type *this_, IPartsList** ppParts) { return(((HRESULT (WINAPI*)(IPart*, IPartsList**))this_->v->tbl[10](this_, ppParts)); }
static inline HRESULT IPart_EnumPartsOutgoing(base_type *this_, IPartsList** ppParts) { return(((HRESULT (WINAPI*)(IPart*, IPartsList**))this_->v->tbl[11](this_, ppParts)); }
static inline HRESULT IPart_GetTopologyObject(base_type *this_, IDeviceTopology** ppTopology) { return(((HRESULT (WINAPI*)(IPart*, IDeviceTopology**))this_->v->tbl[12](this_, ppTopology)); }
static inline HRESULT IPart_Activate(base_type *this_, UINT32 dwClsContext, const Guid* refiid, void** ppvObject) { return(((HRESULT (WINAPI*)(IPart*, UINT32, const Guid*, void**))this_->v->tbl[13](this_, dwClsContext, refiid, ppvObject)); }
static inline HRESULT IPart_RegisterControlChangeCallback(base_type *this_, const Guid* riid, IControlChangeNotify* pNotify) { return(((HRESULT (WINAPI*)(IPart*, const Guid*, IControlChangeNotify*))this_->v->tbl[14](this_, riid, pNotify)); }
static inline HRESULT IPart_UnregisterControlChangeCallback(base_type *this_, IControlChangeNotify* pNotify) { return(((HRESULT (WINAPI*)(IPart*, IControlChangeNotify*))this_->v->tbl[15](this_, pNotify)); }

static inline HRESULT IPartsList_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IPartsList*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IPartsList_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IPartsList*))this_->v->tbl[1](this_)); }
static inline UINT32 IPartsList_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IPartsList*))this_->v->tbl[2](this_)); }
static inline HRESULT IPartsList_GetCount(base_type *this_, UINT32* pCount) { return(((HRESULT (WINAPI*)(IPartsList*, UINT32*))this_->v->tbl[3](this_, pCount)); }
static inline HRESULT IPartsList_GetPart(base_type *this_, UINT32 nIndex, IPart** ppPart) { return(((HRESULT (WINAPI*)(IPartsList*, UINT32, IPart**))this_->v->tbl[4](this_, nIndex, ppPart)); }

static inline HRESULT IPerChannelDbLevel_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(IPerChannelDbLevel*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 IPerChannelDbLevel_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(IPerChannelDbLevel*))this_->v->tbl[1](this_)); }
static inline UINT32 IPerChannelDbLevel_Release(base_type *this_) { return(((UINT32 (WINAPI*)(IPerChannelDbLevel*))this_->v->tbl[2](this_)); }
static inline HRESULT IPerChannelDbLevel_GetChannelCount(base_type *this_, UINT32* pcChannels) { return(((HRESULT (WINAPI*)(IPerChannelDbLevel*, UINT32*))this_->v->tbl[3](this_, pcChannels)); }
static inline HRESULT IPerChannelDbLevel_GetLevelRange(base_type *this_, UINT32 nChannel, FLOAT* pfMinLevelDB, FLOAT* pfMaxLevelDB, FLOAT* pfStepping) { return(((HRESULT (WINAPI*)(IPerChannelDbLevel*, UINT32, FLOAT*, FLOAT*, FLOAT*))this_->v->tbl[4](this_, nChannel, pfMinLevelDB, pfMaxLevelDB, pfStepping)); }
static inline HRESULT IPerChannelDbLevel_GetLevel(base_type *this_, UINT32 nChannel, FLOAT* pfLevelDB) { return(((HRESULT (WINAPI*)(IPerChannelDbLevel*, UINT32, FLOAT*))this_->v->tbl[5](this_, nChannel, pfLevelDB)); }
static inline HRESULT IPerChannelDbLevel_SetLevel(base_type *this_, UINT32 nChannel, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IPerChannelDbLevel*, UINT32, FLOAT, const Guid*))this_->v->tbl[6](this_, nChannel, fLevelDB, pguidEventContext)); }
static inline HRESULT IPerChannelDbLevel_SetLevelUniform(base_type *this_, FLOAT fLevelDB, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IPerChannelDbLevel*, FLOAT, const Guid*))this_->v->tbl[7](this_, fLevelDB, pguidEventContext)); }
static inline HRESULT IPerChannelDbLevel_SetLevelAllChannels(base_type *this_, FLOAT* aLevelsDB, UINT32 cChannels, const Guid* pguidEventContext) { return(((HRESULT (WINAPI*)(IPerChannelDbLevel*, FLOAT*, UINT32, const Guid*))this_->v->tbl[8](this_, aLevelsDB, cChannels, pguidEventContext)); }

static inline HRESULT ISimpleAudioVolume_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISimpleAudioVolume*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISimpleAudioVolume_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISimpleAudioVolume*))this_->v->tbl[1](this_)); }
static inline UINT32 ISimpleAudioVolume_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISimpleAudioVolume*))this_->v->tbl[2](this_)); }
static inline HRESULT ISimpleAudioVolume_SetMasterVolume(base_type *this_, FLOAT fLevel, const Guid* EventContext) { return(((HRESULT (WINAPI*)(ISimpleAudioVolume*, FLOAT, const Guid*))this_->v->tbl[3](this_, fLevel, EventContext)); }
static inline HRESULT ISimpleAudioVolume_GetMasterVolume(base_type *this_, FLOAT* pfLevel) { return(((HRESULT (WINAPI*)(ISimpleAudioVolume*, FLOAT*))this_->v->tbl[4](this_, pfLevel)); }
static inline HRESULT ISimpleAudioVolume_SetMute(base_type *this_, const BOOL bMute, const Guid* EventContext) { return(((HRESULT (WINAPI*)(ISimpleAudioVolume*, const BOOL, const Guid*))this_->v->tbl[5](this_, bMute, EventContext)); }
static inline HRESULT ISimpleAudioVolume_GetMute(base_type *this_, BOOL* pbMute) { return(((HRESULT (WINAPI*)(ISimpleAudioVolume*, BOOL*))this_->v->tbl[6](this_, pbMute)); }

static inline HRESULT ISpatialAudioClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioClient*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioClient*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioClient_GetStaticObjectPosition(base_type *this_, AudioObjectType type, FLOAT* x, FLOAT* y, FLOAT* z) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, AudioObjectType, FLOAT*, FLOAT*, FLOAT*))this_->v->tbl[3](this_, type, x, y, z)); }
static inline HRESULT ISpatialAudioClient_GetNativeStaticObjectTypeMask(base_type *this_, AudioObjectType* mask) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, AudioObjectType*))this_->v->tbl[4](this_, mask)); }
static inline HRESULT ISpatialAudioClient_GetMaxDynamicObjectCount(base_type *this_, UINT32* value) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, UINT32*))this_->v->tbl[5](this_, value)); }
static inline HRESULT ISpatialAudioClient_GetSupportedAudioObjectFormatEnumerator(base_type *this_, IAudioFormatEnumerator** enumerator) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, IAudioFormatEnumerator**))this_->v->tbl[6](this_, enumerator)); }
static inline HRESULT ISpatialAudioClient_GetMaxFrameCount(base_type *this_, const WAVEFORMATEX* objectFormat, UINT32* frameCountPerBuffer) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, const WAVEFORMATEX*, UINT32*))this_->v->tbl[7](this_, objectFormat, frameCountPerBuffer)); }
static inline HRESULT ISpatialAudioClient_IsAudioObjectFormatSupported(base_type *this_, const WAVEFORMATEX* objectFormat) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, const WAVEFORMATEX*))this_->v->tbl[8](this_, objectFormat)); }
static inline HRESULT ISpatialAudioClient_IsSpatialAudioStreamAvailable(base_type *this_, const Guid* streamUuid, const PROPVARIANT* auxiliaryInfo) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, const Guid*, const PROPVARIANT*))this_->v->tbl[9](this_, streamUuid, auxiliaryInfo)); }
static inline HRESULT ISpatialAudioClient_ActivateSpatialAudioStream(base_type *this_, const PROPVARIANT* activationParams, const Guid* riid, void** stream) { return(((HRESULT (WINAPI*)(ISpatialAudioClient*, const PROPVARIANT*, const Guid*, void**))this_->v->tbl[10](this_, activationParams, riid, stream)); }

static inline HRESULT ISpatialAudioClient2_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioClient2_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioClient2*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioClient2_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioClient2*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioClient2_GetStaticObjectPosition(base_type *this_, AudioObjectType type, FLOAT* x, FLOAT* y, FLOAT* z) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, AudioObjectType, FLOAT*, FLOAT*, FLOAT*))this_->v->tbl[3](this_, type, x, y, z)); }
static inline HRESULT ISpatialAudioClient2_GetNativeStaticObjectTypeMask(base_type *this_, AudioObjectType* mask) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, AudioObjectType*))this_->v->tbl[4](this_, mask)); }
static inline HRESULT ISpatialAudioClient2_GetMaxDynamicObjectCount(base_type *this_, UINT32* value) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, UINT32*))this_->v->tbl[5](this_, value)); }
static inline HRESULT ISpatialAudioClient2_GetSupportedAudioObjectFormatEnumerator(base_type *this_, IAudioFormatEnumerator** enumerator) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, IAudioFormatEnumerator**))this_->v->tbl[6](this_, enumerator)); }
static inline HRESULT ISpatialAudioClient2_GetMaxFrameCount(base_type *this_, const WAVEFORMATEX* objectFormat, UINT32* frameCountPerBuffer) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, const WAVEFORMATEX*, UINT32*))this_->v->tbl[7](this_, objectFormat, frameCountPerBuffer)); }
static inline HRESULT ISpatialAudioClient2_IsAudioObjectFormatSupported(base_type *this_, const WAVEFORMATEX* objectFormat) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, const WAVEFORMATEX*))this_->v->tbl[8](this_, objectFormat)); }
static inline HRESULT ISpatialAudioClient2_IsSpatialAudioStreamAvailable(base_type *this_, const Guid* streamUuid, const PROPVARIANT* auxiliaryInfo) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, const Guid*, const PROPVARIANT*))this_->v->tbl[9](this_, streamUuid, auxiliaryInfo)); }
static inline HRESULT ISpatialAudioClient2_ActivateSpatialAudioStream(base_type *this_, const PROPVARIANT* activationParams, const Guid* riid, void** stream) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, const PROPVARIANT*, const Guid*, void**))this_->v->tbl[10](this_, activationParams, riid, stream)); }
static inline HRESULT ISpatialAudioClient2_IsOffloadCapable(base_type *this_, AUDIO_STREAM_CATEGORY category, BOOL* isOffloadCapable) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, AUDIO_STREAM_CATEGORY, BOOL*))this_->v->tbl[11](this_, category, isOffloadCapable)); }
static inline HRESULT ISpatialAudioClient2_GetMaxFrameCountForCategory(base_type *this_, AUDIO_STREAM_CATEGORY category, BOOL offloadEnabled, const WAVEFORMATEX* objectFormat, UINT32* frameCountPerBuffer) { return(((HRESULT (WINAPI*)(ISpatialAudioClient2*, AUDIO_STREAM_CATEGORY, BOOL, const WAVEFORMATEX*, UINT32*))this_->v->tbl[12](this_, category, offloadEnabled, objectFormat, frameCountPerBuffer)); }

static inline HRESULT ISpatialAudioMetadataClient_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataClient*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioMetadataClient_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataClient*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioMetadataClient_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataClient*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioMetadataClient_ActivateSpatialAudioMetadataItems(base_type *this_, UINT16 maxItemCount, UINT16 frameCount, ISpatialAudioMetadataItemsBuffer** metadataItemsBuffer, ISpatialAudioMetadataItems** metadataItems) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataClient*, UINT16, UINT16, ISpatialAudioMetadataItemsBuffer**, ISpatialAudioMetadataItems**))this_->v->tbl[3](this_, maxItemCount, frameCount, metadataItemsBuffer, metadataItems)); }
static inline HRESULT ISpatialAudioMetadataClient_GetSpatialAudioMetadataItemsBufferLength(base_type *this_, UINT16 maxItemCount, UINT32* bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataClient*, UINT16, UINT32*))this_->v->tbl[4](this_, maxItemCount, bufferLength)); }
static inline HRESULT ISpatialAudioMetadataClient_ActivateSpatialAudioMetadataWriter(base_type *this_, SpatialAudioMetadataWriterOverflowMode overflowMode, ISpatialAudioMetadataWriter** metadataWriter) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataClient*, SpatialAudioMetadataWriterOverflowMode, ISpatialAudioMetadataWriter**))this_->v->tbl[5](this_, overflowMode, metadataWriter)); }
static inline HRESULT ISpatialAudioMetadataClient_ActivateSpatialAudioMetadataCopier(base_type *this_, ISpatialAudioMetadataCopier** metadataCopier) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataClient*, ISpatialAudioMetadataCopier**))this_->v->tbl[6](this_, metadataCopier)); }
static inline HRESULT ISpatialAudioMetadataClient_ActivateSpatialAudioMetadataReader(base_type *this_, ISpatialAudioMetadataReader** metadataReader) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataClient*, ISpatialAudioMetadataReader**))this_->v->tbl[7](this_, metadataReader)); }

static inline HRESULT ISpatialAudioMetadataCopier_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataCopier*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioMetadataCopier_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataCopier*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioMetadataCopier_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataCopier*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioMetadataCopier_Open(base_type *this_, ISpatialAudioMetadataItems* metadataItems) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataCopier*, ISpatialAudioMetadataItems*))this_->v->tbl[3](this_, metadataItems)); }
static inline HRESULT ISpatialAudioMetadataCopier_CopyMetadataForFrames(base_type *this_, UINT16 copyFrameCount, SpatialAudioMetadataCopyMode copyMode, ISpatialAudioMetadataItems* dstMetadataItems, UINT16* itemsCopied) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataCopier*, UINT16, SpatialAudioMetadataCopyMode, ISpatialAudioMetadataItems*, UINT16*))this_->v->tbl[4](this_, copyFrameCount, copyMode, dstMetadataItems, itemsCopied)); }
static inline HRESULT ISpatialAudioMetadataCopier_Close(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataCopier*))this_->v->tbl[5](this_)); }

static inline HRESULT ISpatialAudioMetadataItems_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItems*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioMetadataItems_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataItems*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioMetadataItems_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataItems*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioMetadataItems_GetFrameCount(base_type *this_, UINT16* frameCount) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItems*, UINT16*))this_->v->tbl[3](this_, frameCount)); }
static inline HRESULT ISpatialAudioMetadataItems_GetItemCount(base_type *this_, UINT16* itemCount) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItems*, UINT16*))this_->v->tbl[4](this_, itemCount)); }
static inline HRESULT ISpatialAudioMetadataItems_GetMaxItemCount(base_type *this_, UINT16* maxItemCount) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItems*, UINT16*))this_->v->tbl[5](this_, maxItemCount)); }
static inline HRESULT ISpatialAudioMetadataItems_GetMaxValueBufferLength(base_type *this_, UINT32* maxValueBufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItems*, UINT32*))this_->v->tbl[6](this_, maxValueBufferLength)); }
static inline HRESULT ISpatialAudioMetadataItems_GetInfo(base_type *this_, SpatialAudioMetadataItemsInfo* info) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItems*, SpatialAudioMetadataItemsInfo*))this_->v->tbl[7](this_, info)); }

static inline HRESULT ISpatialAudioMetadataItemsBuffer_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItemsBuffer*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioMetadataItemsBuffer_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataItemsBuffer*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioMetadataItemsBuffer_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataItemsBuffer*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioMetadataItemsBuffer_AttachToBuffer(base_type *this_, UINT8* buffer, UINT32 bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItemsBuffer*, UINT8*, UINT32))this_->v->tbl[3](this_, buffer, bufferLength)); }
static inline HRESULT ISpatialAudioMetadataItemsBuffer_AttachToPopulatedBuffer(base_type *this_, UINT8* buffer, UINT32 bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItemsBuffer*, UINT8*, UINT32))this_->v->tbl[4](this_, buffer, bufferLength)); }
static inline HRESULT ISpatialAudioMetadataItemsBuffer_DetachBuffer(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataItemsBuffer*))this_->v->tbl[5](this_)); }

static inline HRESULT ISpatialAudioMetadataReader_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataReader*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioMetadataReader_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataReader*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioMetadataReader_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataReader*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioMetadataReader_Open(base_type *this_, ISpatialAudioMetadataItems* metadataItems) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataReader*, ISpatialAudioMetadataItems*))this_->v->tbl[3](this_, metadataItems)); }
static inline HRESULT ISpatialAudioMetadataReader_ReadNextItem(base_type *this_, UINT8* commandCount, UINT16* frameOffset) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataReader*, UINT8*, UINT16*))this_->v->tbl[4](this_, commandCount, frameOffset)); }
static inline HRESULT ISpatialAudioMetadataReader_ReadNextItemCommand(base_type *this_, UINT8* commandID, void* valueBuffer, UINT32 maxValueBufferLength, UINT32* valueBufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataReader*, UINT8*, void*, UINT32, UINT32*))this_->v->tbl[5](this_, commandID, valueBuffer, maxValueBufferLength, valueBufferLength)); }
static inline HRESULT ISpatialAudioMetadataReader_Close(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataReader*))this_->v->tbl[6](this_)); }

static inline HRESULT ISpatialAudioMetadataWriter_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataWriter*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioMetadataWriter_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataWriter*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioMetadataWriter_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioMetadataWriter*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioMetadataWriter_Open(base_type *this_, ISpatialAudioMetadataItems* metadataItems) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataWriter*, ISpatialAudioMetadataItems*))this_->v->tbl[3](this_, metadataItems)); }
static inline HRESULT ISpatialAudioMetadataWriter_WriteNextItem(base_type *this_, UINT16 frameOffset) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataWriter*, UINT16))this_->v->tbl[4](this_, frameOffset)); }
static inline HRESULT ISpatialAudioMetadataWriter_WriteNextItemCommand(base_type *this_, UINT8 commandID, const void* valueBuffer, UINT32 valueBufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataWriter*, UINT8, const void*, UINT32))this_->v->tbl[5](this_, commandID, valueBuffer, valueBufferLength)); }
static inline HRESULT ISpatialAudioMetadataWriter_Close(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioMetadataWriter*))this_->v->tbl[6](this_)); }

static inline HRESULT ISpatialAudioObject_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObject*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObject_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObject*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObject_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObject*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObject_GetBuffer(base_type *this_, UINT8** buffer, UINT32* bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioObject*, UINT8**, UINT32*))this_->v->tbl[3](this_, buffer, bufferLength)); }
static inline HRESULT ISpatialAudioObject_SetEndOfStream(base_type *this_, UINT32 frameCount) { return(((HRESULT (WINAPI*)(ISpatialAudioObject*, UINT32))this_->v->tbl[4](this_, frameCount)); }
static inline HRESULT ISpatialAudioObject_IsActive(base_type *this_, BOOL* isActive) { return(((HRESULT (WINAPI*)(ISpatialAudioObject*, BOOL*))this_->v->tbl[5](this_, isActive)); }
static inline HRESULT ISpatialAudioObject_GetAudioObjectType(base_type *this_, AudioObjectType* audioObjectType) { return(((HRESULT (WINAPI*)(ISpatialAudioObject*, AudioObjectType*))this_->v->tbl[6](this_, audioObjectType)); }
static inline HRESULT ISpatialAudioObject_SetPosition(base_type *this_, FLOAT x, FLOAT y, FLOAT z) { return(((HRESULT (WINAPI*)(ISpatialAudioObject*, FLOAT, FLOAT, FLOAT))this_->v->tbl[7](this_, x, y, z)); }
static inline HRESULT ISpatialAudioObject_SetVolume(base_type *this_, FLOAT volume) { return(((HRESULT (WINAPI*)(ISpatialAudioObject*, FLOAT))this_->v->tbl[8](this_, volume)); }

static inline HRESULT ISpatialAudioObjectBase_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectBase*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectBase_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectBase*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectBase_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectBase*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectBase_GetBuffer(base_type *this_, UINT8** buffer, UINT32* bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectBase*, UINT8**, UINT32*))this_->v->tbl[3](this_, buffer, bufferLength)); }
static inline HRESULT ISpatialAudioObjectBase_SetEndOfStream(base_type *this_, UINT32 frameCount) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectBase*, UINT32))this_->v->tbl[4](this_, frameCount)); }
static inline HRESULT ISpatialAudioObjectBase_IsActive(base_type *this_, BOOL* isActive) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectBase*, BOOL*))this_->v->tbl[5](this_, isActive)); }
static inline HRESULT ISpatialAudioObjectBase_GetAudioObjectType(base_type *this_, AudioObjectType* audioObjectType) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectBase*, AudioObjectType*))this_->v->tbl[6](this_, audioObjectType)); }

static inline HRESULT ISpatialAudioObjectForHrtf_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectForHrtf_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectForHrtf*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectForHrtf_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectForHrtf*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectForHrtf_GetBuffer(base_type *this_, UINT8** buffer, UINT32* bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, UINT8**, UINT32*))this_->v->tbl[3](this_, buffer, bufferLength)); }
static inline HRESULT ISpatialAudioObjectForHrtf_SetEndOfStream(base_type *this_, UINT32 frameCount) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, UINT32))this_->v->tbl[4](this_, frameCount)); }
static inline HRESULT ISpatialAudioObjectForHrtf_IsActive(base_type *this_, BOOL* isActive) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, BOOL*))this_->v->tbl[5](this_, isActive)); }
static inline HRESULT ISpatialAudioObjectForHrtf_GetAudioObjectType(base_type *this_, AudioObjectType* audioObjectType) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, AudioObjectType*))this_->v->tbl[6](this_, audioObjectType)); }
static inline HRESULT ISpatialAudioObjectForHrtf_SetPosition(base_type *this_, FLOAT x, FLOAT y, FLOAT z) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, FLOAT, FLOAT, FLOAT))this_->v->tbl[7](this_, x, y, z)); }
static inline HRESULT ISpatialAudioObjectForHrtf_SetGain(base_type *this_, FLOAT gain) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, FLOAT))this_->v->tbl[8](this_, gain)); }
static inline HRESULT ISpatialAudioObjectForHrtf_SetOrientation(base_type *this_, const FLOAT** orientation) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, const FLOAT**))this_->v->tbl[9](this_, orientation)); }
static inline HRESULT ISpatialAudioObjectForHrtf_SetEnvironment(base_type *this_, SpatialAudioHrtfEnvironmentType environment) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, SpatialAudioHrtfEnvironmentType))this_->v->tbl[10](this_, environment)); }
static inline HRESULT ISpatialAudioObjectForHrtf_SetDistanceDecay(base_type *this_, SpatialAudioHrtfDistanceDecay* distanceDecay) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, SpatialAudioHrtfDistanceDecay*))this_->v->tbl[11](this_, distanceDecay)); }
static inline HRESULT ISpatialAudioObjectForHrtf_SetDirectivity(base_type *this_, SpatialAudioHrtfDirectivityUnion* directivity) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForHrtf*, SpatialAudioHrtfDirectivityUnion*))this_->v->tbl[12](this_, directivity)); }

static inline HRESULT ISpatialAudioObjectForMetadataCommands_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataCommands*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectForMetadataCommands_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectForMetadataCommands*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectForMetadataCommands_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectForMetadataCommands*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectForMetadataCommands_GetBuffer(base_type *this_, UINT8** buffer, UINT32* bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataCommands*, UINT8**, UINT32*))this_->v->tbl[3](this_, buffer, bufferLength)); }
static inline HRESULT ISpatialAudioObjectForMetadataCommands_SetEndOfStream(base_type *this_, UINT32 frameCount) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataCommands*, UINT32))this_->v->tbl[4](this_, frameCount)); }
static inline HRESULT ISpatialAudioObjectForMetadataCommands_IsActive(base_type *this_, BOOL* isActive) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataCommands*, BOOL*))this_->v->tbl[5](this_, isActive)); }
static inline HRESULT ISpatialAudioObjectForMetadataCommands_GetAudioObjectType(base_type *this_, AudioObjectType* audioObjectType) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataCommands*, AudioObjectType*))this_->v->tbl[6](this_, audioObjectType)); }
static inline HRESULT ISpatialAudioObjectForMetadataCommands_WriteNextMetadataCommand(base_type *this_, UINT8 commandID, void* valueBuffer, UINT32 valueBufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataCommands*, UINT8, void*, UINT32))this_->v->tbl[7](this_, commandID, valueBuffer, valueBufferLength)); }

static inline HRESULT ISpatialAudioObjectForMetadataItems_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataItems*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectForMetadataItems_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectForMetadataItems*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectForMetadataItems_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectForMetadataItems*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectForMetadataItems_GetBuffer(base_type *this_, UINT8** buffer, UINT32* bufferLength) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataItems*, UINT8**, UINT32*))this_->v->tbl[3](this_, buffer, bufferLength)); }
static inline HRESULT ISpatialAudioObjectForMetadataItems_SetEndOfStream(base_type *this_, UINT32 frameCount) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataItems*, UINT32))this_->v->tbl[4](this_, frameCount)); }
static inline HRESULT ISpatialAudioObjectForMetadataItems_IsActive(base_type *this_, BOOL* isActive) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataItems*, BOOL*))this_->v->tbl[5](this_, isActive)); }
static inline HRESULT ISpatialAudioObjectForMetadataItems_GetAudioObjectType(base_type *this_, AudioObjectType* audioObjectType) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataItems*, AudioObjectType*))this_->v->tbl[6](this_, audioObjectType)); }
static inline HRESULT ISpatialAudioObjectForMetadataItems_GetSpatialAudioMetadataItems(base_type *this_, ISpatialAudioMetadataItems** metadataItems) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectForMetadataItems*, ISpatialAudioMetadataItems**))this_->v->tbl[7](this_, metadataItems)); }

static inline HRESULT ISpatialAudioObjectRenderStream_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectRenderStream_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStream*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectRenderStream_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStream*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStream_GetAvailableDynamicObjectCount(base_type *this_, UINT32* value) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*, UINT32*))this_->v->tbl[3](this_, value)); }
static inline HRESULT ISpatialAudioObjectRenderStream_GetService(base_type *this_, const Guid* riid, void** service) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*, const Guid*, void**))this_->v->tbl[4](this_, riid, service)); }
static inline HRESULT ISpatialAudioObjectRenderStream_Start(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*))this_->v->tbl[5](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStream_Stop(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*))this_->v->tbl[6](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStream_Reset(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*))this_->v->tbl[7](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStream_BeginUpdatingAudioObjects(base_type *this_, UINT32* availableDynamicObjectCount, UINT32* frameCountPerBuffer) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*, UINT32*, UINT32*))this_->v->tbl[8](this_, availableDynamicObjectCount, frameCountPerBuffer)); }
static inline HRESULT ISpatialAudioObjectRenderStream_EndUpdatingAudioObjects(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*))this_->v->tbl[9](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStream_ActivateSpatialAudioObject(base_type *this_, AudioObjectType type, ISpatialAudioObject** audioObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStream*, AudioObjectType, ISpatialAudioObject**))this_->v->tbl[10](this_, type, audioObject)); }

static inline HRESULT ISpatialAudioObjectRenderStreamBase_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectRenderStreamBase_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamBase*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectRenderStreamBase_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamBase*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamBase_GetAvailableDynamicObjectCount(base_type *this_, UINT32* value) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*, UINT32*))this_->v->tbl[3](this_, value)); }
static inline HRESULT ISpatialAudioObjectRenderStreamBase_GetService(base_type *this_, const Guid* riid, void** service) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*, const Guid*, void**))this_->v->tbl[4](this_, riid, service)); }
static inline HRESULT ISpatialAudioObjectRenderStreamBase_Start(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*))this_->v->tbl[5](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamBase_Stop(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*))this_->v->tbl[6](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamBase_Reset(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*))this_->v->tbl[7](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamBase_BeginUpdatingAudioObjects(base_type *this_, UINT32* availableDynamicObjectCount, UINT32* frameCountPerBuffer) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*, UINT32*, UINT32*))this_->v->tbl[8](this_, availableDynamicObjectCount, frameCountPerBuffer)); }
static inline HRESULT ISpatialAudioObjectRenderStreamBase_EndUpdatingAudioObjects(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamBase*))this_->v->tbl[9](this_)); }

static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectRenderStreamForHrtf_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectRenderStreamForHrtf_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_GetAvailableDynamicObjectCount(base_type *this_, UINT32* value) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*, UINT32*))this_->v->tbl[3](this_, value)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_GetService(base_type *this_, const Guid* riid, void** service) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*, const Guid*, void**))this_->v->tbl[4](this_, riid, service)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_Start(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*))this_->v->tbl[5](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_Stop(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*))this_->v->tbl[6](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_Reset(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*))this_->v->tbl[7](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_BeginUpdatingAudioObjects(base_type *this_, UINT32* availableDynamicObjectCount, UINT32* frameCountPerBuffer) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*, UINT32*, UINT32*))this_->v->tbl[8](this_, availableDynamicObjectCount, frameCountPerBuffer)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_EndUpdatingAudioObjects(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*))this_->v->tbl[9](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForHrtf_ActivateSpatialAudioObjectForHrtf(base_type *this_, AudioObjectType type, ISpatialAudioObjectForHrtf** audioObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForHrtf*, AudioObjectType, ISpatialAudioObjectForHrtf**))this_->v->tbl[10](this_, type, audioObject)); }

static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectRenderStreamForMetadata_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectRenderStreamForMetadata_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_GetAvailableDynamicObjectCount(base_type *this_, UINT32* value) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*, UINT32*))this_->v->tbl[3](this_, value)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_GetService(base_type *this_, const Guid* riid, void** service) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*, const Guid*, void**))this_->v->tbl[4](this_, riid, service)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_Start(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*))this_->v->tbl[5](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_Stop(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*))this_->v->tbl[6](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_Reset(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*))this_->v->tbl[7](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_BeginUpdatingAudioObjects(base_type *this_, UINT32* availableDynamicObjectCount, UINT32* frameCountPerBuffer) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*, UINT32*, UINT32*))this_->v->tbl[8](this_, availableDynamicObjectCount, frameCountPerBuffer)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_EndUpdatingAudioObjects(base_type *this_) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*))this_->v->tbl[9](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_ActivateSpatialAudioObjectForMetadataCommands(base_type *this_, AudioObjectType type, ISpatialAudioObjectForMetadataCommands** audioObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*, AudioObjectType, ISpatialAudioObjectForMetadataCommands**))this_->v->tbl[10](this_, type, audioObject)); }
static inline HRESULT ISpatialAudioObjectRenderStreamForMetadata_ActivateSpatialAudioObjectForMetadataItems(base_type *this_, AudioObjectType type, ISpatialAudioObjectForMetadataItems** audioObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamForMetadata*, AudioObjectType, ISpatialAudioObjectForMetadataItems**))this_->v->tbl[11](this_, type, audioObject)); }

static inline HRESULT ISpatialAudioObjectRenderStreamNotify_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamNotify*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISpatialAudioObjectRenderStreamNotify_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamNotify*))this_->v->tbl[1](this_)); }
static inline UINT32 ISpatialAudioObjectRenderStreamNotify_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISpatialAudioObjectRenderStreamNotify*))this_->v->tbl[2](this_)); }
static inline HRESULT ISpatialAudioObjectRenderStreamNotify_OnAvailableDynamicObjectCountChange(base_type *this_, ISpatialAudioObjectRenderStreamBase* sender, INT64 hnsComplianceDeadlineTime, UINT32 availableDynamicObjectCountChange) { return(((HRESULT (WINAPI*)(ISpatialAudioObjectRenderStreamNotify*, ISpatialAudioObjectRenderStreamBase*, INT64, UINT32))this_->v->tbl[3](this_, sender, hnsComplianceDeadlineTime, availableDynamicObjectCountChange)); }

static inline HRESULT ISubunit_QueryInterface(base_type *this_, const Guid* riid, void** ppvObject) { return(((HRESULT (WINAPI*)(ISubunit*, const Guid*, void**))this_->v->tbl[0](this_, riid, ppvObject)); }
static inline UINT32 ISubunit_AddRef(base_type *this_) { return(((UINT32 (WINAPI*)(ISubunit*))this_->v->tbl[1](this_)); }
static inline UINT32 ISubunit_Release(base_type *this_) { return(((UINT32 (WINAPI*)(ISubunit*))this_->v->tbl[2](this_)); }

// functions

EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI ActivateAudioInterfaceAsync                            (const WCHAR* deviceInterfacePath, const Guid* riid, PROPVARIANT* activationParams, IActivateAudioInterfaceCompletionHandler* completionHandler, IActivateAudioInterfaceAsyncOperation** activationOperation) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CoRegisterMessageFilter                                (IMessageFilter* lpMessageFilter, IMessageFilter** lplpMessageFilter) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateCaptureAudioStateMonitor                         (IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateCaptureAudioStateMonitorForCategory              (AUDIO_STREAM_CATEGORY category, IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateCaptureAudioStateMonitorForCategoryAndDeviceId   (AUDIO_STREAM_CATEGORY category, const WCHAR* deviceId, IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateCaptureAudioStateMonitorForCategoryAndDeviceRole (AUDIO_STREAM_CATEGORY category, ERole role, IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateRenderAudioStateMonitor                          (IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateRenderAudioStateMonitorForCategory               (AUDIO_STREAM_CATEGORY category, IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateRenderAudioStateMonitorForCategoryAndDeviceId    (AUDIO_STREAM_CATEGORY category, const WCHAR* deviceId, IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C HRESULT DECLSPEC_IMPORT WINAPI CreateRenderAudioStateMonitorForCategoryAndDeviceRole  (AUDIO_STREAM_CATEGORY category, ERole role, IAudioStateMonitor** audioStateMonitor) WIN_NOEXCEPT;
EXTERN_C BOOL    DECLSPEC_IMPORT WINAPI PlaySoundA                                             (const PSTR pszSound, HMODULE hmod, SND_FLAGS fdwSound) WIN_NOEXCEPT;
EXTERN_C BOOL    DECLSPEC_IMPORT WINAPI PlaySoundW                                             (const WCHAR* pszSound, HMODULE hmod, SND_FLAGS fdwSound) WIN_NOEXCEPT;
