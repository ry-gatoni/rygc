// -----------------------------------------------------------------------------
// types

typedef S32 OSStatus;
typedef unsigned char Boolean;

typedef U32 FourCharCode;
typedef FourCharCode OSType;

// CoreFoundation

typedef long CFIndex;

typedef U32 CFStringEncoding;

typedef struct CFRange
{
  CFIndex location;
  CFIndex length;
} CFRange;

typedef const struct __CFString *CFStringRef;

// CoreAudio

typedef U32 AudioObjectID;

typedef U32 AudioObjectPropertySelector;
typedef U32 AudioObjectPropertyScope;
typedef U32 AudioObjectPropertyElement;

typedef struct AudioObjectPropertyAddress
{
  AudioObjectPropertySelector selector;
  AudioObjectPropertyScope scope;
  AudioObjectPropertyElement element;
} AudioObjectPropertyAddress;

typedef U32 SMPTETimeType;
typedef U32 SMPTETimeFlags;

typedef struct SMPTETime
{
  S16 subframes;
  S16 subframe_divisor;
  U32 counter;
  SMPTETimeType type;
  SMPTETimeFlags flags;
  S16 hours;
  S16 minutes;
  S16 seconds;
  S16 frames;
} SMPTETime;

typedef U32 AudioTimestampFlags;

typedef struct AudioTimestamp
{
  R64 sample_time;
  U64 host_time;
  R64 rate_scalar;
  U64 word_clock_time;
  SMPTETime smpte_time;
  AudioTimestampFlags flags;
  U32 reserved;
} AudioTimestamp;

typedef struct AudioBuffer
{
  U32 num_channels;
  U32 data_byte_size;
  void *data;
} AudioBuffer;

typedef struct AudioBufferList
{
  U32 num_buffers;
  AudioBuffer buffers[1];
} AudioBufferList;

typedef U32 AudioFormatID;
typedef U32 AudioFormatFlags;

typedef struct AudioStreamBasicDescription
{
  R64 sample_rate;
  AudioFormatID format_id;
  AudioFormatFlags format_flags;
  U32 bytes_per_packet;
  U32 frames_per_packet;
  U32 bytes_per_frame;
  U32 channels_per_frame;
  U32 bits_per_channel;
  U32 reserved;
} AudioStreamBasicDescription;

// AudioUnit

typedef U32 AudioUnitRenderActionFlags;

typedef U32 AudioUnitType;
typedef U32 AudioUnitSubtype;

typedef U32 AudioUnitPropertyID;
typedef U32 AudioUnitScope;
typedef U32 AudioUnitElement;

#pragma pack(push, 4)
typedef struct AudioComponentDescription
{
  OSType componentType;
  OSType componentSubType;
  OSType componentManufacturer;
  U32 componentFlags;
  U32 componentFlagsMask;
} AudioComponentDescription;
#pragma pack(pop)

typedef OSStatus (*AURenderCallback)(void *, AudioUnitRenderActionFlags *, const AudioTimestamp *, U32 , U32 , AudioBufferList *);
typedef struct AURenderCallbackStruct
{
  AURenderCallback input_proc;
  void *input_proc_ref_con;
} AURenderCallbackStruct;

typedef void *AudioComponent;
typedef void *AudioComponentInstance;

typedef AudioComponentInstance AudioUnit;

// -----------------------------------------------------------------------------
// constants

// CoreFoundation

// CFStringEncoding
enum
{
  kCFStringEncodingMacRoman = 0,
  kCFStringEncodingWindowsLatin1 = 0x0500, /* ANSI codepage 1252 */
  kCFStringEncodingISOLatin1 = 0x0201, /* ISO 8859-1 */
  kCFStringEncodingNextStepLatin = 0x0B01, /* NextStep encoding*/
  kCFStringEncodingASCII = 0x0600, /* 0..127 (in creating CFString, values greater than 0x7F are treated as corresponding Unicode value) */
  kCFStringEncodingUnicode = 0x0100, /* kTextEncodingUnicodeDefault  + kTextEncodingDefaultFormat (aka kUnicode16BitFormat) */
  kCFStringEncodingUTF8 = 0x08000100, /* kTextEncodingUnicodeDefault + kUnicodeUTF8Format */
  kCFStringEncodingNonLossyASCII = 0x0BFF, /* 7bit Unicode variants used by Cocoa & Java */

