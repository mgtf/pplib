<? 
////
// $Id: header.inc,v 1.4 2003/08/28 15:22:44 mgf Exp $
// ! headers
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE


class header extends mime
{
  var $headers;

  function header()
  {
    $this->mime();
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
