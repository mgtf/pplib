<?

////
//  $Id: cache_http_request.inc,v 1.3 2003/08/28 15:22:44 mgf Exp $
//  !http request caching system
//  Ce fichier est soumis à la licence d'utilisation
//  contenue dans le fichier LICENCE
//
//  public methods :
//  get($id, $max_age = CACHE_DEFAULT_MAX_AGE)
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

class cache_http_request extends cache {
  var $cache_path;

  function cache_http_request() {
    $this->cache();
  }

  function get_context($id) {
    static $context = array();
    if(isset($context[$id]))
      return $context[$id];
    $get = isset($_GET) ? serialize($_GET) : '';
    $post = isset($_POST) ? serialize($_POST) : '';
    $cookie = isset($_COOKIE) ? serialize($_COOKIE) : '';
    $session = isset($_SESSION) ? serialize($_SESSION) : '';
    return ($context[$id] = md5($get.$post.$cookie.$session.$id));
  }
};

?>