  kCFStringEncodingUTF16 = 0x0100, /* kTextEncodingUnicodeDefault + kUnicodeUTF16Format (alias of kCFStringEncodingUnicode) */
  kCFStringEncodingUTF16BE = 0x10000100, /* kTextEncodingUnicodeDefault + kUnicodeUTF16BEFormat */
  kCFStringEncodingUTF16LE = 0x14000100, /* kTextEncodingUnicodeDefault + kUnicodeUTF16LEFormat */

  kCFStringEncodingUTF32 = 0x0c000100, /* kTextEncodingUnicodeDefault + kUnicodeUTF32Format */
  kCFStringEncodingUTF32BE = 0x18000100, /* kTextEncodingUnicodeDefault + kUnicodeUTF32BEFormat */
  kCFStringEncodingUTF32LE = 0x1c000100 /* kTextEncodingUnicodeDefault + kUnicodeUTF32LEFormat */
};

// CoreAudio

global int kAudioObjectSystemObject = 1;

// AudioObjectPropertySelector
enum
{
  kAudioObjectPropertyName                                    = 'lnam',

  kAudioHardwarePropertyDevices                               = 'dev#',
  kAudioHardwarePropertyDefaultInputDevice                    = 'dIn ',
  kAudioHardwarePropertyDefaultOutputDevice                   = 'dOut',
};

// AudioObjectPropertyScope/Element
enum
{
  kAudioObjectPropertyScopeGlobal         = 'glob',
  kAudioObjectPropertyScopeInput          = 'inpt',
  kAudioObjectPropertyScopeOutput         = 'outp',
  kAudioObjectPropertyScopePlayThrough    = 'ptru',
  kAudioObjectPropertyElementMain         = 0,
};

// AudioFormatID
enum
{
  kAudioFormatLinearPCM = 'lpcm',
  kAudioFormatAC3 = 'ac-3',
  kAudioFormat60958AC3 = 'cac3',
  kAudioFormatAppleIMA4 = 'ima4',
  kAudioFormatMPEG4AAC = 'aac ',
  kAudioFormatMPEG4CELP = 'celp',
  kAudioFormatMPEG4HVXC = 'hvxc',
  kAudioFormatMPEG4TwinVQ = 'twvq',
  kAudioFormatMACE3 = 'MAC3',
  kAudioFormatMACE6 = 'MAC6',
  kAudioFormatULaw = 'ulaw',
  kAudioFormatALaw = 'alaw',
  kAudioFormatQDesign = 'QDMC',
  kAudioFormatQDesign2 = 'QDM2',
  kAudioFormatQUALCOMM = 'Qclp',
  kAudioFormatMPEGLayer1 = '.mp1',
  kAudioFormatMPEGLayer2 = '.mp2',
  kAudioFormatMPEGLayer3 = '.mp3',
  kAudioFormatTimeCode = 'time',
  kAudioFormatMIDIStream = 'midi',
  kAudioFormatParameterValueStream = 'apvs',
  kAudioFormatAppleLossless = 'alac',
  kAudioFormatMPEG4AAC_HE = 'aach',
  kAudioFormatMPEG4AAC_LD = 'aacl',
  kAudioFormatMPEG4AAC_ELD = 'aace',
  kAudioFormatMPEG4AAC_ELD_SBR = 'aacf',
  kAudioFormatMPEG4AAC_ELD_V2 = 'aacg',
  kAudioFormatMPEG4AAC_HE_V2 = 'aacp',
  kAudioFormatMPEG4AAC_Spatial = 'aacs',
  kAudioFormatMPEGD_USAC = 'usac',
  kAudioFormatAMR = 'samr',
  kAudioFormatAMR_WB = 'sawb',
  kAudioFormatAudible = 'AUDB',
  kAudioFormatiLBC = 'ilbc',
  kAudioFormatDVIIntelIMA = 0x6D730011,
  kAudioFormatMicrosoftGSM = 0x6D730031,
  kAudioFormatAES3 = 'aes3',
  kAudioFormatEnhancedAC3 = 'ec-3',
  kAudioFormatFLAC = 'flac',
  kAudioFormatOpus = 'opus',
  kAudioFormatAPAC = 'apac',
};

