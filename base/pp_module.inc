<?
// $Id: pp_module.inc,v 1.4 2002/02/20 19:10:37 tieum Exp $
//
// conf file syntax :
// id_site<tab>module_file<tab>module_name<tab>menu

class pp_module extends pp_file
{
  var $module_conf_file;
  var $module_list;
  var $module_path;
  var $module_key;
  function pp_module()
  {
    $this->module_path = -1;
    $this->module_key = -1;
    $this->module_conf_file = -1;
    $this->module_list = array();
  }

  function load_modules()
  {
    if($this->module_conf_file == -1)
      $this->error('pp_module::load_modules : '._("please set").' module_conf_file');
    if(array_key_exists('modules_list', $_SESSION))
    {
      $this->module_list = $_SESSION['modules_list'];
      foreach($_SESSION['modules_list'] as $module)
      {
	include_once(BASE_PATH.'/modules/'.$module['module_file']);
	if(DEBUG && SYSLOG)
	  $this->syslog('-- loading module '.$module['module_name'].' (session) --');
      }
      return;
    }
    $this->filename = $this->module_conf_file;
    $this->read();
    $i = 0;
    foreach(split("\n", $this->data) as $line)
      if(strlen($line))
	if($line[0] != '#')
	{
	  $data = split("\t", $line);
	  if($data[0] == $this->module_key)
	  {
	    $module_file = $data[1];
	    $module_name = $data[2];
	    $module_menu = $data[3];
	    $_SESSION['modules_list'][$i]['module_file'] = $module_file;
	    $_SESSION['modules_list'][$i]['module_name'] = $module_name;
	    $_SESSION['modules_list'][$i]['module_menu'] = $module_menu;
	    $i++;
	    include_once($this->module_path.'/'.$module_file);
	    if(DEBUG && SYSLOG)
	      $this->syslog('-- loading module '.$module_name.' --');
	  }
	}
    $this->module_list = $_SESSION['modules_list'];
  }
};

?>
