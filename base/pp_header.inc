<? 
// $Id: pp_header.inc,v 1.1 2002/01/15 15:22:11 tieum Exp $

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
    if(headers_sent() == FALSE)
      foreach($this->headers as $header)
	header($header);
  }
};

?>
