<?

////
//  $Id: cache.inc,v 1.8 2003/08/28 11:53:50 mgf Exp $
//  !caching system
//  Ce fichier est soumis à la licence d'utilisation
//  contenue dans le fichier LICENCE
//
//  public methods :
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
  define('CACHEFILE_MAXLIFETIME', 3600);
}

class pp_cache extends pp_file {
  var $cache_path;

  function pp_cache() {
    $this->pp_file();
    if(defined('CACHE_PATH'))
      $this->cache_path = CACHE_PATH;
    else
      user_error(_("please define CACHE_PATH"), E_USER_ERROR);
    if(! defined('CACHE_DEFAULT_MAX_AGE')) {
      define('CACHE_DEFAULT_MAX_AGE', 5);
    }
  }

  function get_context($id) {
    static $context = array();
    if(isset($context[$id]))
      return CACHEFILE_PREFIX.$context[$id];
    $get = isset($_GET) ? serialize($_GET) : '';
    $post = isset($_POST) ? serialize($_POST) : '';
    $cookie = isset($_COOKIE) ? serialize($_COOKIE) : '';
    $session = isset($_SESSION) ? serialize($_SESSION) : '';
    $context[$id] = md5($get.$post.$cookie.$session.$id);
    return CACHEFILE_PREFIX.$context[$id];
  }

  function is_expired($id, $max_age = CACHE_DEFAULT_MAX_AGE) {
    $file = $this->cache_path.$this->get_context($id);
    clearstatcache();
    return (! file_exists($file) || ((time() - fileatime($file)) > $max_age));
  }

  function get($id, $max_age = CACHE_DEFAULT_MAX_AGE) {
    if(! $this->is_expired($id, $max_age))
      return $this->load($id);
    else
      return false;
  }

  function save($id, $data) {
    $data = serialize($data);
    $this->filename = $this->cache_path.$this->get_context($id);
    $this->data =& $data;
    $this->write(true);
  }
  
  function load($id) {
    $this->garbagecollection();
    $this->filename = $this->cache_path.$this->get_context($id);
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
    $already_run = true;
  }

};

?>
