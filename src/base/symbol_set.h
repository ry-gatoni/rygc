/**
   Credit to Allen "Mr. 4th Dimention" Webster (mr4th.com)

   TERMINOLOGY:
   - count:    the total number of symbols in the set
   - baseptr:  the pointer to the base of the symbol set data array
   - oplptr:   the pointer to one-past-the-last element of the the symbol set data array
   - id:       the 1-based index of the symbol
               id 0 is the special 'nil' symbol
               id [1, count] is a valid symbol in the set
   - metadata: a pointer to the slot in the array for this symbol
   - raw:      address of the metadata as an integer

   USAGE:
   ```c
   // to define a symbol set
   #define SYMBOL_SET_DEFINE example
   #define example_Type ExampleType
   #define example_section ".ryEXMPL"
   #include "symbol_set.define.h"

   // common helper wrappers
   #define ExampleId(name) SymbolId(example, name)
   #define ExampleRaw(name) SymbolRaw(example, name)
   #define ExampleDecl(name) SymbolDeclare(example, name)
   ```
 */

// -----------------------------------------------------------------------------
// user interface

#define SymbolDeclare(symset, name)\
  Section(Glue(symset, _section)) extern Symbol__Type(symset) Symbol__Sym(symset, name)

#define SymbolDefineNameless(symset) SymbolDefine(symset, Glue(_auto__, __COUNTER__))
#define SymbolDefine(symset, name) Symbol__Sym(symset, name) SymbolDeclare(symset, name)

#define SymbolCount(symset) Glue(symset, _count)
#define SymbolBasePtr(symset) Glue(symset, _baseptr)
#define SymbolOplPtr(symset) (SymbolBasePtr(symset) + SymbolCount(symset))

#define SymbolMetadata(symset, name) (&Symbol__Sym(symset, name))
#define SymbolId(symset, name) SymbolIdFromMetadata(symset, SymbolMetadata(symset, name))

#define SymbolIdFromMetadata(symset, ptr)\
  (((SymbolBasePtr(symset) <= (ptr)) && ((ptr) <= SymbolOplPtr(symset))) ?\
   ((U32)(1 + ((ptr) - SymbolBasePtr(symset)))) : 0)

#define SymbolMetadataFromId(symset, id)\
  (((1 <= (id)) && ((id) <= SymbolCount(symset))) ?\
   (SymbolBasePtr(symset) + (id) - 1) : (SymbolMetadata(symset, 0)))

#define SymbolRaw(symset, name) IntFromPtr(SymbolMetadata(symset, name))
#define SymbolIdFromRaw(symset, raw)\
  SymbolIdFromMetadata(symset, (Symbol__Type(symset)*)(PtrFromInt(raw)))
#define SymbolRawFromId(symset, id) IntFromPtr(SymbolMetadataFromId(symset, id))

// -----------------------------------------------------------------------------
// internal

#define Symbol__Type(symset) Glue(symset, _Type)
#define Symbol__Sym(symset, name) Glue(symset, Glue(_, name))
#define Symbol__Section(symset) Glue(symset, _section)
