<?
// $Id: output_buffering.inc,v 1.3 2002/02/26 12:47:44 tieum Exp $

class pp_output_buffering extends pp_class
{
  var $data;
  var $output_callback;
  var $compression;
  var $do_compression;
  var $do_caching;

  function pp_output_buffering($my_output_callback = -1)
  {
    $this->pp_class();
    if($my_output_callback == -1)
      $this->error(
	  "pp_output_buffering::pp_output_buffering : ".
	  _("no output callback function defined")
	  );
    $this->data = '';
    $this->output_callback = $my_output_callback;
    $this->compression = '';
    $this->do_compression = 1;
    $this->do_caching = 1;
  }

  function start()
  {
    ob_start($this->output_callback);
  }

  function stop()
  {
    ob_end_flush();
  }

  function clean()
  {
    ob_end_clean();
  }

  function suspend()
  {
    $this->data = ob_get_contents();
    $this->clean();
    ob_start();
  }

  function resume()
  {
    $tmp = ob_get_contents();
    $this->clean();
    $this->start();
    print $this->data;
    $this->data = '';
    return $tmp;
  }

  function get_compression()
  {
    $accept_encoding = $GLOBALS['HTTP_ACCEPT_ENCODING'];
    if (strpos($accept_encoding, 'x-gzip') !== false)
      return ($this->compression = 'x-gzip');
    if (strpos($accept_encoding, 'gzip') !== false)
      return ($this->compression = 'gzip');
    return '';
  }

  function printcontent($content = '') 
  {
    global $HTTP_SERVER_VARS;
    if ($content == '')
      $content = & $this->data;
    if ($this->do_compression && $this->do_caching) 
    {
      $etag = 'PPLIB-Cache-' . md5(substr($content, -40));
      header("ETag: $etag");
      if (strstr(stripslashes($HTTP_SERVER_VARS['HTTP_IF_NONE_MATCH']), $etag)) 
      {
	// not modified
	header('HTTP/1.0 304');
	return;
      } 
      else 
      {
	// client accepts some encoding - send headers & data
	header("Content-Encoding: ".$this->compression);
	header('Vary: Accept-Encoding');
	print "\x1f\x8b\x08\x00\x00\x00\x00\x00";
      }
    }
    print $content;
  }
};

?>
