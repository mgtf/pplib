<?

////
//  $Id: pp_cache.inc,v 1.4 2002/09/18 11:58:44 tieum Exp $
//  !data class - not finished yet
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


class pp_cache extends pp_file
{
  var $cache_path;
  function pp_cache()
  {
    $this->pp_file();
    if(DEFINED('CACHE_PATH'))
      $this->cache_path = CACHE_PATH;
    else
      user_error('please define CACHE_PATH', E_USER_ERROR);
  }
  
  function cache($data)
  {
    $this->filename = $this->cache_path.'/'.md5($data);
    $this->data = $data;
    $this->write();
  }
};

?>
