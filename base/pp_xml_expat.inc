<?

// $Id: pp_xml_expat.inc,v 1.2 2002/02/11 13:38:47 tieum Exp $

class pp_xml_expat extends pp_template
{
  var $parser;
  var $env;
  var $buffer;
  var $level;

  function pp_xml_expat()
  {
    $this->pp_template();
    $this->buffer = '';
    $this->level = 0;
    $this->parser = xml_parser_create();
    if(! is_resource($this->parser))
      $this->error("pp_xml_expat::pp_xml_expat : "._("Can't create XML parser"));
    xml_parser_set_option($this->parser, XML_OPTION_CASE_FOLDING, 0);
    // xml_parser_set_option($this->parser, XML_OPTION_SKIP_WHITE, 1);
    // handlers :
    xml_set_element_handler($this->parser, "h_start_el", "h_end_el");
    xml_set_character_data_handler($this->parser, "h_char_data");
    xml_set_processing_instruction_handler($this->parser, "h_pi");
    xml_set_default_handler($this->parser, "h_default");
    xml_set_unparsed_entity_decl_handler($this->parser, "h_unparsed_entity_decl");
    xml_set_notation_decl_handler($this->parser, "h_notation_decl");
    xml_set_external_entity_ref_handler($this->parser, "h_external_entity_ref");
    //
  }

  function parse($data)
  {
    xml_set_object($this->parser, &$this);
    $data = ereg_replace("\ \ +", "", $data);
    $data = ereg_replace("\n", "", $data);
    if( (xml_parse($this->parser, $data)) == FALSE )
    {
      $err = "pp_xml_expat::parse_xml_stream : "._("XML parse error")."<br>";
      $err .= xml_error_string(xml_get_error_code($this->parser))."<br>";
      $err .= _("line").' :'.xml_get_current_line_number($this->parser);
      $this->error($err);
    }
    return strlen($this->buffer);
  }

  function free()
  {
    xml_parser_free($this->parser);
    $this->buffer = '';
  }

  function h_start_el($parser, $name, $attrs)
  {
    $this->level++;
    $this->buffer .= ' {'.strtolower($name);
      while(list($k, $v) = each($attrs))
	$this->buffer .= ' '.$k.'="'.$v.'"';
  }

  function h_end_el($parser, $name)
  {
    $this->buffer .= "}\n";
    $this->level--;
  }

  function h_char_data($parser, $data)
  {
    if(strlen($data))
      $this->buffer .= ' data="'.$data.'" level="'.$this->level.'"';
  }

  function h_pi($parser, $target, $data) { }
  function h_default($parser, $data) { }
  function h_unparsed_entity_decl($parser, $data) { }
  function h_notation_decl($parser, $notationname, $base, $systemid, $publicid) { }
  function h_external_entity_ref($parser, $openentitynames, $base, $systemid, $publicid) { }
};

?>
