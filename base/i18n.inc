<?

////
// $Id: i18n.inc,v 1.3 2002/09/18 11:58:44 tieum Exp $
// !i18n
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE

class pp_i18n extends pp_class
{
  var $language;

  function pp_i18n()
  {
    $this->pp_class();
    if(defined('LANGUAGE'))
      $this->language = LANGUAGE;
    else
      $this->language = 'C';
    $this->set_language();
    return 1;
  }

  function set_language()
  {
    chdir(PP_LIB_PATH);
    putenv('LC_ALL=' . $this->language);
    if (@setlocale(LC_ALL, $this->language) == NULL)
      user_error("pp_i18n::set_language : invalid language '".$this->language."'", E_USER_ERROR);
    bindtextdomain('pp_lib', 'locale');
    textdomain('pp_lib');
    return 1;
  }
};

?>
