<?
// $Id: pp_file.inc,v 1.6 2002/02/08 16:29:15 tieum Exp $

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

  function unlink()
  {
    if( file_exists($this->filename))
      unlink($this->filename);
    else
      $this->error('pp_file::unlink : '._("file")." '".$this->filename."' "._("does not exist"));
  }

  function write($force = 0, $lock = 1)
  {
    if($this->data == -1 || $this->filename == -1 || strlen($this->filename) < 1)
      $this->error("pp_file::write : "._("data or filename empty"));
    $p = $this->split_filename();
    $path = $p[0];
    $filename = $p[1];
    if( file_exists($this->filename) && !$force)
      $this->error("pp_file::write : "._("file")." '".$filename."' ".
	           _("already exist, use pp_file->write(1) to force overwrite"));
    if(! is_writable($path) )
      $this->error("pp_file::write : "._("directory")." '".$path."' ".
	           _("is not writeable"));
    $this->fd = @fopen($this->filename, "w");
    if($lock)
      $this->ex_lock();
    if($this->fd == FALSE)
      $this->error("pp_file::write : "._("can't open")." '".$this->filename."'");
    if( @fwrite($this->fd, $this->data) == -1)
      $this->error("pp_file::write : "._("can't write")." '".$this->filename."'");
    if($lock)
      $this->unlock();
    if( @fclose($this->fd) == FALSE )
      $this->error("pp_file::write : "._("can't close")." '".$this->filename."'");
    return strlen($this->data);
  }

  function read($lock = 1)
  {
    if($this->filename == -1 || strlen($this->filename) < 1)
      $this->error("pp_file::read : no file to read");
    if(! @file_exists($this->filename))
      $this->error("pp_file::read : "._("file")." '".$this->filename."' ".
	            _("does not exist"));
    if(! @is_readable($this->filename) )
      $this->error("pp_file::read : '".$this->filename."' "._("is not readable"));
    if( ($this->fd = @fopen ($this->filename, "r")) == FALSE)
      $this->error("pp_file::read : "._("can't open")." '".$this->filename."'");
    if($lock)
      $this->sh_lock();
    $this->data = @fread ($this->fd, filesize ($this->filename));
    if($lock)
      $this->unlock();
    if ( @fclose($this->fd) == FALSE )
      $this->error("pp_file::read : "._("can't close")." ".$this->filename."'");
    return $this->data;
  }

  // shared lock
  function sh_lock()
  {
    if( flock($this->fd, LOCK_SH) == FALSE)
      $this->error("pp_file::lock : "._("can't get shared lock on file ").$this->filename);
  }

  // exclusive lock
  function ex_lock()
  {
    if( flock($this->fd, LOCK_EX) == FALSE)
      $this->error("pp_file::lock : "._("can't get exclusive lock on file ").$this->filename);
  }

  function unlock()
  {
    if( flock($this->fd, LOCK_UN) == FALSE)
      $this->error("pp_file::lock : "._("can't unlock file ").$this->filename);
  }
};

?>
