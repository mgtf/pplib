<?
// $Id: pp_output_buffering.inc,v 1.10 2002/03/07 10:03:15 tieum Exp $

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
    print($this->data);
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
    return 0;
  }

  function compress_add_headers() 
  {
    global $HTTP_SERVER_VARS, $pp;
    $etag = 'PPLIB-cache-' . md5(substr($this->data, -40));
    $pp->header->header_add("ETag: $etag");
    if (strstr(stripslashes($HTTP_SERVER_VARS['HTTP_IF_NONE_MATCH']), $etag))
      $pp->header->header_add("HTTP/1.0 304");
    else
    {
      $pp->header->header_add("Content-Encoding: ".$this->compression);
      $pp->header->header_add("Vary: Accept-Encoding");
    }
  }

  function compress($data)
  {
    $len = strlen($data);
    $crc = crc32($data);
    $data_compress = gzcompress($data, 9);
    $data_compress = substr($data_compress, 0, 
	strlen($data_compress) - 4) . pack('V', $crc) . pack('V', $len);
	return "\x1f\x8b\x08\x00\x00\x00\x00\x00".$data_compress;
  }
};

?>
