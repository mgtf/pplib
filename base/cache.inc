<?

////
//  $Id: cache.inc,v 1.18 2003/09/04 13:47:21 mgf Exp $
//  !caching system
//  Ce fichier est soumis à la licence d'utilisation
//  contenue dans le fichier LICENCE
//
//  public methods :
//  get($id, $max_age = CACHEFILE_MAXAGE)
//  save($id, $data) {
//  
//  private methods :
//
//  none
//
//  consts :
// 
//  CACHE_PATH
//  CACHEFILE_PREFIX
//  CACHEFILE_MAXLIFETIME

if(! defined('CACHEFILE_PREFIX')) {
  define('CACHEFILE_PREFIX', 'cache_');
}
if(! defined('CACHEFILE_MAXLIFETIME')) {
  // Default to 1 day
  define('CACHEFILE_MAXLIFETIME', 86400);
}

class cache extends file {
  var $cache_path;

  function cache() {
    $this->file();
    if(defined('CACHE_PATH'))
      $this->cache_path = CACHE_PATH;
    else
      user_error(_("please define CACHE_PATH"), E_USER_ERROR);
    if(! defined('CACHEFILE_MAXAGE')) {
      define('CACHEFILE_MAXAGE', (DEVEL_MODE ? 0 : 3600));
    }
  }

  function get_context($id) {
    return md5(serialize($id));
  }

  function is_expired($id, $max_age) {
    $file = $this->cache_path.CACHEFILE_PREFIX.$this->get_context($id);
    clearstatcache();
    return (! file_exists($file) || ((time() - fileatime($file)) > $max_age));
  }

  function get($id, $max_age = CACHEFILE_MAXAGE) {
    if(! $this->is_expired($id, $max_age))
      return $this->load($id);
    else {
      $this->filename = $this->cache_path.CACHEFILE_PREFIX.$this->get_context($id);
      if($this->exists())
	$this->unlink();
      return false;
    }
  }

  function save($id, $data) {
    $data = serialize($data);
    $this->filename = $this->cache_path.CACHEFILE_PREFIX.$this->get_context($id);
    $this->data =& $data;
    $this->write(true);
  }
  
  function load($id) {
    $this->filename = $this->cache_path.CACHEFILE_PREFIX.$this->get_context($id);
    if($this->exists()) {
      $this->read();
      return unserialize($this->data);
    } else {
      return false;
    }
  }

  function garbagecollection() {
    static $already_run = false;
    if($already_run)
      return;
    $already_run = true;
    srand((double) microtime() * 1000000);
    // run only 2% of times.
    if(rand(1, 100) > 2)
      return;
    $cmd = "find ".$this->cache_path." -type f -name '".CACHEFILE_PREFIX."*' -print";
    $a = `$cmd`;
    $files = explode("\n", $a);
    clearstatcache();
    foreach($files as $file) {
      if(file_exists($file) && ((time() - filemtime($file)) > CACHEFILE_MAXLIFETIME))
	unlink($file);
    }
  }
};

?>
