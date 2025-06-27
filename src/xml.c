proc XmlParseContext
xml_parse_init(String8 s, ParsedXml *dest)
{
  XmlParseContext result = {0};
  result.buffer.size = s.count;
  result.buffer.mem = s.string;
  result.current_parent = 0;
  result.dest = dest;
  
  xml_parse_header(&result);
  xml_parse_eat_whitespace(&result);
  return(result);
}

proc B32
xml_parse_is_valid(XmlParseContext *ctxt)
{
  B32 result = ctxt->buffer.size > 0;
  return(result);
}

proc void
xml_parse_advance(XmlParseContext *ctxt)
{  
  if(ctxt->buffer.size > 0) {
    ++ctxt->buffer.mem;
    --ctxt->buffer.size;
  }
}

proc void
xml_parse_advance_count(XmlParseContext *ctxt, U64 count)
{
  while(xml_parse_is_valid(ctxt) && count--) {
    xml_parse_advance(ctxt);
  }
}

proc U8
xml_parse_read_byte(XmlParseContext *ctxt)
{
  U8 result = 0;
  if(xml_parse_is_valid(ctxt)) {
    result = *ctxt->buffer.mem;
  }
  
  return(result);
}

proc void
xml_parse_eat_whitespace(XmlParseContext *ctxt)
{
  while(xml_parse_is_valid(ctxt) &&
	is_whitespace(xml_parse_read_byte(ctxt))) {
    xml_parse_advance(ctxt);
  }
}

proc B32
xml_parse_expect_char(XmlParseContext *ctxt, char c)
{
  U8 current_byte = xml_parse_read_byte(ctxt);
  B32 result = current_byte == c;
  if(result) {    
    xml_parse_advance(ctxt);
  }

  return(result);
}

proc B32
xml_parse_expect_str(XmlParseContext *ctxt, String8 s)
{
  String8 to_check = {.string = ctxt->buffer.mem, s.count};
  B32 result = str8s_are_equal(to_check, s);
  if(result) {
    xml_parse_advance_count(ctxt, s.count);
  }

  return(result);
}

proc void
xml_parse_seek_char(XmlParseContext *ctxt, char c)
{
  while(xml_parse_is_valid(ctxt) &&
	xml_parse_read_byte(ctxt) != c) {
    xml_parse_advance(ctxt);
  }
}

proc void
xml_parse_seek_str(XmlParseContext *ctxt, String8 s)
{
  String8 to_check = {.string = ctxt->buffer.mem, .count = s.count};
  while(xml_parse_is_valid(ctxt) &&
	!str8s_are_equal(to_check, s)) {
    xml_parse_advance_count(ctxt, s.count);
    to_check.string = ctxt->buffer.mem;
  }
}

proc void
xml_parse_seek_past_char(XmlParseContext *ctxt, char c)
{
  xml_parse_seek_char(ctxt, c);
  xml_parse_advance(ctxt);
}

proc void
xml_parse_seek_past_str(XmlParseContext *ctxt, String8 s)
{
  xml_parse_seek_str(ctxt, s);
  xml_parse_advance_count(ctxt, s.count);
}

proc void
xml_parse_header(XmlParseContext *ctxt)
{
  // NOTE: don't care about the header
  if(xml_parse_expect_str(ctxt, Str8Lit("<?"))) {
    xml_parse_seek_past_str(ctxt, Str8Lit("?>"));
  }
}

proc void
xml_parse_attribute(Arena *arena, XmlParseContext *ctxt, XmlNode *node)
{
  XmlAttribute *new_attribute = arena_push_struct_z(arena, XmlAttribute);

  U8 *attribute_name_start_at = ctxt->buffer.mem;
  xml_parse_seek_char(ctxt, '=');

  U8 *attribute_name_end_at = ctxt->buffer.mem;
  String8 attribute_name = {.string = attribute_name_start_at,
			    .count = IntFromPtr(attribute_name_end_at) - IntFromPtr(attribute_name_start_at)};
  new_attribute->name = arena_push_str8_copy(arena, attribute_name);

  xml_parse_advance(ctxt);

  xml_parse_expect_char(ctxt, '"');
  U8 *attribute_value_start_at = ctxt->buffer.mem;
  xml_parse_seek_char(ctxt, '"');

  U8 *attribute_value_end_at = ctxt->buffer.mem;
  String8 attribute_value = {.string = attribute_value_start_at,
			     .count = IntFromPtr(attribute_value_end_at) - IntFromPtr(attribute_value_start_at)};
  new_attribute->value = arena_push_str8_copy(arena, attribute_value);
  
  xml_parse_advance(ctxt);

  SLLQueuePush(node->first_attribute, node->last_attribute, new_attribute);
  ++node->attribute_count;
}

