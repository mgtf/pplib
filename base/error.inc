<?

////
// $Id: error.inc,v 1.17 2003/08/29 13:08:28 mgf Exp $
// !error handling
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//

if(DEVEL_MODE == TRUE)
  error_reporting(E_ALL);
else
  error_reporting(E_ALL ^ E_NOTICE);
set_error_handler('my_error_handler');

function my_error_handler($errno, $errstr, $errfile, $errline, $errcontext) {
  switch($errno) {
    case E_ERROR:
    case E_PARSE:
    case E_USER_ERROR:
    case E_CORE_ERROR:
      global $pp;
      // if an error occurs before $pp exists, just quit :
      if(! is_object($pp))
	die("-- "._("error")." --\n".$errstr."\n".__FILE__.' : '. __LINE__);
      echo '<table align=center border=0 cellspacing=0 cellpadding=5>'.
	   '<tr><td width="10" align="center"><img src="/icons/error.gif">'.
	   '<br><font size=2 color="#9C0000">'._("error").'</font></th>'.
           '<td><font size=2>'.nl2br($errstr).'</font>'.
	   '<br><font size=1>'._("file").' : '.basename($errfile).'&nbsp;'._("line").' : '.$errline.'</font>'.
	   '</td></tr>'.
	   '</table><br>';
      die();
      break;
    case E_WARNING:
    case E_NOTICE:
    case E_USER_NOTICE:
    case E_USER_WARNING:
    case E_CORE_WARNING:
      echo '<table align="center" border="0" cellspacing="0" cellpadding="5">'.
	   '<tr><td width="15" align="center"><img src="/icons/warning.gif">'.
	   '<br><font size=2 color="#0040BD">'._("warning").'</font></th>'.
           '<td><font size=2>'.nl2br($errstr).'</font>'.
	   '<br><font size=1>'._("file").' : '.basename($errfile).'&nbsp;'._("line").' : '.$errline.'</font>'.
	   '</td></tr>'.
	   '</table><br>';
      break;
    default:
      die("-- error --\n".__FILE__.' : '. __LINE__);
      break;
  }
}

?>
