<?

////
//  $Id: pp_string.inc,v 1.1 2002/01/15 15:22:12 tieum Exp $
//  string util class
//
//  public methods :
//
//  function short_text($text, $size, $end_string = '...')
//
//  private methods :
//
//  none
//
//  consts :
//
//  none


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
