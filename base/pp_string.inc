<?

////
// $Id: pp_string.inc,v 1.2 2002/09/18 11:58:45 tieum Exp $
// !string util class
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//
// public methods :
//
// function short_text($text, $size, $end_string = '...')
//
// private methods :
//
// none
//
// consts :
//
// none
//

class pp_string extends pp_class
{
  function pp_string()
  {
    $this->pp_class();
  }

  ////
  // !private function
  function cut_text($text, $size, $end_string = '...')
  {
    $length = strlen($text);
    if ($length < $size)
      return $text;
    return substr($text, 0, ($size - strlen($end_string)) ).$end_string;
  }

};

?>
