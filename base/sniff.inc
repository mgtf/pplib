<?
// $Id: sniff.inc,v 1.1 2002/02/20 14:38:39 tieum Exp $

class pp_sniff extends pp_class
{

  var $platform;
  var $browser;
  
  function pp_sniff()
  {
    $this->pp_class();
    $this->platform = -1;
    $this->browser = -1;
    $this->browser_version = -1;
    // unset ($BROWSER_AGENT);
    // unset ($BROWSER_VER);
    // unset ($BROWSER_PLATFORM);
  }

  function browser_get_agent() 
  {
    global $BROWSER_AGENT;
    $this->browser = $BROWSER_AGENT;
    if(ereg('MSIE ([0-9].[0-9]{1,2})', 
	    $HTTP_USER_AGENT, 
	    $log_version)) 
    {
      $this->browser_version=$log_version[1];
      $this->browser='ie';
    }
    elseif(ereg('Opera ([0-9].[0-9]{1,2})',
	        $HTTP_USER_AGENT,
		$log_version))
    {
      $this->browser_version=$log_version[1];
      $this->browser='opera';
    } 
    elseif(ereg('Mozilla/([0-9].[0-9]{1,2})',
	         $HTTP_USER_AGENT,
		 $log_version)) 
    {
      $this->browser_version=$log_version[1];
      $this->browser='mozilla';
    } 
    else 
    {
      $this->browser_version=0;
      $this->browser='other';
    }
  }

  // function browser_get_version()
  // {
  //   global $BROWSER_VER;
  //   $this->browser_version = $BROWSER_VER;
  // }
  //
  function browser_get_platform() 
  {
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