// AudioFormatFlags
enum
{
  kAudioFormatFlagIsFloat = (1U << 0),     // 0x1
  kAudioFormatFlagIsBigEndian = (1U << 1),     // 0x2
  kAudioFormatFlagIsSignedInteger = (1U << 2),     // 0x4
  kAudioFormatFlagIsPacked = (1U << 3),     // 0x8
  kAudioFormatFlagIsAlignedHigh = (1U << 4),     // 0x10
  kAudioFormatFlagIsNonInterleaved = (1U << 5),     // 0x20
  kAudioFormatFlagIsNonMixable = (1U << 6),     // 0x40
  kAudioFormatFlagsAreAllClear = 0x80000000,

  kLinearPCMFormatFlagIsFloat = kAudioFormatFlagIsFloat,
  kLinearPCMFormatFlagIsBigEndian = kAudioFormatFlagIsBigEndian,
  kLinearPCMFormatFlagIsSignedInteger = kAudioFormatFlagIsSignedInteger,
  kLinearPCMFormatFlagIsPacked = kAudioFormatFlagIsPacked,
  kLinearPCMFormatFlagIsAlignedHigh = kAudioFormatFlagIsAlignedHigh,
  kLinearPCMFormatFlagIsNonInterleaved = kAudioFormatFlagIsNonInterleaved,
  kLinearPCMFormatFlagIsNonMixable = kAudioFormatFlagIsNonMixable,
  kLinearPCMFormatFlagsSampleFractionShift = 7,
  kLinearPCMFormatFlagsSampleFractionMask = (0x3F << kLinearPCMFormatFlagsSampleFractionShift),
  kLinearPCMFormatFlagsAreAllClear = kAudioFormatFlagsAreAllClear,

  kAppleLosslessFormatFlag_16BitSourceData = 1,
  kAppleLosslessFormatFlag_20BitSourceData = 2,
  kAppleLosslessFormatFlag_24BitSourceData = 3,
  kAppleLosslessFormatFlag_32BitSourceData = 4
};

// AudioUnit

// AudioUnitRenderActionFlags
enum
{
  kAudioUnitRenderAction_PreRender = (1UL << 2),
  kAudioUnitRenderAction_PostRender = (1UL << 3),
  kAudioUnitRenderAction_OutputIsSilence = (1UL << 4),
  kAudioOfflineUnitRenderAction_Preflight = (1UL << 5),
  kAudioOfflineUnitRenderAction_Render = (1UL << 6),
  kAudioOfflineUnitRenderAction_Complete = (1UL << 7),
  kAudioUnitRenderAction_PostRenderError = (1UL << 8),
  kAudioUnitRenderAction_DoNotCheckRenderArgs = (1UL << 9)
};

// AudioUnitType
enum
{
  kAudioUnitType_Output = 'auou',
  kAudioUnitType_MusicDevice = 'aumu',
  kAudioUnitType_MusicEffect = 'aumf',
  kAudioUnitType_FormatConverter = 'aufc',
  kAudioUnitType_Effect = 'aufx',
  kAudioUnitType_Mixer = 'aumx',
  kAudioUnitType_Panner = 'aupn',
  kAudioUnitType_Generator = 'augn',
  kAudioUnitType_OfflineEffect = 'auol',
  kAudioUnitType_MIDIProcessor = 'aumi',
  kAudioUnitType_SpeechSynthesizer = 'ausp',

