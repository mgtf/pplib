<?

////
// $Id: string.inc,v 1.4 2003/08/28 15:22:44 mgf Exp $
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

class string extends _stdclass {
  function string() {
    $this->_stdclass();
  }

  ////
  // !private function
  function cut_text($str, $size, $end_string = '...') {
    $length = strlen($str);
    if ($length < $size)
      return $str;
    return substr($str, 0, ($size - strlen($end_string)) ).$end_string;
  }

  function remove_accent($str) {
    return strtr(
	$str, 
	"¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ", 
	"YuAAAAAAACEEEEIIIIDNOOOOOOUUUUYsaaaaaaaceeeeiiiionoooooouuuuyy"
	);
  }
};

?>
