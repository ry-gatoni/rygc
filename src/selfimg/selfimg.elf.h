// -----------------------------------------------------------------------------
// elf header

#define ELF_FILETYPE_XLIST\
  X(NONE, 0)\
  X(REL, 1)\
  X(EXEC, 2)\
  X(DYN, 3)\
  X(CORE, 4)\
  X(NUM, 5)\
  X(LOOS, 0xFE00)\
  X(HIOS, 0xFEFF)\
  X(LOPROC, 0xFF00)\
  X(HIPROC, 0xFFFF)

typedef U16 Elf_FileType;
enum
{
#define X(name, val) Glue(Elf_FileType_, name) = val,
  ELF_FILETYPE_XLIST
#undef X
};

#define ELF_MACHINETYPE_XLIST\
  X(NONE, 0)\
  X(M32, 1)\
  X(SPARC, 2)\
  X(386, 3)\
  X(68K, 4)\
  X(88K, 5)\
  X(IAMCU, 6)\
  X(860, 7)\
  X(MIPS, 8)\
  X(S370, 9)\
  X(MIPS_RS3_LE, 10)\
  X(PARISC, 15)\
  X(VPP500, 17)\
  X(SPARC32PLUS, 18)\
  X(960, 19)\
  X(PPC, 20)\
  X(PPC64, 21)\
  X(S390, 22)\
  X(SPU, 23)\
  X(V800, 36)\
  X(FR20, 37)\
  X(RH32, 38)\
  X(RCE, 39)\
  X(ARM, 40)\
  X(FAKE_ALPHA, 41)\
  X(SH, 42)\
  X(SPARCV9, 43)\
  X(TRICORE, 44)\
  X(ARC, 45)\
  X(H8_300, 46)\
  X(H8_300H, 47)\
  X(H8S, 48)\
  X(H8_500, 49)\
  X(IA_64, 50)\
  X(MIPS_X, 51)\
  X(COLDFIRE, 52)\
  X(68HC12, 53)\
  X(MMA, 54)\
  X(PCP, 55)\
  X(NCPU, 56)\
  X(NDR1, 57)\
  X(STARCORE, 58)\
  X(ME16, 59)\
  X(ST100, 60)\
  X(TINYJ, 61)\
  X(X86_64, 62)\
  X(PDSP, 63)\
  X(PDP10, 64)\
  X(PDP11, 65)\
  X(FX66, 66)\
  X(ST9PLUS, 67)\
  X(ST7, 68)\
  X(68HC16, 69)\
  X(68HC11, 70)\
  X(68HC08, 71)\
  X(68HC05, 72)\
  X(SVX, 73)\
  X(ST19, 74)\
  X(VAX, 75)\
  X(CRIS, 76)\
  X(JAVELIN, 77)\
  X(FIREPATH, 78)\
  X(ZSP, 79)\
  X(MMIX, 80)\
  X(HUANY, 81)\
  X(PRISM, 82)\
  X(AVR, 83)\
  X(FR30, 84)\
  X(D10V, 85)\
  X(D30V, 86)\
  X(V850, 87)\
  X(M32R, 88)\
  X(MN10300, 89)\
  X(MN10200, 90)\
  X(PJ, 91)\
  X(OPENRISC, 92)\
  X(ARC_COMPACT, 93)\
  X(XTENSA, 94)\
  X(VIDEOCORE, 95)\
  X(TMM_GPP, 96)\
  X(NS32K, 97)\
  X(TPC, 98)\
  X(SNP1K, 99)\
  X(ST200, 100)\
  X(IP2K, 101)\
  X(MAX, 102)\
  X(CR, 103)\
  X(F2MC16, 104)\
  X(MSP430, 105)\
  X(BLACKFIN, 106)\
  X(SE_C33, 107)\
  X(SEP, 108)\
  X(ARCA, 109)\
  X(UNICORE, 110)\
  X(EXCESS, 111)\
  X(DXP, 112)\
  X(ALTERA_NIOS2, 113)\
  X(CRX, 114)\
  X(XGATE, 115)\
  X(C166, 116)\
  X(M16C, 117)\
  X(DSPIC30F, 118)\
  X(CE, 119)\
  X(M32C, 120)\
  X(TSK3000, 131)\
  X(RS08, 132)\
  X(SHARC, 133)\
  X(ECOG2, 134)\
  X(SCORE7, 135)\
  X(DSP24, 136)\
  X(VIDEOCORE3, 137)\
  X(LATTICEMICO32, 138)\
  X(SE_C17, 139)\
  X(TI_C6000, 140)\
  X(TI_C2000, 141)\
  X(TI_C5500, 142)\
  X(TI_ARP32, 143)\
  X(TI_PRU, 144)\
  X(MMDSP_PLUS, 160)\
  X(CYPRESS_M8C, 161)\
  X(R32C, 162)\
  X(TRIMEDIA, 163)\
  X(QDSP6, 164)\
  X(8051, 165)\
  X(STXP7X, 166)\
  X(NDS32, 167)\
  X(ECOG1X, 168)\
  X(MAXQ30, 169)\
  X(XIMO16, 170)\
  X(MANIK, 171)\
  X(CRAYNV2, 172)\
  X(RX, 173)\
  X(METAG, 174)\
  X(MCST_ELBRUS, 175)\
  X(ECOG16, 176)\
  X(CR16, 177)\
  X(ETPU, 178)\
  X(SLE9X, 179)\
  X(L10M, 180)\
  X(K10M, 181)\
  X(AARCH64, 183)\
  X(AVR32, 185)\
  X(STM8, 186)\
  X(TILE64, 187)\
  X(TILEPRO, 188)\
  X(MICROBLAZE, 189)\
  X(CUDA, 190)\
  X(TILEGX, 191)\
  X(CLOUDSHIELD, 192)\
  X(COREA_1ST, 193)\
  X(COREA_2ND, 194)\
  X(ARCV2, 195)\
  X(OPEN8, 196)\
  X(RL78, 197)\
  X(VIDEOCORE5, 198)\
  X(78KOR, 199)\
  X(56800EX, 200)\
  X(BA1, 201)\
  X(BA2, 202)\
  X(XCORE, 203)\
  X(MCHP_PIC, 204)\
  X(INTELGT, 205)\
  X(KM32, 210)\
  X(KMX32, 211)\
  X(EMX16, 212)\
  X(EMX8, 213)\
  X(KVARC, 214)\
  X(CDP, 215)\
  X(COGE, 216)\
  X(COOL, 217)\
  X(NORC, 218)\
  X(CSRKALIMBA, 219)\
  X(Z80, 220)\
  X(VISIUM, 221)\
  X(FT32, 222)\
  X(MOXIE, 223)\
  X(AMDGPU, 224)\
  X(RISCV, 243)\
  X(BPF, 247)\
  X(CSKY, 252)\
  X(LOONGARCH, 258)\
  X(NUM, 259)\
  X(ARC_A5, 93)\
  X(ALPHA, 0x9026)

