<? 
// $Id: pp_header.inc,v 1.2 2002/02/28 11:34:18 tieum Exp $

class pp_header extends pp_mime
{
  var $headers;

  function pp_header()
  {
    $this->pp_mime();
    $this->headers = array();
  }

  function header_add($header_str = '')
  {
    global $pp;
    if(strlen($header_str))
      $this->headers[] = $header_str;
  }

  function header_send()
  {
    foreach($this->headers as $header)
      header($header);
  }
};

?>