proc void
xml_parse_node(Arena *arena, XmlParseContext *ctxt)
{
  XmlNode *new_node = arena_push_struct_z(arena, XmlNode);

  U8 *label_start_at = ctxt->buffer.mem;
  while(!(xml_parse_read_byte(ctxt) == ' ' || xml_parse_read_byte(ctxt) == '>')) {
    xml_parse_advance(ctxt);
  }

  U8 *label_end_at = ctxt->buffer.mem;
  String8 label = {.string = label_start_at,
		   .count = IntFromPtr(label_end_at) - IntFromPtr(label_start_at)};  
  new_node->label = arena_push_str8_copy(arena, label);

  xml_parse_eat_whitespace(ctxt);

  // NOTE: parse attributes
  B32 is_terminal = 0;
  while(!xml_parse_expect_char(ctxt, '>')) {
    xml_parse_attribute(arena, ctxt, new_node);

    if(xml_parse_expect_char(ctxt, '/') &&
       xml_parse_expect_char(ctxt, '>')) {
      is_terminal = 1;
      break;
    }

    xml_parse_eat_whitespace(ctxt);
  }

  // NOTE: insert new node into tree
  new_node->parent = ctxt->current_parent;
  if(ctxt->current_parent) {
    DLLPushBack(ctxt->current_parent->first, ctxt->current_parent->last, new_node);
    ++ctxt->current_parent->child_count;
    if(!is_terminal) {
      SLLStackPush_N(ctxt->current_parent, new_node, parent);
    }
  }
  else {
    ctxt->current_parent = ctxt->dest->root = new_node;
  }
  ++ctxt->dest->node_count;
}

proc ParsedXml
xml_parse(Arena *arena, String8 loaded_xml)
{
  ParsedXml result = {0};

  for(XmlParseContext ctxt = xml_parse_init(loaded_xml, &result);
      xml_parse_is_valid(&ctxt);
      xml_parse_eat_whitespace(&ctxt)) {
    if(xml_parse_expect_char(&ctxt, '<')) {
      if(xml_parse_expect_char(&ctxt, '/')) {	
	if(xml_parse_expect_str(&ctxt, ctxt.current_parent->label)) {
	  // NOTE: we have hit a node terminator, so pop the current parent
	  SLLStackPop_N(ctxt.current_parent, parent);
	  xml_parse_expect_char(&ctxt, '>');
	}
      }
      else if(xml_parse_expect_char(&ctxt, '!')) {
	// NOTE: ignore comment
	xml_parse_seek_past_char(&ctxt, '>');
      }
      else {
	xml_parse_node(arena, &ctxt);
      }
    }
    else if(is_letter(xml_parse_read_byte(&ctxt))) {
      // NOTE: body of the current parent
      // TODO: the body can be utf8, so we should treat it as such
      U8 *body_start_at = ctxt.buffer.mem;      
      U8 *body_last_newline_at = body_start_at;
      while(xml_parse_read_byte(&ctxt) != '<') {
	if(xml_parse_read_byte(&ctxt) == '\n') {
	  body_last_newline_at = ctxt.buffer.mem;
	}
	xml_parse_advance(&ctxt);
      }

      //String8 body = {.string = body_start_at, .count = body_last_newline_pos - body_start_count};
      String8 body = {.string = body_start_at,
		      .count = IntFromPtr(body_last_newline_at) - IntFromPtr(body_start_at)};
      ctxt.current_parent->body = arena_push_str8_copy(arena, body);
    }
  }
  
  return(result);
}

proc XmlIterator
xml_iterate_depth_first_preorder(XmlNode *node)
{
  XmlIterator result = {0};
  if(node->first) {
    result.next = node->first;
    result.push_count = 1;
  }
  else {
    for(XmlNode *n = node; n; n = n->parent) {
      if(n->next) {
	result.next = n->next;
	break;
      }

      ++result.pop_count;
    }    
  }

  return(result);
}


proc XmlIterator
xml_iterate_breadth_first_preorder(XmlNode *node)
{
  XmlIterator result = {0};
  if(node->next) {
    result.next = node->next;
  }
  else {
    if(node->parent) {
      for(XmlNode *n = node->parent->first; n; n = n->next) {
	if(n->first) {
	  result.next = n->first;
	  ++result.push_count;
	  break;
	}
      }      
    }
    else {
      result.next = node->first;
      ++result.push_count;
    }        
  }

  return(result);
}

proc void
print_parsed_xml(ParsedXml xml)
{  
  S32 depth = 0;
  XmlIterator iter = {0};
  for(XmlNode *node = xml.root;
      node;
      iter = xml_iterate_depth_first_preorder(node), node = iter.next) {
    depth += iter.push_count;
    depth -= iter.pop_count;

    for(S32 i = 0; i < depth; ++i) {
      printf("|");
    }
    printf("label=%.*s", (int)node->label.count, node->label.string);
    for(XmlAttribute *attr = node->first_attribute; attr; attr = attr->next) {
      printf(", %.*s=%.*s", (int)attr->name.count, attr->name.string, (int)attr->value.count, attr->value.string);
    }
    /* if(node->body.count) { */
    /*   printf("\n%.*s", (int)node->body.count, node->body.string); */
    /* } */
    printf("\n");
  }
}
