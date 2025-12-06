proc B32
elf_is_valid(Elf_Header *header)
{
  B32 result = (header->ident.elf_magic == ELF_MAGIC &&
#if ARCH_64BIT
                header->ident.class == Elf_ClassType_64 &&
# elif ARCH_32BIT
                header->ident.class == ELF_ClassType_32 &&
#else
                0 &&
#endif
                header->ident.version <= ELF_VERSION_CURRENT &&
                (header->type == Elf_FileType_EXEC || header->type == Elf_FileType_DYN));
  return(result);
}

proc U64
elf_loaded_image_size(void *image_raw)
{
  Elf_Header *elf_header = image_raw;
  if(!elf_is_valid(elf_header)) return(0);

  U64 va_min = U64_MAX;
  U64 va_max = 0;
  {
    Elf_Segment *first_segment = (Elf_Segment*)((U8*)image_raw + elf_header->program_table_file_offset);
    Elf_Segment *segment_opl = first_segment + elf_header->program_table_entry_count;
    for(Elf_Segment *seg = first_segment; seg < segment_opl; ++seg)
    {
      if(seg->type != Elf_SegmentType_LOAD) continue;

      va_min = Min(va_min, seg->virtual_addr);
      va_max = Max(va_max, seg->virtual_addr + seg->memory_size);
    }
    // TODO: detect actual page size
    va_min = TruncPow2(va_min, 4096);
    va_max = AlignPow2(va_max, 4096);
  }

  U64 result = va_max - va_min;
  return(result);
}

proc Elf_Section*
elf_get_section_from_name(String8 image, String8 section_name)
{
  Elf_Header *elf_header = 0;
  if(image.count >= sizeof(Elf_Header))
  {
    elf_header = (Elf_Header*)image.string;
    if(!elf_is_valid(elf_header)) elf_header = 0;
  }

  // TODO: this is all wrong
  char *string_table_base = (char*)(image.string + elf_header->section_string_table_idx);

  Elf_Section *result = 0;
  Elf_Section *section_base = (Elf_Section*)(image.string + elf_header->section_table_file_offset);
  Elf_Section *section_opl = section_base + elf_header->section_table_entry_count;
  for(Elf_Section *sec = section_base; sec < section_opl; ++sec)
  {
    char *sec_name_cstr = string_table_base + sec->name;
    String8 sec_name = Str8Cstr(sec_name_cstr);
    if(str8s_are_equal(sec_name, section_name))
    {
      result = sec;
      break;
    }
  }

  return(result);
}

proc RangePtr
selfimg_get_section(void *image_raw, String8 section_name)
{
  U64 image_size = elf_loaded_image_size(image_raw);
  String8 image = str8(image_raw, image_size);
  Assert(!"not implemented");
  Elf_Section *section = elf_get_section_from_name(image, section_name);

  RangePtr result = {0};
  result.first = (U8*)section;
  result.opl = result.first + section->size;
  return(result);
}
