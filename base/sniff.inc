<?

//// 
// $Id: sniff.inc,v 1.5 2003/09/01 12:28:20 mgf Exp $
// !sniff client browser
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//

class sniff extends _stdclass {

  var $platform;
  var $browser;
  
  function sniff() {
    $this->_stdclass();
    $this->platform = -1;
    $this->browser = -1;
    $this->browser_version = -1;
    $this->browser_get_agent();
    $this->browser_get_platform();
  }

  function browser_get_agent() {
    global $HTTP_USER_AGENT;
    if(ereg('MSIE ([0-9].[0-9]{1,2})', 
	    $HTTP_USER_AGENT, 
	    $log_version)) {
      $this->browser_version=$log_version[1];
      $this->browser='msie';
    } elseif(ereg('Opera ([0-9].[0-9]{1,2})', $HTTP_USER_AGENT, $log_version)) {
      $this->browser_version=$log_version[1];
      $this->browser='opera';
    } elseif(ereg('Mozilla/([0-9].[0-9]{1,2})', $HTTP_USER_AGENT, $log_version)) {
      $this->browser_version=$log_version[1];
      $this->browser='mozilla';
    } else {
      $this->browser_version=0;
      $this->browser='other';
    }
  }

  function browser_get_platform() {
    global $HTTP_USER_AGENT;
    if (strstr($HTTP_USER_AGENT,'Win'))
      $this->platform='win';
    else if (strstr($HTTP_USER_AGENT,'Mac'))
      $this->platform='mac';
    else if (strstr($HTTP_USER_AGENT,'Linux'))
      $this->platform='linux';
    else if (strstr($HTTP_USER_AGENT,'Unix'))
      $this->platform='unix';
    else
      $this->platform='other';
  }
};

?>
