<?

////
//  $Id: cache_http_request.inc,v 1.7 2003/09/12 13:57:52 mgf Exp $
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
    $this->context = 
      __CACHE_CONTEXT_GET +
      __CACHE_CONTEXT_POST +
      __CACHE_CONTEXT_COOKIE +
      __CACHE_CONTEXT_SESSION;
  }

  function get_context($id) {
    $x[] =& $id;
    if(($this->context & __CACHE_CONTEXT_GET) && isset($_GET))
      $x[] =& $_GET;
    if(($this->context & __CACHE_CONTEXT_POST) && isset($_POST))
      $x[] =& $_POST;
    if(($this->context & __CACHE_CONTEXT_COOKIE) && isset($_COOKIE))
      $x[] =& $_COOKIE;
    if(($this->context & __CACHE_CONTEXT_SESSION) && isset($_SESSION))
      $session = isset($_SESSION) ? serialize($_SESSION) : '';
    return md5(serialize($x));
  }
};

?>
