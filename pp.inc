<?

////
//  $Id: pp.inc,v 1.91 2003/08/29 08:38:24 mgf Exp $
//  !ocms main class, it define pp object
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
//  LIBNAME
//  MAJOR_VERSION
//  MINOR_VERSION
//  GZ_HEADER
//  GZ_COMPRESSION_LEVEL
//  DEVEL_MODE
//  MAINTENANCE_MODE
//  AUTO_STRIPSLASHES

// do not modify below
define('LIBNAME', 'ocms');
define('SYSLOG_PREFIX', '[ '.LIBNAME.' ]');
define('SERVER_ADMIN', $_SERVER['SERVER_ADMIN']);
define('MAGIC_MIME_FILE', '/usr/share/misc/file/magic.mime');
define('LIBPATH', dirname(__FILE__).'/');
define('MAJOR_VERSION', '3');
define('MINOR_VERSION', '4a');
define('GZ_HEADER', "\x1f\x8b\x08\x00\x00\x00\x00\x00");
define('GZ_COMPRESSION_LEVEL', 9);

// all classes extend this one :
require_once('base/class.inc');

if(! defined('DEVEL_MODE'))
  define('DEVEL_MODE', false);
if(DEVEL_MODE) {
  require_once('base/microtime.inc');
  $timer = new microtime;
  $timer->start('full_page_parse_time');
}

require_once('base/error.inc');
require_once('base/output_buffering.inc');
require_once('base/i18n.inc');
require_once('base/sniff.inc');
require_once('base/tree.inc');
require_once('base/mime.inc');
require_once('base/header.inc');
require_once('base/file.inc');
require_once('base/cache.inc');
require_once('base/cache_http_request.inc');
require_once('base/template.inc');
require_once('base/mysql.inc');
require_once('base/string.inc');
require_once('base/user.inc');
require_once('base/form.inc');
require_once('base/session.inc');
require_once('base/xml_expat.inc');
require_once('base/image.inc');
require_once('base/ttf_text.inc');
require_once('base/mail.inc');
// require_once('list/list.inc');

class pp extends _stdclass {
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

  function pp() {
    $this->_stdclass();
    new_($this->ob, 'output_buffering', 'my_output_callback');
    new_($this->tpl, 'template');
    new_($this->string, 'string');
    new_($this->db, 'db');
    new_($this->form, 'form');
    new_($this->file, 'file');
    new_($this->i18n, 'i18n');
    new_($this->session, 'session');
    new_($this->image, 'image');
    new_($this->ttf, 'ttf_text');
    new_($this->header, 'header');
    new_($this->mail, 'mail');
    $this->version = MAJOR_VERSION.'.'.MINOR_VERSION;
    $this->no_parse = false;
    $this->header_str = '';
    $this->footer_str = '';
    $this->ob->start();
    $this->compress_data = false;
    ob_implicit_flush(false);
  }
};

function my_shutdown_function() {
  global $pp;
  $pp->tpl->cache->garbagecollection();
  return;
}

function my_output_callback($buffer) {
  global $timer, $pp;

  $str = $buffer;

  if($pp->no_parse == TRUE)
    return $str;

  if(MAINTENANCE_MODE == 1) {
    if($pp->tpl->mime_subtype == 'html' || $pp->tpl->mime_subtype == 'xhtml')
      return 
	_("this application is currently in a maintenance state").
	$pp->tpl->return_char.
	_("Please come back later");
    else
      return '';
  }
  $str = $pp->tpl->comment_start.
    " ".LIBNAME." v".
    $pp->version.
    " - ".
    date("D M d Y")." ".
    $pp->tpl->comment_end.
    "\n".
    $pp->header_str.
    $str;
  switch($pp->tpl->mime_subtype) {
    case 'html':
      $str = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'."\n".$str;
      break;
    case 'xhtml':
      $str = '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/2000/REC-xhtml1-20000126/DTD/xhtml1-transitional.dtd">'."\n".$str;
      break;
    case 'xml':
      $str = '<?xml version="1.0"?>'."\n".$str;
      break;
  }
  if(DEVEL_MODE) {
    $timer->stop('full_page_parse_time');
    $elapsed = round($timer->elapsed('full_page_parse_time'), 3);
    if($pp->tpl->mime_subtype == 'html' || $pp->tpl->mime_subtype == 'xhtml')
      $str .= 
	'<div style="text-align: center; font-size: 9px; font-family: Verdana">'.
	$pp->tpl->return_char.
	_("page generated on ").$elapsed.' sec '.
	'('.LIBNAME.' v'.$pp->version.')</div>';
    else
      $str .= 
	_("page generated on ").$elapsed.' sec '.
	'('.LIBNAME.' v'.$pp->version.')'."\n";
  }
  $str = $str.$pp->footer_str;
  $str = $pp->tpl->parse_template($str);
  // XXX - this break javascript since some javascript code 
  // should need to escape `'' also
  if(AUTO_STRIPSLASHES)
    $str = stripslashes($str);
  // XXX - this reduce about 13% datasize send
  // if($pp->compress_data && ! DEVEL_MODE)
  // {
  //   $str = str_replace("\n", "", $str);
  //   $str = str_replace("\r", "", $str);
  //   $str = str_replace("\t", " ", $str);
  //   $str = preg_replace("/ ( )+/", " ", $str);
  // }
  if($pp->compress_data && ($pp->ob->get_compression() != 'none') ) {
    $pp->ob->compress_add_headers();
    $str = $pp->ob->compress($str);
  }
  $pp->header->header_add("X-Powered-By: ".LIBNAME." ".$pp->version);
  $pp->header->header_send();
  return $str;
}

register_shutdown_function('my_shutdown_function');
$pp = new pp;

?>
