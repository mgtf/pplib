<?

////
// $Id: pp_string.inc,v 1.3 2002/10/28 15:10:19 tieum Exp $
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
  function cut_text($str, $size, $end_string = '...')
  {
    $length = strlen($str);
    if ($length < $size)
      return $str;
    return substr($str, 0, ($size - strlen($end_string)) ).$end_string;
  }

  function remove_accent($str)
  {
    return strtr(
	$str, 
	"¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ", 
	"YuAAAAAAACEEEEIIIIDNOOOOOOUUUUYsaaaaaaaceeeeiiiionoooooouuuuyy"
	);
  }

};

?>