typedef U16 Elf_MachineType;
enum
{
#define X(name, val) Glue(Elf_MachineType_, name) = val,
  ELF_MACHINETYPE_XLIST
#undef X
};

#define ELF_CLASSTYPE_XLIST\
  X(NONE, 0)\
  X(32, 1)\
  X(64, 2)\
  X(NUM, 3)

typedef U8 Elf_ClassType;
enum
{
#define X(name, value) Glue(Elf_ClassType_, name) = value,
  ELF_CLASSTYPE_XLIST
#undef X
};

#define ELF_DATATYPE_XLIST\
  X(NONE, 0)\
  X(2LSB, 1)\
  X(2MSB, 2)\
  X(NUM, 3)

typedef U8 Elf_DataType;
enum
{
#define X(name, value) Glue(Elf_DataType_, name) = value,
  ELF_DATATYPE_XLIST
#undef X
};

#define ELF_OSABI_XLIST\
  X(NONE, 0)\
  X(SYSV, 0)\
  X(HPUX, 1)\
  X(NETBSD, 2)\
  X(GNU, 3)\
  X(LINUX, 3)\
  X(SOLARIS, 6)\
  X(AIX, 7)\
  X(IRIX, 8)\
  X(FREEBSD, 9)\
  X(TRU64, 10)\
  X(MODESTO, 11)\
  X(OPENBSD, 12)\
  X(ARM_AEABI, 64)\
  X(ARM, 97)\
  X(STANDALONE, 255)

