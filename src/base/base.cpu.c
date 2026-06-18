#if CPU_X86 || CPU_X64
#  include "base.cpu.x86.c"
#elif CPU_ARM || CPU_ARM64
#  include "base.cpu.arm.c"
#else
#  error base cpu layer not implemented for this cpu
#endif
