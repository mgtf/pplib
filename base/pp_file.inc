<?

////
// $Id: pp_file.inc,v 1.16 2002/10/16 12:01:30 tieum Exp $
// !files I/O class
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//
// public methods :
//
// exists()
// unlink()
// write()
// read()
// get_mime()
//
// private methods :
//
// sh_lock()
// ex_lock()
// unlock()
// split_filename()
//
// consts :
//
// none

class pp_file extends pp_mime
{
  var $filename;
  var $data;
  var $fd;

  function pp_file()
  {
    $this->pp_mime();
    $this->filename = -1;
    $this->data = -1;
    return 1;
  }

  function split_filename()
  {
    $array_path = split('/', $this->filename);
    $filename = $array_path[sizeof($array_path) - 1];
    $path = substr($this->filename, 0, strlen($this->filename) - strlen($filename));
    return array($path, $filename);
  }

  function exists()
  {
    return file_exists($this->filename);
  }

  function unlink()
  {
    if($this->exists())
      unlink($this->filename);
    else
      user_error('pp_file::unlink : '._("file")." '".$this->filename."' "._("does not exist"), E_USER_ERROR);
  }

  function write($force = 0, $lock = 1)
  {
    if($this->data == -1 || $this->filename == -1 || strlen($this->filename) < 1)
      user_error("pp_file::write : "._("data or filename empty"), E_USER_ERROR);
    $p = $this->split_filename();
    $path = $p[0];
    $filename = $p[1];
    if( $this->exists() && !$force)
      user_error(
	  "pp_file::write : "._("file")." '".$filename."' ".
	  _("already exist, use pp_file->write(1) to force overwrite"), E_USER_ERROR);
    if(! is_writable($path) )
      user_error(
	  "pp_file::write : "._("directory")." '".$path."' ".
	  _("is not writeable"), E_USER_ERROR);
    $this->fd = @fopen($this->filename, "w");
    if($lock)
      $this->ex_lock();
    if($this->fd == FALSE)
      user_error("pp_file::write : "._("can't open")." '".$this->filename."'", E_USER_ERROR);
    // using set_magic_quotes_runtime does _not_ work.
    // set_magic_quotes_runtime(0);
    if( @fwrite($this->fd, $this->data) == -1)
      user_error("pp_file::write : "._("can't write")." '".$this->filename."'", E_USER_ERROR);
    // set_magic_quotes_runtime(get_magic_quotes_gpc());
    if($lock)
      $this->unlock();
    if( @fclose($this->fd) == FALSE )
      user_error("pp_file::write : "._("can't close")." '".$this->filename."'", E_USER_ERROR);
    return strlen($this->data);
  }

  function read($lock = 1)
  {
    if($this->filename == -1 || strlen($this->filename) < 1)
      user_error("pp_file::read : no file to read", E_USER_ERROR);
    if(! $this->exists())
      user_error(
	  "pp_file::read : "._("file")." '".$this->filename."' ".
	  _("does not exist"), E_USER_ERROR);
    if(! @is_readable($this->filename) )
      user_error("pp_file::read : '".$this->filename."' "._("is not readable"), E_USER_ERROR);
    if( ($this->fd = @fopen ($this->filename, "r")) == FALSE)
      user_error("pp_file::read : "._("can't open")." '".$this->filename."'", E_USER_ERROR);
    if($lock)
      $this->sh_lock();
    $this->data = @fread($this->fd, filesize ($this->filename));
    if($lock)
      $this->unlock();
    if ( @fclose($this->fd) == FALSE )
      user_error("pp_file::read : "._("can't close")." ".$this->filename."'", E_USER_ERROR);
    return $this->data;
  }

  // shared lock
  function sh_lock()
  {
    if( flock($this->fd, LOCK_SH) == FALSE)
      user_error(
	  "pp_file::lock : "._("can't get shared lock on file ").
	  $this->filename, E_USER_ERROR);
  }

  // exclusive lock
  function ex_lock()
  {
    if( flock($this->fd, LOCK_EX) == FALSE)
      user_error(
	  "pp_file::lock : "._("can't get exclusive lock on file ").
	  $this->filename, E_USER_ERROR);
  }

  function unlock()
  {
    if( flock($this->fd, LOCK_UN) == FALSE)
      user_error("pp_file::lock : "._("can't unlock file ").$this->filename, E_USER_ERROR);
  }

  function get_mime()
  {
    // XXX - the future new solution (currently from php4 CVS) :
    // $mime = mime_content_type($this->filename);
    // $mime = split('/', $mime[1]);
    // $this->mime_type = substr($mime[0], 1);
    // $this->mime_subtype = $mime[1];
    // XXX - my own solution :)
    if(! $this->exists())
      user_error('pp_file::get_mime : '._("file")." '".$this->filename."' "._("does not exist"), E_USER_ERROR);
    global $pp;
    $pp->ob->suspend();
    system('/usr/bin/file -i '.$this->filename);
    $mime = $pp->ob->resume();
    $mime = split(':', $mime);
    $mime = split('/', $mime[1]);
    $this->mime_type = substr($mime[0], 1);
    $this->mime_subtype = $mime[1];
  }
};

?>
