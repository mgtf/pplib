<?
// $Id: pp_module.inc,v 1.2 2002/02/20 18:00:13 tieum Exp $
//
// conf file syntax :
// module_key<tab>file_to_include

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
	include_once(BASE_PATH.'/modules/'.$module);
	if(DEBUG && SYSLOG)
	  $this->syslog('-- loading module '.$module.' (session) --');
      }
      return;
    }
    $this->filename = $this->module_conf_file;
    $this->read();
    foreach(split("\n", $this->data) as $line)
      if(strlen($line))
	if($line[0] != '#')
	{
	  $data = split("\t", $line);
	  if($data[0] == $this->module_key)
	  {
	    $module = $data[1];
	    $_SESSION['modules_list'][] = $module;
	    include_once($this->module_path.'/'.$module);
	    if(DEBUG && SYSLOG)
	      $this->syslog('-- loading module '.$module.' --');
	  }
	}
    $this->module_list = $_SESSION['modules_list'];
  }
};

?>