  kAudioUnitType_RemoteEffect = 'aurx',
  kAudioUnitType_RemoteGenerator = 'aurg',
  kAudioUnitType_RemoteInstrument = 'auri',
  kAudioUnitType_RemoteMusicEffect = 'aurm',
};

// AudioUnitSubtype
enum
{
  kAudioUnitSubType_GenericOutput = 'genr',
  kAudioUnitSubType_VoiceProcessingIO = 'vpio',

  kAudioUnitSubType_HALOutput = 'ahal',
  kAudioUnitSubType_DefaultOutput = 'def ',
  kAudioUnitSubType_SystemOutput = 'sys ',
};

global U32 kAudioUnitManufacturer_Apple = 'appl';

// AudioUnitPropertyID
enum
{
  kAudioOutputUnitProperty_CurrentDevice = 2000,
  kAudioOutputUnitProperty_IsRunning = 2001,
  kAudioOutputUnitProperty_ChannelMap = 2002, // this will also work with AUConverter
  kAudioOutputUnitProperty_EnableIO = 2003,
  kAudioOutputUnitProperty_StartTime = 2004,
  kAudioOutputUnitProperty_SetInputCallback = 2005,
  kAudioOutputUnitProperty_HasIO = 2006,
  kAudioOutputUnitProperty_StartTimestampsAtZero = 2007,	// this will also work with AUConverter

  kAudioOutputUnitProperty_OSWorkgroup = 2015,

  kAudioUnitProperty_ClassInfo = 0,
  kAudioUnitProperty_MakeConnection = 1,
  kAudioUnitProperty_SampleRate = 2,
  kAudioUnitProperty_ParameterList = 3,
  kAudioUnitProperty_ParameterInfo = 4,
  kAudioUnitProperty_CPULoad = 6,
  kAudioUnitProperty_StreamFormat = 8,
  kAudioUnitProperty_ElementCount = 11,
  kAudioUnitProperty_Latency = 12,
  kAudioUnitProperty_SupportedNumChannels = 13,
  kAudioUnitProperty_MaximumFramesPerSlice = 14,
  kAudioUnitProperty_ParameterValueStrings = 16,
  kAudioUnitProperty_AudioChannelLayout = 19,
  kAudioUnitProperty_TailTime = 20,
  kAudioUnitProperty_BypassEffect = 21,
  kAudioUnitProperty_LastRenderError = 22,
  kAudioUnitProperty_SetRenderCallback = 23,
  kAudioUnitProperty_FactoryPresets = 24,
  kAudioUnitProperty_RenderQuality = 26,
  kAudioUnitProperty_HostCallbacks = 27,
  kAudioUnitProperty_InPlaceProcessing = 29,
  kAudioUnitProperty_ElementName = 30,
  kAudioUnitProperty_SupportedChannelLayoutTags = 32,
  kAudioUnitProperty_PresentPreset = 36,
  kAudioUnitProperty_DependentParameters = 45,
  kAudioUnitProperty_InputSamplesInOutput = 49,
  kAudioUnitProperty_ShouldAllocateBuffer = 51,
  kAudioUnitProperty_FrequencyResponse = 52,
  kAudioUnitProperty_ParameterHistoryInfo = 53,
  kAudioUnitProperty_NickName = 54,
  kAudioUnitProperty_OfflineRender = 37,
  kAudioUnitProperty_ParameterIDName = 34,
  kAudioUnitProperty_ParameterStringFromValue = 33,
  kAudioUnitProperty_ParameterClumpName = 35,
  kAudioUnitProperty_ParameterValueFromString = 38,
  kAudioUnitProperty_ContextName = 25,
  kAudioUnitProperty_PresentationLatency = 40,
  kAudioUnitProperty_ClassInfoFromDocument = 50,
  kAudioUnitProperty_RequestViewController = 56,
  kAudioUnitProperty_ParametersForOverview = 57,
  kAudioUnitProperty_SupportsMPE = 58,
  kAudioUnitProperty_RenderContextObserver = 60,
  kAudioUnitProperty_LastRenderSampleTime = 61,
  kAudioUnitProperty_LoadedOutOfProcess = 62,

