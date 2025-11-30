typedef enum LogMessageKind
{
  LogMessageKind_info,
  LogMessageKind_error,
  LogMessageKind_Count,
} LogMessageKind;

typedef struct LogScope LogScope;
struct LogScope
{
  LogScope *next;
  U64 arena_pos;
  String8 name;
  U64 depth;
  String8List msgs[LogMessageKind_Count];
};

typedef struct LogScopeResult
{
  String8 msgs[LogMessageKind_Count];
} LogScopeResult;

typedef struct Log
{
  Arena *arena;
  LogScope *active_scope;
} Log;

thread_var Log *active_log = 0;

// -----------------------------------------------------------------------------
// allocate, release, selection

proc Log* log_alloc(void);
proc void log_release(Log *log);
proc void log_select(Log *log);

// -----------------------------------------------------------------------------
// messaging

proc void log_msg(LogMessageKind msg_kind, String8 msg);
proc void log_msgf(LogMessageKind msg_kind, char *fmt, ...);

#define log_info(msg) log_msg(LogMessageKind_info, msg)
#define log_infof(fmt, ...) log_msgf(LogMessageKind_info, fmt, __VA_ARGS__)
#define log_error(msg) log_msg(LogMessageKind_error, msg)
#define log_errorf(fmt, ...) log_msgf(LogMessageKind_error, fmt, __VA_ARGS__)

// -----------------------------------------------------------------------------
// scope management

proc void log_begin_scope(String8 scope_name);
proc LogScopeResult log_end_scope(void);
