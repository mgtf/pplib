<?

////
// $Id: class.inc,v 1.16 2003/09/01 14:00:31 mgf Exp $
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//

function new_(&$object, $classname, $arg = '') {
  if(! class_exists($classname)) {
    new_($tpl, 'template');
    $tpl->error(
	"new_ : '$classname' ".
	_("is not the name of an existing class"));
  }
  if(! is_object($object)) {
    $object = new $classname($arg);
  } elseif( ($class = get_class($object)) == $classname ) {
    if(method_exists($object, $classname ))
      $object->$classname($arg);
    return 1;
  } else {
    new_($tpl, 'template');
    $tpl->error(
	"new_ : "._("you are trying to create an object from the class")." $classname\n".
	_("but it is already an object from the class")." $class");
  }
  return 1;
}

class _stdclass extends stdclass {
  var $debug;
  var $syslog;

  function _stdclass() {
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

  function syslog($errstr, $priority = LOG_ERR) {
    openlog(SYSLOG_PREFIX, LOG_PID | LOG_PERROR, LOG_LOCAL0);
    syslog($priority, $errstr);
    closelog();
  }

  function error($errstr, $file = -1, $line = -1) {
    if($file != -1 && $line != -1)
      $errstr .= "\n"._("file")." : $file\n"._("line")." : $line";
    if($this->syslog)
      $this->syslog($errstr);
    user_error($errstr, E_USER_ERROR);
  }

  function warning($errstr) {
    if($this->syslog)
      $this->syslog($errstr, LOG_WARNING);
    if($this->debug)
      user_error($errstr, E_USER_WARNING);
  }
};

?>
