#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>

#if CPU_X86 || CPU_X64
#  include <x86intrin.h>
#endif