typedef U8 Elf_OsAbi;
enum
{
#define X(name, value) Glue(Elf_OsAbi_, name) = value,
  ELF_OSABI_XLIST
#undef X
};

#define ELF_VERSION_CURRENT 1

#define ELF_IDENT_COUNT 16

typedef struct Elf_Ident
{
  union
  {
    struct
    {
      U32 elf_magic;
      Elf_ClassType class;
      Elf_DataType data;
      U8 version;
      Elf_OsAbi abi;
    };

    U8 raw[ELF_IDENT_COUNT];
  };
} Elf_Ident;

#define ELF_MAGIC FOURCC("\177ELF")

typedef struct Elf_Header32
{
  Elf_Ident             ident;
  Elf_FileType          type;
  Elf_MachineType       machine;
  U32                   version;
  U32                   entry_addr;
  U32                   program_table_file_offset;
  U32                   section_table_file_offset;
  U32                   processor_flags;
  U16                   elf_header_size;
  U16                   program_table_entry_size;
  U16                   program_table_entry_count;
  U16                   section_table_entry_size;
  U16                   section_table_entry_count;
  U16                   section_string_table_idx;
} Elf_Header32;

typedef struct Elf_Header64
{
  Elf_Ident             ident;
  Elf_FileType          type;
  Elf_MachineType       machine;
  U32                   version;
  U64                   entry_addr;
  U64                   program_table_file_offset;
  U64                   section_table_file_offset;
  U32                   processor_flags;
  U16                   elf_header_size;
  U16                   program_table_entry_size;
  U16                   program_table_entry_count;
  U16                   section_table_entry_size;
  U16                   section_table_entry_count;
  U16                   section_string_table_idx;
} Elf_Header64;

#if ARCH_64BIT
typedef Elf_Header64 Elf_Header;
#elif ARCH_32BIT
typedef Elf_Header32 Elf_Header;
#else
#  error ERROR: unsupported architecture
#endif

// -----------------------------------------------------------------------------
// elf section

#define ELF_SECTIONTYPE_XLIST\
  X(UNDEF, 0)\
  X(PROGBITS, 1)\
  X(SYMTAB, 2)\
  X(STRTAB, 3)\
  X(RELA, 4)\
  X(HASH, 5)\
  X(DYNAMIC, 6)\
  X(NOTE, 7)\
  X(NOBITS, 8)\
  X(REL, 9)\
  X(SHLIB, 10)\
  X(DYNSYM, 11)\
  X(INIT_ARRAY, 14)\
  X(FINI_ARRAY, 15)\
  X(PREINIT_ARRAY, 16)\
  X(GROUP, 17)\
  X(SYMTAB_SHNDX, 18)\
  X(RELR, 19)\
  X(NUM, 20)\
  X(LOOS, 0x60000000)\
  X(GNU_ATTRIBUTES, 0x6ffffff5)\
  X(GNU_HASH, 0x6ffffff6)\
  X(GNU_LIBLIST, 0x6ffffff7)\
  X(CHECKSUM, 0x6ffffff8)\
  X(LOSUNW, 0x6ffffffa)\
  X(SUNW_move, 0x6ffffffa)\
  X(SUNW_COMDAT, 0x6ffffffb)\
  X(SUNW_syminfo, 0x6ffffffc)\
  X(GNU_verdef, 0x6ffffffd)\
  X(GNU_verneed, 0x6ffffffe)\
  X(GNU_versym, 0x6fffffff)\
  X(HISUNW, 0x6fffffff)\
  X(HIOS, 0x6fffffff)\
  X(LOPROC, 0x70000000)\
  X(HIPROC, 0x7fffffff)\
  X(LOUSER, 0x80000000)\
  X(HIUSER, 0x8fffffff)

