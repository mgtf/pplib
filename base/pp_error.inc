<?
// $Id: pp_error.inc,v 1.2 2002/01/16 11:49:42 tieum Exp $

if(DEVEL_MODE == TRUE)
  error_reporting(E_ALL);
else
  error_reporting(E_ALL ^ E_NOTICE);
set_error_handler('my_error_handler');

function my_error_handler($errno, $errstr, $errfile, $errline)
{
  switch($errno)
  {
    case E_ERROR:
    case E_PARSE:
    case E_USER_ERROR:
    case E_CORE_ERROR:
      global $pp;
      // if error occurs before $pp exists, just quit :
      if(! is_object($pp))
	die("-- error --\n".__FILE__.' : '. __LINE__);
      $pp->ob->suspend();
      echo '<table align=center border=1 cellspacing=0 cellpadding=2>'.
	   '<tr><th bgcolor="FFCCCC"><font size=1 color="red">'._("error").'</font></th></tr>'.
           '<tr><td align=center><font size=1>'.nl2br($errstr).'</font></td></tr>'.
           '<tr><td align=center><font size=1>'.
	   '<b>'._("file").' : </b>'.$errfile.'&nbsp;<b>'._("line").' : </b>'.$errline.'</font>'.
	   '</td></tr>'.
	   '</table><br>';
      $errstr = $pp->ob->resume();
      $pp->ob->clean();
      die($errstr);
      break;
    case E_WARNING:
    case E_NOTICE:
    case E_USER_NOTICE:
    case E_USER_WARNING:
    case E_CORE_WARNING:
      echo '<table align=center border=1 cellspacing=0 cellpadding=2>'.
	   '<tr><th bgcolor="FFCCCC"><font size=1 color="green">'._("warning").'</font></th></tr>'.
           '<tr><td align=center><font size=1>'.nl2br($errstr).'</font></td></tr>'.
           '<tr><td align=center><font size=1>'.
	   '<b>'._("file").' : </b>'.$errfile.'&nbsp;<b>'._("line").' : </b>'.$errline.'</font>'.
	   '</td></tr>'.
	   '</table><br>';
      break;
    default:
      echo "-- error --\n".__FILE__.' : '. __LINE__;
      break;
  }
}

?>
