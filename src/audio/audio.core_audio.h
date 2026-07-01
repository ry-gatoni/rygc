// -----------------------------------------------------------------------------
// api types/constants/functions

typedef S32 OSStatus;

typedef bool Boolean;

typedef long CFIndex;

typedef struct CFRange
{
  CFIndex location;
  CFIndex length;
} CFRange;

proc inline CFRange
CFRangeMake(CFIndex loc, CFIndex len)
{
  CFRange range = {
    .location = loc,
    .length = len,
  };
  return range;
}

typedef U32 CFStringEncoding;
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

typedef struct __CFString *CFStringRef;

extern CFIndex CFStringGetLength(CFStringRef str);
extern CFIndex CFStringGetMaximumSizeForEncoding(CFIndex length, CFStringEncoding encoding);
extern CFIndex CFStringGetBytes(CFStringRef str, CFRange rng, CFStringEncoding encoding, U8 loss_byte, Boolean external_representation, U8 *buf, CFIndex max_buf_len, CFIndex *used_buf_len);
extern Boolean CFStringGetCString(CFStringRef str, char *buffer, CFIndex buffer_size, CFStringEncoding encoding);

typedef U32 AudioObjectID;

global int kAudioObjectSystemObject = 1;

typedef U32 AudioObjectPropertySelector;
enum
{
  kAudioObjectPropertyName                                    = 'lnam',

  kAudioHardwarePropertyDevices                               = 'dev#',
  kAudioHardwarePropertyDefaultInputDevice                    = 'dIn ',
  kAudioHardwarePropertyDefaultOutputDevice                   = 'dOut',
};

typedef U32 AudioObjectPropertyScope;
enum
{
  kAudioObjectPropertyScopeGlobal         = 'glob',
  kAudioObjectPropertyScopeInput          = 'inpt',
  kAudioObjectPropertyScopeOutput         = 'outp',
  kAudioObjectPropertyScopePlayThrough    = 'ptru',
  kAudioObjectPropertyElementMain         = 0,
};

typedef U32 AudioObjectPropertyElement;

typedef struct AudioObjectPropertyAddress
{
  AudioObjectPropertySelector selector;
  AudioObjectPropertyScope scope;
  AudioObjectPropertyElement element;
} AudioObjectPropertyAddress;

extern OSStatus AudioObjectGetPropertyDataSize(AudioObjectID inObjectID, const AudioObjectPropertyAddress *inAddress, U32 inQualifierDataSize, const void *inQualifierData, U32 *outDataSize);
extern OSStatus AudioObjectGetPropertyData(AudioObjectID inObjectID, const AudioObjectPropertyAddress *inAddress, U32 inQualifierDataSize, const void *inQualifierData, U32 *ioDataSize, void *outData);

// -----------------------------------------------------------------------------
// internal types

typedef struct CoreAudio_Device CoreAudio_Device;
struct CoreAudio_Device
{
  CoreAudio_Device *next;
  CoreAudio_Device *prev;

  String8 name;
  U32 input_channel_count;
  U32 output_channel_count;

  U64 max_sample_rate;

  AudioObjectID id;
};

typedef struct CoreAudio_State
{
  Arena *arena;

  CoreAudio_Device *first_device;
  CoreAudio_Device *last_device;
  U64 device_count;

  CoreAudio_Device *first_free_device;
  CoreAudio_Device *last_free_device;

  CoreAudio_Device *default_input_device;
  CoreAudio_Device *default_output_device;

  CoreAudio_Device *input_device;
  CoreAudio_Device *output_device;
} CoreAudio_State;

global CoreAudio_State *core_audio_state = 0;

// -----------------------------------------------------------------------------
// state

proc B32 core_audio_init(Arena *arena);
proc void core_audio_uninit(void);

// -----------------------------------------------------------------------------
// devices

proc CoreAudio_Device* core_audio_default_input_device(void);
proc CoreAudio_Device* core_audio_default_output_device(void);

proc CoreAudio_Device* core_audio_input_device(void);
proc CoreAudio_Device* core_audio_output_device(void);

proc void core_audio_set_input_device(CoreAudio_Device *input);
proc void core_audio_set_output_device(CoreAudio_Device *output);

// -----------------------------------------------------------------------------
// helpers

proc inline CoreAudio_Device* core_audio__device_alloc(String8 name);
proc inline void core_audio__device_release(CoreAudio_Device *device);

proc inline Audio_Handle core_audio__handle_from_device(CoreAudio_Device *device);
proc inline CoreAudio_Device* core_audio__device_from_audio_handle(Audio_Handle handle);

proc inline String8 str8_from_cfstr(Arena *arena, CFStringRef cfstr);
