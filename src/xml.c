
proc XmlParseContext
xml_parse_init(String8 s, ParsedXml *dest)
{
  XmlParseContext result = {.at = s.string,
			    .count = 0,
			    .len = s.count,
			    .current_parent = 0,
			    .dest = dest};
  xml_parse_header(&result);
  xml_parse_eat_whitespace(&result);
  return(result);
}

proc B32
xml_parse_is_valid(XmlParseContext ctxt)
{
  B32 result = ctxt.count < ctxt.len;
  return(result);
}

proc void
xml_parse_advance(XmlParseContext *ctxt)
{
  ++ctxt->at;
  ++ctxt->count;
}

proc void
xml_parse_advance_count(XmlParseContext *ctxt, U64 count)
{
  while(count--) {
    xml_parse_advance(ctxt);
  }
}

proc void
xml_parse_eat_whitespace(XmlParseContext *ctxt)
{
  while(is_whitespace(*ctxt->at)) {
    xml_parse_advance(ctxt);
  }
}

proc B32
xml_parse_expect_char(XmlParseContext *ctxt, char c)
{
  B32 result = *ctxt->at == c;
  if(result) {    
    xml_parse_advance(ctxt);
  }

  return(result);
}

proc B32
xml_parse_expect_str(XmlParseContext *ctxt, String8 s)
{
  String8 to_check = {.string = ctxt->at, s.count};
  B32 result = str8s_are_equal(to_check, s);
  if(result) {
    xml_parse_advance_count(ctxt, s.count);
  }

  return(result);
}

proc void
xml_parse_seek_char(XmlParseContext *ctxt, char c)
{
  while(*ctxt->at != c) {
    xml_parse_advance(ctxt);
  }
}

proc void
xml_parse_seek_str(XmlParseContext *ctxt, String8 s)
{
  String8 to_check = {.string = ctxt->at, .count = s.count};
  while(!str8s_are_equal(to_check, s)) {
    xml_parse_advance_count(ctxt, s.count);
    to_check.string = ctxt->at;
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

  U8 *attribute_name_start_at = ctxt->at;
  U64 attribute_name_start_count = ctxt->count;
  xml_parse_seek_char(ctxt, '=');

  new_attribute->name.string = attribute_name_start_at;
  new_attribute->name.count = ctxt->count - attribute_name_start_count;

  xml_parse_advance(ctxt);

  xml_parse_expect_char(ctxt, '"');
  U8 *attribute_value_start_at = ctxt->at;
  U64 attribute_value_start_count = ctxt->count;
  xml_parse_seek_char(ctxt, '"');
  
  new_attribute->value.string = attribute_value_start_at;
  new_attribute->value.count = ctxt->count - attribute_value_start_count;

  xml_parse_advance(ctxt);

  SLLQueuePush(node->first_attribute, node->last_attribute, new_attribute);
  ++node->attribute_count;
}

proc void
xml_parse_node(Arena *arena, XmlParseContext *ctxt)
{
  XmlNode *new_node = arena_push_struct_z(arena, XmlNode);

  U8 *label_start_at = ctxt->at;
  U64 label_start_count = ctxt->count;
  //xml_parse_seek_char(ctxt, ' ');
  while(!(*ctxt->at == ' ' || *ctxt->at == '>')) {
    xml_parse_advance(ctxt);
  }

  new_node->label.string = label_start_at;
  new_node->label.count = ctxt->count - label_start_count;

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

  U64 parsed_count = 0; // NOTE: DEBUG
  for(XmlParseContext ctxt = xml_parse_init(loaded_xml, &result);
      xml_parse_is_valid(ctxt);
      xml_parse_eat_whitespace(&ctxt)) {
    if(xml_parse_expect_char(&ctxt, '<')) {
      if(xml_parse_expect_char(&ctxt, '/')) {
	// NOTE: DEBUG
	{
	  String8 protocol_str = Str8Lit("protocol");
	  String8 to_match = {.string = ctxt.at, .count = protocol_str.count};
	  if(str8s_are_equal(to_match, protocol_str)) {
	    fprintf(stderr, "CLOSING ROOT");
	  }
	}
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
    else if(is_letter(*ctxt.at)) {
      // NOTE: body of the current parent
      // TODO: the body can be utf8, so we should treat it as such
      U8 *body_start_at = ctxt.at;
      U64 body_start_count = ctxt.count;
      U64 body_last_newline_pos = body_start_count;
      while(*ctxt.at != '<') {
	if(*ctxt.at == '\n') {
	  body_last_newline_pos = ctxt.count;
	}
	xml_parse_advance(&ctxt);
      }

      ctxt.current_parent->body.string = body_start_at;
      ctxt.current_parent->body.count = body_last_newline_pos - body_start_count;
    }

    parsed_count = ctxt.count; // NOTE: DEBUG
  }

  fprintf(stderr, "parsed %lu bytes\n", parsed_count); // NOTE: DEBUG
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
