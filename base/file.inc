<?
// $Id: file.inc,v 1.1 2002/01/15 15:22:11 tieum Exp $

class pp_file extends pp_mime
{
  var $filename;
  var $data;

  function pp_file()
  {
    $this->pp_mime();
    $this->filename = -1;
    $this->data = -1;
    return 1;
  }

  // function _pp_file()
  // {
  //
  //
  // }

  function split_filename()
  {
    $array_path = split('/', $this->filename);
    $filename = $array_path[sizeof($array_path) - 1];
    $path = substr($this->filename, 0, strlen($this->filename) - strlen($filename));
    return array($path, $filename);
  }

  function write($force = 0, $lock = 1)
  {
    if($lock)
      $this->lock();
    if($this->data == -1 || $this->filename == -1 || strlen($this->filename) < 1)
      $this->error("pp_file::write : "._("data or filename empty"));
    $p = $this->split_filename();
    $path = $p[0];
    $filename = $p[1];
    if( file_exists($this->filename && !$force))
      $this->error("pp_file::write : "._("file")." '".$filename."' ".
	           _("already exist, use pp_file->write(1) to force overwrite"));
    if(! is_writable($path) )
      $this->error("pp_file::write : "._("directory")." '".$path."' ".
	           _("is not writeable"));
    $fd = @fopen($this->filename, "w");
    if($fd == FALSE)
      $this->error("pp_file::write : "._("can't open")." '".$this->filename."'");
    if( @fwrite($fd, $this->data) == -1)
      $this->error("pp_file::write : "._("can't write")." '".$this->filename."'");
    if( @fclose($fd) == FALSE )
      $this->error("pp_file::write : "._("can't close")." '".$this->filename."'");
    if($lock)
      $this->unlock();
    return strlen($this->data);
  }

  function read()
  {
    $this->wait_for_lock();
    if($this->filename == -1 || strlen($this->filename) < 1)
      $this->error("pp_file::read : no file to read");
    if(! @file_exists($this->filename))
      $this->error("pp_file::read : "._("file")." '".$this->filename."' ".
	            _("does not exist"));
    if(! @is_readable($this->filename) )
      $this->error("pp_file::read : '".$this->filename."' "._("is not readable"));
    if( ($fd = @fopen ($this->filename, "r")) == FALSE)
      $this->error("pp_file::read : "._("can't open")." '".$this->filename."'");
    if( $this->data = @fread ($fd, filesize ($this->filename)) )
    if ( @fclose($fd) == FALSE )
      $this->error("pp_file::read : "._("can't close")." ".$this->filename."'");
    return $this->data;
  }

  function lock()
  {
    $this->wait_for_lock();
    $lockfile = $this->get_lock_filename();
    if( @touch($lockfile) == FALSE )
      $this->error("pp_file::lock : "._("can't create lock file").": $lockfile");
  }

  function unlock()
  {
    $lockfile = $this->get_lock_filename();
    if($this->is_lock())
      @unlink($lockfile);
  }

  function get_lock_filename()
  {
    $p = $this->split_filename();
    return $p[0].'.'.$p[1];
  }

  function is_lock()
  {
    $lockfile = $this->get_lock_filename();
    return file_exists($lockfile);
  }

  function wait_for_lock()
  {
    if(! $this->is_lock())
      return;
    $lockfile = $this->get_lock_filename();
    while(file_exists($lockfile))
    {
      usleep(LOCK_SLEEP_TIME);
      clearstatcache();
    }
  }
  
};

?>
