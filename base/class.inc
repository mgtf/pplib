<?

// $Id: class.inc,v 1.3 2002/02/15 15:22:57 tieum Exp $
// just a test to try get_class(), class_exists(), ...
// don't know if it's really usefull

function new_(&$object, $classname, $arg = '')
{
  if(! class_exists($classname))
  {
    new_($tpl, 'pp_template');
    $tpl->error("new_ : '$classname' ".
	        _("is not the name of an existing class"));
  }
  if(! is_object($object))
    $object = new $classname($arg);
  elseif( ($class = get_class($object)) == $classname )
  {
    if(method_exists($object, $classname ))
      $object->$classname($arg);
    return 1;
  }
  else
  {
    global $pp;
    $pp->error("new_ : "._("you are trying to create an object from the class")." $classname\n".
	       _("but it is already an object from the class")." $class");
  }
  return 1;
}

class pp_class
{
  var $debug;
  var $syslog;

  function pp_class()
  {
    if(defined('DEBUG'))
      $this->debug = DEBUG;
    else
      $this->debug = FALSE;
    if(DEVEL_MODE == TRUE)
      $this->syslog = TRUE;
    else
      $this->syslog = FALSE;
    if(defined('SYSLOG'))
      $this->syslog = SYSLOG;
  }

  function syslog($errstr)
  {
    openlog(PP_LOG_NAME, LOG_PID | LOG_PERROR, LOG_LOCAL0);
    syslog(LOG_ERR, $errstr);
    closelog();
  }

  function error($errstr)
  {
    if($this->syslog) 
      $this->syslog($errstr);
    user_error($errstr, E_USER_ERROR);
  }

  function warning($errstr)
  {
    if($this->syslog) 
      $this->syslog($errstr);
    if($this->debug)
      user_error($errstr, E_USER_WARNING);
  }
};

?>
