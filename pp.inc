<?

////
//  $Id: pp.inc,v 1.83 2002/10/31 11:16:51 tieum Exp $
//  !pplib main class, define pp object
//  Ce fichier est soumis à la licence d'utilisation 
//  contenue dans le fichier LICENCE
//
//  properties :
//  
//  no_parse : set it to FALSE for not to parse output
//             before send it back to the browser
//  header_str, footer_str : header/footer add to the output.
//  compress_data : whether we should use output compression or not.
//
//  public methods :
//  none
//
//  private methods :
//  none
//
//  consts :
//
//  PP_MAJOR_VERSION
//  PP_MINOR_VERSION
//  GZ_HEADER
//  GZ_COMPRESSION_LEVEL
//  DEVEL_MODE
//  MAINTENANCE_MODE
//  AUTO_STRIPSLASHES

// do not modify below
include('conf/pplib.conf');
define('PP_MAJOR_VERSION', '3');
define('PP_MINOR_VERSION', '3d');
define('GZ_HEADER', "\x1f\x8b\x08\x00\x00\x00\x00\x00");
define('GZ_COMPRESSION_LEVEL', 9);

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
require_once('base/pp_cache.inc');
require_once('base/pp_template.inc');
require_once('base/pp_mysql.inc');
require_once('base/pp_string.inc');
require_once('base/pp_user.inc');
require_once('base/pp_form.inc');
require_once('base/pp_session.inc');
require_once('base/pp_xml_expat.inc');
require_once('base/pp_image.inc');
require_once('base/pp_ttf_text.inc');
require_once('base/pp_mail.inc');
require_once('list/pp_list.inc');

class pp extends pp_class
{
  var $version;
  var $string;
  var $db;
  var $form;
  var $tpl;
  var $list;
  var $file;
  var $i18n;
  var $session;
  var $image;
  var $ttf;
  var $ob;
  var $header;
  var $mail;

  var $header_str;
  var $footer_str;
  var $no_parse;
  var $compress_data;

  function pp()
  {
    $this->pp_class();
    new_($this->ob, 'pp_output_buffering', 'my_output_callback');
    new_($this->tpl, 'pp_template');
    new_($this->string, 'pp_string');
    new_($this->db, 'pp_db');
    new_($this->form, 'pp_form');
    new_($this->list, 'pp_list');
    new_($this->file, 'pp_file');
    new_($this->i18n, 'pp_i18n');
    new_($this->session, 'pp_session');
    new_($this->image, 'pp_image');
    new_($this->ttf, 'pp_ttf_text');
    new_($this->header, 'pp_header');
    new_($this->mail, 'pp_mail');
    $this->version = PP_MAJOR_VERSION.'.'.PP_MINOR_VERSION;
    $this->no_parse = FALSE;
    $this->header_str = '';
    $this->footer_str = '';
    $this->ob->start();
    $this->compress_data = FALSE;
    ob_implicit_flush(FALSE);
  }
};

function my_shutdown_function()
{
  // XXX cleanup, call destructor there ..
  // system("rm -f /path/to/cache/*");
  return;
}

function my_output_callback($buffer)
{
  global $timer, $pp;

  $str = $buffer;

  if($pp->no_parse == TRUE)
    return $str;

  if(MAINTENANCE_MODE == 1)
  {
    if($pp->tpl->mime_subtype == 'html' || $pp->tpl->mime_subtype == 'xhtml')
      return 
	_("this application is currently in a maintenance state").
	$pp->tpl->return_char.
	_("Please come back later");
    else
      return '';
  }
  $str = $pp->tpl->comment_start.
    " pplib v".
    $pp->version.
    " - ".
    date("D M d Y")." ".
    $pp->tpl->comment_end.
    "\n".
    $pp->header_str.
    $str;
  switch($pp->tpl->mime_subtype)
  {
    case 'html':
      $str = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'."\n".$str;
      break;
    case 'xhtml':
      $str = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">'."\n".$str;
      break;
    case 'xml':
      $str = '<?xml version="1.0"?>'."\n".$str;
      break;
  }
  if(DEVEL_MODE)
  {
    $timer->stop('full_page_parse_time');
    $elapsed = round($timer->elapsed('full_page_parse_time'), 3);
    if($pp->tpl->mime_subtype == 'html' || $pp->tpl->mime_subtype == 'xhtml')
      $str .= 
	$pp->tpl->return_char.'<div align="center"><font size="1" face="Verdana">'.
	_("page generated on ").$elapsed.' sec '.
	'(pplib v'.$pp->version.')</font></div>';
    else
      $str .= 
	_("page generated on ").$elapsed.' sec '.
	'(pplib v'.$pp->version.')'."\n";
  }
  $str = $str.$pp->footer_str;
  $str = $pp->tpl->parse_template($str);
  if(AUTO_STRIPSLASHES)
    $str = stripslashes($str);
  // XXX - this reduce about 13% datasize send
  if($pp->compress_data && ! DEVEL_MODE)
  {
    $str = str_replace("\n", "", $str);
    $str = str_replace("\r", "", $str);
    $str = str_replace("\t", " ", $str);
    $str = preg_replace("/ ( )+/", " ", $str);
  }
  if($pp->compress_data && ($pp->ob->get_compression() != 'none') )
  {
    $pp->ob->compress_add_headers();
    $str = $pp->ob->compress($str);
  }
  $pp->header->header_add("X-Powered-By: pplib ".$pp->version);
  $pp->header->header_send();
  return $str;
}

register_shutdown_function('my_shutdown_function');
$pp = new pp;

?>
