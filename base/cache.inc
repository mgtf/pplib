<?

////
//  $Id: cache.inc,v 1.3 2002/04/12 17:08:49 tieum Exp $
//  data class
//
//  public methods :
//
//  private methods :
//
//  none
//
//  consts :
// 
//  STORAGE_PATH


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
