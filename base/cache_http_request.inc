<?

////
//  $Id: cache_http_request.inc,v 1.4 2003/09/01 14:01:36 mgf Exp $
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
