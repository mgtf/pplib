<?

////
// $Id: xml_expat.inc,v 1.8 2003/09/01 12:28:20 mgf Exp $
// !xml parser class
// Ce fichier est soumis � la licence d'utilisation
// contenue dans le fichier LICENCE
//

class xml_expat extends template {
  var $parser;
  var $env;
  var $buffer;
  var $level;

  function xml_expat() {
    $this->template();
    $this->buffer = '';
    $this->level = 0;
    $this->parser = xml_parser_create();
    if(! is_resource($this->parser))
      user_error("xml_expat::xml_expat : "._("Can't create XML parser"), E_USER_ERROR);
    xml_parser_set_option($this->parser, XML_OPTION_CASE_FOLDING, 0);
    // xml_parser_set_option($this->parser, XML_OPTION_SKIP_WHITE, 1);
    xml_set_element_handler($this->parser, "h_start_el", "h_end_el");
    xml_set_character_data_handler($this->parser, "h_char_data");
    xml_set_processing_instruction_handler($this->parser, "h_pi");
    xml_set_default_handler($this->parser, "h_default");
    xml_set_unparsed_entity_decl_handler($this->parser, "h_unparsed_entity_decl");
    xml_set_notation_decl_handler($this->parser, "h_notation_decl");
    xml_set_external_entity_ref_handler($this->parser, "h_external_entity_ref");
  }

  function parse($data) {
    xml_set_object($this->parser, &$this);
    if( (xml_parse($this->parser, $data)) == FALSE ) {
      $err = "xml_expat::parse_xml_stream : "._("XML parse error")."<br>".
	     xml_error_string(xml_get_error_code($this->parser))."<br>".
	      _("line").' :'.xml_get_current_line_number($this->parser);
      user_error($err, E_USER_ERROR);
    }
    return strlen($this->buffer);
  }

  function free() {
    xml_parser_free($this->parser);
    $this->buffer = '';
  }

  function h_start_el($parser, $name, $attrs) { }
  function h_end_el($parser, $name) { }
  function h_char_data($parser, $data) { }
  function h_pi($parser, $target, $data) { }
  function h_default($parser, $data) { }
  function h_unparsed_entity_decl($parser, $data) { }
  function h_notation_decl($parser, $notationname, $base, $systemid, $publicid) { }
  function h_external_entity_ref($parser, $openentitynames, $base, $systemid, $publicid) { }
};

?>
