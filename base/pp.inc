<?

////
//  $Id: pp.inc,v 1.1 2002/01/15 15:22:11 tieum Exp $
//  proportail main class
//
//  public methods :
//
//  private methods :
//
//  none
//
//  consts :
//
//  PP_MAJOR_VERSION
//  PP_MINOR_VERSION
//  PP_LIB_PATH
//  PP_LOG_NAME

// PP_LIB_PATH use by i18n : 
define('PP_LIB_PATH', '/home/tieum/public_html/proportail3/lib/');
define('PP_LOG_NAME', 'pplib');
define('PP_MAJOR_VERSION', '3');
define('PP_MINOR_VERSION', '0b');
// microsecond to sleep between each try
define('LOCK_SLEEP_TIME', '1000000');

// all classes extend this one :
require_once('pp_class.inc');

if(DEVEL_MODE)
{
  require_once('pp_microtime.inc');
  $timer = new pp_microtime;
  $timer->start();
}

require_once('pp_error.inc');
require_once('pp_output_buffering.inc');
require_once('pp_i18n.inc');
require_once('pp_tree.inc');
//
require_once('pp_mime.inc');
require_once('pp_header.inc');
require_once('pp_file.inc');
require_once('pp_data.inc');
require_once('pp_template.inc');
require_once('pp_mysql.inc');
require_once('pp_string.inc');
require_once('pp_user.inc');
require_once('pp_form.inc');
require_once('pp_list.inc');
require_once('pp_session.inc');
require_once('pp_xml_expat.inc');
require_once('pp_image.inc');
require_once('pp_ttf_text.inc');

class pp extends pp_class
{
  var $version;
  var $string;
  var $db;
  var $form;
  var $tpl;
  var $list;
  var $data;
  var $file;
  var $i18n;
  var $session;
  var $image;
  var $ob;
  var $header;

  // private : 
  var $no_parse;

  function pp()
  {
    $this->pp_class();
    new_($this->ob, 'pp_output_buffering', 'my_output_callback');
    new_($this->tpl, 'pp_template');
    new_($this->string, 'pp_string');
    new_($this->db, 'pp_db');
    new_($this->form, 'pp_form');
    new_($this->list, 'pp_list');
    new_($this->data, 'pp_data');
    new_($this->file, 'pp_file');
    new_($this->i18n, 'pp_i18n');
    new_($this->session, 'pp_session');
    new_($this->image, 'pp_image');
    new_($this->header, 'pp_header');
    $this->version = PP_MAJOR_VERSION.'.'.PP_MINOR_VERSION;
    $this->no_parse = FALSE;
  }

};

function my_output_callback($buffer)
{
  global $pp, $timer;

  // si $pp n'est pas définie ici. on quitte.
  if(! is_object($pp))
    return "-- output error --\n";
  //

  if(MAINTENANCE_MODE == 1)
  {
    if($pp->tpl->mime_subtype == 'html' && $this->no_parse == FALSE)
      return _("This site is currently in a maintenance state.").
	     '<br>'.
	     _("Please come back later.");
    else
      return '';
  }
  if($pp->no_parse == TRUE)
    return $buffer;
  $str = $pp->tpl->parse_template($buffer);
  $str = $pp->tpl->comment_start.
         " Proportail v".
         $pp->version.
         " - ".
         date("D M d Y").
         $pp->tpl->comment_end.
         "\n".
         $str;
  if($pp->tpl->mime_subtype == 'html')
    $str = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'."\n".$str;
  if(DEVEL_MODE)
  {
    $timer->stop();
    $elapsed = round($timer->elapsed(), 3);
    if($pp->tpl->mime_subtype == 'html')
      $str .= '<font size=1 face=verdana><center>'.
	      _("page generated on ").$elapsed.' sec </center></font>';
    else
      $str .= _("page generated on ").$elapsed." sec\n";
  }
  if(AUTO_STRIPSLASHES)
    $str = stripslashes($str);
  return $str;
}

function my_shutdown_function()
{
  // III netoyage, appel de destructeurs ..
  // system("rm -f /home/tieum/public_html/proportail3/cache/*");
  return;
}

register_shutdown_function('my_shutdown_function');
// création de l'objet $pp
$pp = new pp;
// démarrage de l'output buffering :
$pp->ob->start();
ob_implicit_flush(false);

?>
