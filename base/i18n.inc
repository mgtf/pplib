<?

////
// $Id: i18n.inc,v 1.5 2003/08/28 15:22:44 mgf Exp $
// !i18n
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE

class i18n extends _stdclass
{
  var $language;

  function i18n()
  {
    $this->_stdclass();
    if(defined('LANGUAGE'))
      $this->language = LANGUAGE;
    else
      $this->language = 'C';
    $this->set_language();
    return 1;
  }

  function set_language()
  {
    chdir(LIBPATH);
    putenv('LC_ALL=' . $this->language);
    if (@setlocale(LC_ALL, $this->language) == NULL)
      user_error("i18n::set_language : invalid language '".$this->language."'", E_USER_ERROR);
    bindtextdomain('lib', 'locale');
    textdomain('lib');
    return 1;
  }
};

?>