  kAudioUnitProperty_FastDispatch = 5,
  kAudioUnitProperty_SetExternalBuffer = 15,
  kAudioUnitProperty_GetUIComponentList = 18,
  kAudioUnitProperty_CocoaUI = 31,
  kAudioUnitProperty_IconLocation = 39,
  kAudioUnitProperty_AUHostIdentifier = 46,

  kAudioUnitProperty_MIDIOutputCallbackInfo = 47,
  kAudioUnitProperty_MIDIOutputCallback = 48,

  kAudioUnitProperty_MIDIOutputEventListCallback = 63,

  kAudioUnitProperty_AudioUnitMIDIProtocol = 64,
  kAudioUnitProperty_HostMIDIProtocol = 65,

  kAudioUnitProperty_MIDIOutputBufferSizeHint = 66,
};

// AudioUnitScope
enum
{
  kAudioUnitScope_Global = 0,
  kAudioUnitScope_Input = 1,
  kAudioUnitScope_Output = 2,
  kAudioUnitScope_Group = 3,
  kAudioUnitScope_Part = 4,
  kAudioUnitScope_Note = 5,
  kAudioUnitScope_Layer = 6,
  kAudioUnitScope_LayerItem = 7,
};

// AudioUnitElement

// -----------------------------------------------------------------------------
// functions

// CoreFoundation

proc inline CFRange
CFRangeMake(CFIndex loc, CFIndex len)
{
  CFRange range = {
    .location = loc,
    .length = len,
  };
  return range;
}

extern CFIndex CFStringGetLength(CFStringRef str);
extern CFIndex CFStringGetMaximumSizeForEncoding(CFIndex length, CFStringEncoding encoding);
extern CFIndex CFStringGetBytes(CFStringRef str, CFRange rng, CFStringEncoding encoding, U8 loss_byte, Boolean external_representation, U8 *buf, CFIndex max_buf_len, CFIndex *used_buf_len);
extern Boolean CFStringGetCString(CFStringRef str, char *buffer, CFIndex buffer_size, CFStringEncoding encoding);

// CoreAudio

extern OSStatus AudioObjectGetPropertyDataSize(AudioObjectID inObjectID, const AudioObjectPropertyAddress *inAddress, U32 inQualifierDataSize, const void *inQualifierData, U32 *outDataSize);
extern OSStatus AudioObjectGetPropertyData(AudioObjectID inObjectID, const AudioObjectPropertyAddress *inAddress, U32 inQualifierDataSize, const void *inQualifierData, U32 *ioDataSize, void *outData);

// AudioUnit

extern AudioComponent AudioComponentFindNext(AudioComponent inComponent, const AudioComponentDescription *inDesc);
extern OSStatus AudioComponentInstanceNew(AudioComponent inComponent, AudioComponentInstance *outInstance);

extern OSStatus AudioUnitSetProperty(AudioUnit inUnit, AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, const void *inData, U32 inDataSize);
extern OSStatus AudioUnitInitialize(AudioUnit inUnit);
extern OSStatus AudioUnitRender(AudioUnit inUnit, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimestamp *inTimeStamp, U32 inOutputBusNumber, U32 inNumberFrames, AudioBufferList *ioData);

extern OSStatus AudioOutputUnitStart(AudioUnit ci);
extern OSStatus AudioOutputUnitStop(AudioUnit ci);
