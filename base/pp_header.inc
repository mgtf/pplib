<? 
////
// $Id: pp_header.inc,v 1.3 2002/09/18 11:58:44 tieum Exp $
// ! headers
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE


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