typedef U16 Elf_SectionType;
enum
{
#define X(name, value) Glue(Elf_SectionType_, name) = value,
  ELF_SECTIONTYPE_XLIST
#undef X
};

typedef struct Elf_Section32
{
  U32                   name;   // string table index
  Elf_SectionType       type;
  U32                   flags;
  U32                   exec_addr;
  U32                   file_offset;
  U32                   size;
  U32                   link;
  U32                   info;
  U32                   align;
  U32                   entry_size;
} Elf_Section32;

typedef struct Elf_Section64
{
  U32                   name;   // string table index
  Elf_SectionType       type;
  U64                   flags;
  U64                   exec_addr;
  U64                   file_offset;
  U64                   size;
  U32                   link;
  U32                   info;
  U64                   align;
  U64                   entry_size;
} Elf_Section64;

#if ARCH_64BIT
typedef Elf_Section64 Elf_Section;
#elif ARCH_32BIT
typedef Elf_Section32 Elf_Section;
#else
#  error ERROR: unsupported architecture
#endif

// -----------------------------------------------------------------------------
// elf compression

// -----------------------------------------------------------------------------
// elf symbol

// -----------------------------------------------------------------------------
// elf relocation

// -----------------------------------------------------------------------------
// elf program segment

#define ELF_SEGMENTTYPE_XLIST\
  X(UNUSED, 0)\
  X(LOAD, 1)\
  X(DYNAMIC, 2)\
  X(INTERP, 3)\
  X(NOTE, 4)\
  X(SHLIB, 5)\
  X(PHDR, 6)\
  X(TLS, 7)\
  X(NUM, 8)\
  X(LOOS, 0x60000000)\
  X(GNU_EH_FRAME, 0x6474e550)\
  X(GNU_STACK, 0x6474e551)\
  X(GNU_RELRO, 0x6474e552)\
  X(GNU_PROPERTY, 0x6474e553)\
  X(GNU_SFRAME, 0x6474e554)\
  X(LOSUNW, 0x6ffffffa)\
  X(SUNWBSS, 0x6ffffffa)\
  X(SUNWSTACK, 0x6ffffffb)\
  X(HISUNW, 0x6fffffff)\
  X(HIOS, 0x6fffffff)\
  X(LOPROC, 0x70000000)\
  X(HIPROC, 0x7fffffff)

typedef U32 Elf_SegmentType;
enum
{
#define X(name, value) Glue(Elf_SegmentType_, name) = value,
  ELF_SEGMENTTYPE_XLIST
#undef X
};

#define ELF_SEGMENTFLAGS_XLIST\
  X(X, 1)\
  X(W, 2)\
  X(R, 4)\
  X(MASKOS, 0x0ff00000)\
  X(MASKPROC, 0xf0000000)

typedef U32 Elf_SegmentFlags;
enum
{
#define X(name, value) Glue(Elf_SegmentFlags_, name) = value,
  ELF_SEGMENTFLAGS_XLIST
#undef X
};

typedef struct Elf_Segment32
{
  Elf_SegmentType       type;
  U32                   file_offset;
  U32                   virtual_addr;
  U32                   physical_addr;
  U32                   file_size;
  U32                   mem_size;
  Elf_SegmentFlags      flags;
  U32                   alignment;
} Elf_Segment32;

typedef struct Elf_Segment64
{
  Elf_SegmentType       type;
  Elf_SegmentFlags      flags;
  U64                   file_offset;
  U64                   virtual_addr;
  U64                   physical_addr;
  U64                   file_size;
  U64                   memory_size;
  U64                   alignment;
} Elf_Segment64;

#if ARCH_64BIT
typedef Elf_Segment64 Elf_Segment;
#elif ARCH_32BIT
typedef Elf_Segment32 Elf_Segment;
#else
#  error ERROR: unsupported architecture
#endif
