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

proc Elf_Section*
elf_get_section_from_name(String8 image, String8 section_name)
{
  Elf_Header *elf_header = 0;
  if(image.count >= sizeof(Elf_Header))
  {
    elf_header = (Elf_Header*)image.string;
    if(!elf_is_valid(elf_header)) elf_header = 0;
  }

  char *string_table_base = (char*)(image.string + elf_header->section_string_table_idx);

  Elf_Section *result = 0;
  Elf_Section *section_base = (Elf_Section*)(image.string + elf_header->section_table_offset);
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
  Assert(!"not implemented");

  String8 image = str8(image_raw, 0 /* ?? */); // TODO: how to get loaded image size?
  Elf_Section *section = elf_get_section_from_name(image, section_name);

  RangePtr result = {0};
  result.first = (U8*)section;
  result.opl = result.first + section->size;
  return(result);
}
