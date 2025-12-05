global U64 SymbolCount(RYGC_SYMBOL_SET) = 0;
global Symbol__Type(RYGC_SYMBOL_SET) *SymbolBasePtr(RYGC_SYMBOL_SET) = 0;
global Symbol__Type(RYGC_SYMBOL_SET) Symbol__Sym(RYGC_SYMBOL_SET, 0);

#if COMPILER_MSVC
#  pragma section(Symbol__Section(RYGC_SYMBOL_SET), read, write)
#endif

BeforeMain()
{
  void *image = os_this_image();
  RangePtr range = selfimg_get_section(image, Str8Lit(Symbol__Section(RYGC_SYMBOL_SET)));
  SymbolCount(RYGC_SYMBOL_SET) = (range.opl - range.first)/sizeof(Symbol__Type(RYGC_SYMBOL_SET));
  SymbolBasePtr(RYGC_SYMBOL_SET) = (Symbol__Type(RYGC_SYMBOL_SET)*)(range.first);
}
