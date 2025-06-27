// NOTE: types
typedef struct XmlAttribute XmlAttribute;
struct XmlAttribute
{
  XmlAttribute *next;
  String8 name;
  String8 value;
};

typedef struct XmlNode XmlNode;
struct XmlNode
{
  XmlNode *first;
  XmlNode *last;
  XmlNode *next;
  XmlNode *prev;
  XmlNode *parent;
  U64 child_count;
  
  String8 label;
  
  XmlAttribute *first_attribute;
  XmlAttribute *last_attribute;
  U64 attribute_count;
  
  String8 body;
};

typedef struct ParsedXml
{
  XmlNode *root;
  U64 node_count;
} ParsedXml;

typedef struct XmlParseContext
{
  Buffer buffer;

  XmlNode *current_parent;
  ParsedXml *dest;
} XmlParseContext;

typedef struct XmlIterator
{
  XmlNode *next;
  S32 push_count;
  S32 pop_count;
} XmlIterator;

// NOTE: parse utility functions
proc XmlParseContext xml_parse_init(String8 s, ParsedXml *dest);
proc B32 xml_parse_is_valid(XmlParseContext *ctxt);
proc void xml_parse_advance(XmlParseContext *ctxt);
proc void xml_parse_advance_count(XmlParseContext *ctxt, U64 count);
proc U8 xml_parse_read_byte(XmlParseContext *ctxt);
proc void xml_parse_eat_whitespace(XmlParseContext *ctxt);
proc B32 xml_parse_expect_char(XmlParseContext *ctxt, char c);
proc B32 xml_parse_expect_str(XmlParseContext *ctxt, String8 s);
proc void xml_parse_seek_char(XmlParseContext *ctxt, char c);
proc void xml_parse_seek_str(XmlParseContext *ctxt, String8 s);
proc void xml_parse_seek_past_char(XmlParseContext *ctxt, char c);
proc void xml_parse_seek_past_str(XmlParseContext *ctxt, String8 s);

// NOTE: top-level parse functions
proc void xml_parse_header(XmlParseContext *ctxt);
proc void xml_parse_attribute(Arena *arena, XmlParseContext *ctxt, XmlNode *node);
proc void xml_parse_node(Arena *arena, XmlParseContext *ctxt);
proc ParsedXml xml_parse(Arena *arena, String8 loaded_xml);

// NOTE: xml tree traversal utilities
proc XmlIterator xml_iterate_depth_first_preorder(XmlNode *node);
proc XmlIterator xml_iterate_breadth_first_preorder(XmlNode *node);

// NOTE: printers
proc void print_parsed_xml(ParsedXml xml);
