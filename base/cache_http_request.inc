<?

////
//  $Id: cache_http_request.inc,v 1.5 2003/09/02 10:06:40 mgf Exp $
//  !http request caching system
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

define('__CACHE_CONTEXT_GET', 1);
define('__CACHE_CONTEXT_POST', 2);
define('__CACHE_CONTEXT_COOKIE', 4);
define('__CACHE_CONTEXT_SESSION', 8);

class cache_http_request extends cache {
  var $cache_path;
  var $context;

  function cache_http_request() {
    $this->cache();
    $this->context = 15;
  }

  function get_context($id) {
    static $context = array();
    if(isset($context[$id]))
      return $context[$id];
    $get = $post = $cookie = $session = '';
    if($this->context & __CACHE_CONTEXT_GET)
      $get = isset($_GET) ? serialize($_GET) : '';
    if($this->context & __CACHE_CONTEXT_POST)
      $post = isset($_POST) ? serialize($_POST) : '';
    if($this->context & __CACHE_CONTEXT_COOKIE)
      $cookie = isset($_COOKIE) ? serialize($_COOKIE) : '';
    if($this->context & __CACHE_CONTEXT_SESSION)
      $session = isset($_SESSION) ? serialize($_SESSION) : '';
    return ($context[$id] = md5($get.$post.$cookie.$session.serialize($id)));
  }
};

?>
