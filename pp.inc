<?

////
//  $Id: pp.inc,v 1.100 2003/09/04 13:46:26 mgf Exp $
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

// do not modify below
define('LIBNAME', 'ocms');
define('SYSLOG_PREFIX', '[ '.LIBNAME.' ]');
define('SERVER_ADMIN', $_SERVER['SERVER_ADMIN']);
define('MAGIC_MIME_FILE', '/usr/share/misc/file/magic.mime');
define('LIBPATH', dirname(__FILE__).'/');
define('MAJOR_VERSION', '3');
define('MINOR_VERSION', '4b');
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
require_once('base/mime.inc');
require_once('base/header.inc');
require_once('base/file.inc');
require_once('base/cache.inc');
require_once('base/cache_http_request.inc');
require_once('base/template.inc');
require_once('base/mysql.inc');
require_once('base/string.inc');
require_once('base/user.inc');
require_once('base/session.inc');
require_once('base/image.inc');
require_once('base/ttf_text.inc');
require_once('base/mail.inc');

class pp extends _stdclass {
  var $version;
  var $string;
  var $db;
  var $tpl;
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
  cache::garbagecollection();
}

function my_output_callback($buffer) {
  global $pp;

  $str = $buffer;

  if($pp->no_parse == TRUE)
    return $str;

  if(defined('MAINTENANCE_MODE') && MAINTENANCE_MODE) {
    if($pp->tpl->mime_subtype == 'html' || $pp->tpl->mime_subtype == 'xhtml')
      return 
	_("this application is currently in a maintenance state").
	$pp->tpl->return_char.
	_("Please come back later");
    else
      return '';
  }
  $str = 
    mime::get_doctype($pp->tpl->mime_subtype).
    $pp->tpl->comment_start.
    " ".LIBNAME." v".
    $pp->version.
    " - ".
    date("D M d Y")." ".
    $pp->tpl->comment_end.
    "\n".
    $pp->header_str.
    $str;
  if(DEVEL_MODE) {
    global $timer;
    $timer->stop('full_page_parse_time');
    $elapsed = round($timer->elapsed('full_page_parse_time'), 3);
    if($pp->tpl->mime_subtype == 'html' || $pp->tpl->mime_subtype == 'xhtml')
      $str .= 
	"\n".'<div style="text-align: center; font-size: 9px; font-family: Verdana">'.
	$pp->tpl->return_char.
	_("page generated on ").$elapsed.' sec '.
	'('.LIBNAME.' v'.$pp->version.')</div>';
    else
      $str .= 
	_("page generated on ").$elapsed.' sec '.
	'('.LIBNAME.' v'.$pp->version.')'."\n";
  }
  $str .= $pp->footer_str;
  $str = $pp->tpl->parse_template($str);
  $str = stripslashes($str);
  // XXX - bug here, see jbvnews popup
  // if($pp->compress_data && (output_buffering::get_compression() != 'none') ) {
  //   output_buffering::compress_add_headers();
  //   $str = output_buffering::compress($str);
  // }
  header::header_add("X-Powered-By: ".LIBNAME." ".$pp->version);
  header::header_send();
  return $str;
}

register_shutdown_function('my_shutdown_function');
$pp = new pp;

?>
