<?

////
//  $Id: pp.inc,v 1.33 2002/02/26 17:13:27 tieum Exp $
//  pplib main class
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
//  PP_SYSLOG_NAME

define('PP_LIB_PATH', '/home/tieum/pplight/pplib/');
define('PP_SYSLOG_NAME', '[ PPLIB ]');
define('PP_MAJOR_VERSION', '3');
define('PP_MINOR_VERSION', '1b');

// all classes extend this one :
require_once('base/pp_class.inc');

if(! defined('DEVEL_MODE'))
  define('DEVEL_MODE', FALSE);
if(DEVEL_MODE)
{
  require_once('base/pp_microtime.inc');
  $timer = new pp_microtime;
  $timer->start('full_page_parse_time');
}

require_once('base/pp_error.inc');
require_once('base/pp_output_buffering.inc');
require_once('base/pp_i18n.inc');
require_once('base/pp_sniff.inc');
require_once('base/pp_tree.inc');
require_once('base/pp_mime.inc');
require_once('base/pp_header.inc');
require_once('base/pp_file.inc');
require_once('base/pp_data.inc');
require_once('base/pp_template.inc');
require_once('base/pp_mysql.inc');
require_once('base/pp_string.inc');
require_once('base/pp_user.inc');
require_once('base/pp_form.inc');
require_once('base/pp_session.inc');
require_once('base/pp_xml_expat.inc');
require_once('base/pp_image.inc');
require_once('base/pp_ttf_text.inc');
require_once('base/pp_wddx.inc');
require_once('list/pp_list.inc');

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
  var $ttf;
  var $ob;
  var $header;

  var $header_str;
  var $footer_str;

  // private : 
  var $no_parse;

  function pp()
  {
    register_shutdown_function(array($this, 'my_shutdown_function'));
    $this->pp_class();
    new_($this->ob, 'pp_output_buffering', array($this, 'my_output_callback'));
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
    new_($this->ttf, 'pp_ttf_text');
    new_($this->header, 'pp_header');
    $this->version = PP_MAJOR_VERSION.'.'.PP_MINOR_VERSION;
    $this->no_parse = FALSE;
    $this->header_str = '';
    $this->footer_str = '';

    $this->ob->start();
    ob_implicit_flush(false);
  }

  function my_shutdown_function()
  {
    // XXX cleanup, call destructor there ..
    // system("rm -f /home/tieum/public_html/proportail3/cache/*");
    return;
  }

  function my_output_callback($buffer)
  {
    global $pp, $timer;

    $str = $buffer;

    if($pp->no_parse == TRUE)
      return $str;

    // continue only if $pp is define at 
    // this time since we need $pp after;
    if(! is_object($pp))
      return "-- output error --\n";
    if(MAINTENANCE_MODE == 1)
    {
      if($pp->tpl->mime_subtype == 'html')
	return _("This site is currently in a maintenance state.").
	  '<br>'.
	  _("Please come back later.");
      else
	return '';
    }
    $str = $pp->tpl->comment_start.
      " pplib v".
      $pp->version.
      " - ".
      date("D M d Y").
      $pp->tpl->comment_end.
      "\n".
      $pp->header_str.
      $str;
    if($pp->tpl->mime_subtype == 'html')
      $str = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'."\n".$str;
    if(DEVEL_MODE)
    {
      $timer->stop('full_page_parse_time');
      $elapsed = round($timer->elapsed('full_page_parse_time'), 3);
      if($pp->tpl->mime_subtype == 'html')
	$str .= '
	  <font size=1 face=verdana><center>'.
	  _("page generated on ").$elapsed.' sec '.
	  '(pplib version : '.$pp->version.')</center></font>';
      else
	$str .= 
	  _("page generated on ").$elapsed.' sec '.
	  '(pplib version : '.$pp->version.')'."\n";
    }
    $str = $str.$pp->footer_str;
    $str = $pp->tpl->parse_template($str);
    if(AUTO_STRIPSLASHES)
      $str = stripslashes($str);
    return $str;
  }
};

$pp = new pp;

?>
