<?

////
// $Id: output_buffering.inc,v 1.20 2003/08/28 15:22:44 mgf Exp $
// !ouput buffering
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE

class output_buffering extends _stdclass {
  var $data;
  var $output_callback;
  var $compression;
  var $do_compression;
  var $do_caching;

  function output_buffering($my_output_callback = -1) {
    $this->_stdclass();
    if($my_output_callback == -1)
      user_error(
	  "output_buffering::output_buffering : ".
	  _("no output callback function defined"), E_USER_ERROR
	  );
    $this->data = '';
    $this->output_callback = $my_output_callback;
    $this->compression = '';
    $this->do_compression = 1;
    $this->do_caching = 1;
  }

  function start() {
    ob_start($this->output_callback);
  }

  function stop() {
    ob_end_flush();
  }

  function clean() {
    ob_end_clean();
  }

  function suspend() {
    $this->data = ob_get_contents();
    $this->clean();
    ob_start();
  }

  function resume() {
    $tmp = ob_get_contents();
    $this->clean();
    $this->start();
    print($this->data);
    $this->data = '';
    return $tmp;
  }

  function get_compression() {
    if(array_key_exists('__ua_compression', $_SESSION))
      return ($this->compression = $_SESSION['__ua_compression']);
    $accept_encoding = $GLOBALS['HTTP_ACCEPT_ENCODING'];
    if (strpos($accept_encoding, 'x-gzip') !== FALSE)
      return ($_SESSION['__ua_compression'] = $this->compression = 'x-gzip');
    if (strpos($accept_encoding, 'gzip') !== FALSE)
      return ($_SESSION['__ua_compression'] = $this->compression = 'gzip');
    return ($_SESSION['__ua_compression'] = $this->compression = 'none');
  }

  function compress_add_headers() {
    global $HTTP_SERVER_VARS, $pp;
    $etag = LIBNAME.'-cache-' . md5($this->data);
    $pp->header->header_add("ETag: $etag");
    if (strstr(stripslashes($HTTP_SERVER_VARS['HTTP_IF_NONE_MATCH']), $etag)) {
      $pp->header->header_add("HTTP/1.0 304");
    } else {
      $pp->header->header_add("Content-Encoding: ".$this->compression);
      $pp->header->header_add("Vary: Accept-Encoding");
    }
  }

  function compress($data) {
    $len = strlen($data);
    $crc = crc32($data);
    $data_compress = gzcompress($data, GZ_COMPRESSION_LEVEL);
    $data_compress = 
      substr($data_compress, 0, strlen($data_compress) - 4). 
      pack('V', $crc).pack('V', $len);
    return GZ_HEADER.$data_compress;
  }
};

?>
