<?

////
//  $Id: template.inc,v 1.57 2002/07/26 16:21:23 tieum Exp $
//  !pp_template.inc - general template class.
//
//  public methods :
//
//  add_tag($name, $action, $options = array())
//  add_tag_array($array)
//  del_tag($nom)
//  del_tag_array($array)
//  tag_exists($tag)
//  parse_template_file($filename)
//  parse_template_to_file($str, $filename, $force = 0)
//  parse_template($str)
//  parse_template_file_to_file($file_src, $file_dest, $force = 0)
//
//  private methods :
//
//  pp_template()
//  do_parse_template($str)
//  template_echo($tag)
//  find_begin($str)
//  find_end($str, $begin = 0)
//  dquote_end($str)
//  squote_end($str)
//  bquote_end($str)
//  parse_tag($tag_str)
//  tag_include($tag)
//  tag_if($tag)
//  tag_dynbloc($tag)
//  tag_replace($tag)
//  tag_debug($tag)
//  tag_uppercase($tag)
//  tag_br($tag)
//  tag_ua_mozilla($tag)
//  tag_ua_msie($tag)
//  tag_urlencode($tag)
//  tag_urldecode($tag)
//
//  consts : 
//
//  TEMPLATE_TIMEOUT
//  INC_PATH
//  TPL_PATH
//  DEBUG
//  DEVEL_MODE
//  SYSLOG
//
// tags prédéfinis :
//
// {if cond='' then='' else=''}
// {include name='' type=''}
// {dynbloc bloc=''}
// {debug}
// {replace mode='' what='' by='' in=''}
// {uppercase data=''}
// {self}
// {ua_mozilla}
// {ua_msie}
// {urlencode data=''}
// {urldecode data=''}
// {hrdate data='' type=''}

class pp_template extends pp_file
{
  var $tags;
  var $timeout;
  var $debug;
  var $syslog;
  var $tpl_path;
  var $parse_iteration;
  var $dynbloc_data;

  function pp_template()
  {/*{{{*/
    $this->pp_file();
    $this->set_mime('html');
    if(defined('TEMPLATE_TIMEOUT'))
      $this->timeout = TEMPLATE_TIMEOUT;
    else
      $this->timeout = 800;
    if(defined('INC_PATH'))
      $this->inc_path = INC_PATH;
    else
      $this->inc_path = 'src/';
    if(defined('TPL_PATH'))
      $this->tpl_path = TPL_PATH;
    else
      $this->tpl_path = 'tpl/';
    if(defined('DEBUG'))
      $this->debug = DEBUG;
    else
      $this->debug = FALSE;
    if(DEVEL_MODE == 1)
      $this->syslog = TRUE;
    else
      $this->syslog = FALSE;
    if(defined('SYSLOG'))
      $this->syslog = SYSLOG;
    //
    $this->parse_iteration = 0;
    $this->dynbloc_data = '';
    // predefined tags :
    $this->add_tag('include', 'tag_include');
    $this->add_tag('dynbloc', 'tag_dynbloc');
    $this->add_tag('if', 'tag_if');
    $this->add_tag('replace', 'tag_replace');
    $this->add_tag('debug', 'tag_debug');
    $this->add_tag('uppercase', 'tag_uppercase');
    $this->add_tag('self', 'tag_self');
    $this->add_tag('ua_mozilla', 'tag_ua_mozilla');
    $this->add_tag('ua_msie', 'tag_ua_msie');
    $this->add_tag('urlencode', 'tag_urlencode');
    $this->add_tag('urldecode', 'tag_urldecode');
    $this->add_tag('br', 'tag_br');
    $this->add_tag('hrdate', 'tag_hrdate');
  }/*}}}*/

  ////
  // !ajoute le tag $name à la liste de tags extistant.
  // le parseur execute $action() lorsque le tag est rencontré
  // par celui-ci.
  function add_tag($name, $action, $options = array())
  {/*{{{*/
    if( ! function_exists($action) && ! method_exists($this, $action))
      user_error("pp_template::add_tag : "._("function")." '".$action."' "._("does not exist"), E_USER_ERROR);
    if( (! strlen($name)) || (! strlen($action)) )
    {
      if(! strlen($name))
	user_error("pp_template::add_tag : \$name "._("is not defined").", action = '$action'", E_USER_ERROR);
      else
	user_error("pp_template::add_tag : \$action "._("is not defined").", name = '$name'", E_USER_ERROR);
    }
    $this->del_tag($name);
    $this->tags[$name]['action'] = $action;
    $this->tags[$name]['name'] = $name;
    $this->tags[$name]['options'] = $options;
  }/*}}}*/

  ////
  // !ajoute un tableau associatif de tags.
  // la tableau est de la forme : array('nom_du_tag' => 'valeur_de_remplaceement' ..)
  // le parseur remplace le tag par sa valeur lorsque le tag est rencontré
  // par celui-ci.
  function add_tag_array($array)
  {/*{{{*/
    if(! is_array($array))
      user_error("pp_template::add_tag_array : "._("arg is not an array"), E_USER_ERROR);
    while (list($key, $value) = each($array))
    {
      if( $this->add_tag($key, 'template_echo', array("text" => $value)) == -1 )
	return (-1);
    }
    return (1);
  }/*}}}*/

  ////
  // !renvoi TRUE si le tag $tagname est défini, FALSE sinon.
  function tag_exists($tagname)
  {/*{{{*/
    return array_key_exists($tagname, $this->tags);
  }/*}}}*/

  ////
  // !retire le tag $name de la liste des tags connus.
  function del_tag($name)
  {/*{{{*/
    if(isset($this->tags))
      unset($this->tags[$name]);
  }/*}}}*/

  ////
  // !retire le tableau associatif de tags $array de la liste des tags connus.
  function del_tag_array($array)
  {/*{{{*/
    if(! is_array($array))
    {
      user_error("pp_template::del_tag_array : "._("arg is not an array"), E_USER_ERROR);
      return (-1);
    }
    while (list($key, $value) = each($array))
      $this->del_tag($key);
    return (1);
  }/*}}}*/

  ////
  // !lit le contenu de $filename, execute parse_template() et renvoie la chaine parsee
  function parse_template_file($filename)
  {/*{{{*/
    if(! strlen($filename))
      user_error("pp_template::parse_template_file : "._("no file to read"), E_USER_ERROR);
    $this->filename = $filename;
    $this->read();
    return ($this->parse_template($this->data));
  }/*}}}*/

  ////
  // !parse la chaine entrée en parametre et renvoie la chaine parsee
  function parse_template($str)
  {/*{{{*/
    $result = $this->do_parse_template($str);
    while ($result != $str)
    {
      $str = $result;
      $result = $this->do_parse_template($str);
    }
    return ($this->data = $result);
  }/*}}}*/

  ////
  // !parse la chaine entrée en parametre et l'ecris dans le fichier
  function parse_template_to_file($str, $filename, $force = 0)
  {/*{{{*/
    if(! strlen($filename))
      user_error("pp_template::parse_template_to_file : "._("no file to write"), E_USER_ERROR);
    if(! strlen($str))
      user_error("pp_template::parse_template_to_file : "._("no data to write"), E_USER_ERROR);
    $this->filename = $filename;
    $this->parse_template($str);
    return $this->write($force);
  }/*}}}*/

  ////
  // !parse le fichier entré $file_src et l'ecris dans $file_dest
  function parse_template_file_to_file($file_src, $file_dest, $force = 0)
  {/*{{{*/
    return $this->parse_template_to_file($this->parse_template_file($file_src), 
	$file_dest, 
	$force);
  }/*}}}*/

  ///////////////////////////////////////////////////////////////////////////////
  ///////// PRIVATE FUNCTIONS ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  ////
  // !private function
  function do_parse_template($str)
  {/*{{{*/
    $begin = $this->find_begin($str);
    if ($begin === false)
      return ($str);
    $result = '';
    $remaining = $str;
    for ($this->parse_iteration = 0; !($begin === false); $begin = $this->find_begin($remaining))
    {
      $this->parse_iteration++;
      if ($this->parse_iteration >= $this->timeout)
	user_error('pp_template::do_parse_template : '.
	    _("template timeout")." ("._("max iteration").' = '.$this->timeout.')', E_USER_ERROR);
      $end = $this->find_end($remaining, $begin);
      $before = substr($remaining, 0, $begin);
      $tag_string = substr($remaining, $begin + 1, $end - $begin - 1);
      $remaining = substr($remaining, $end);
      $result .= $before;
      // XXX - à tester, permet de mettre des tags dans le champ bloc des blocs dynamiques 
      if(substr($tag_string, 0, 7) == 'dynbloc')
	$result .= $this->parse_tag($tag_string);
      else
	$result .= $this->parse_tag($this->parse_template($tag_string));
    }
    $result .= $remaining;
    $this->parse_iteration = 0;
    return ($result);
  }/*}}}*/

  ////
  // !private function
  function find_begin($str)
  {/*{{{*/
    $len = strlen($str);
    if(! $len)
      return FALSE;
    if($str[0] == $this->tag_start)
      return 0;
    for ($i = 1; $i < $len; $i++)
      if($str[$i] == $this->tag_start && $str[$i - 1] != '\\')
	return $i;
    return FALSE;
  }/*}}}*/

  ////
  // !private function
  function find_end($str, $begin = 0)
  {/*{{{*/
    $nb = 1;
    for ($i = $begin + 1; $nb && isset($str[$i]); $i++)
    {
      switch ($str[$i])
      {
	case $this->tag_start:
	  if($str[$i - 1] != '\\')
	    $nb++;
	  break;
	case $this->tag_stop:
	  if($str[$i - 1] != '\\')
	    $nb--;
	  break;
      }
    }
    return $nb ? FALSE : $i;
  }/*}}}*/

  ////
  // !private function
  function find_char($str, $char)
  {/*{{{*/
    for ($i = 1; isset($str[$i]); $i++)
      if (($str[$i] == $char) && ($str[$i - 1] != '\\'))
	return ($i);
    return FALSE;
  }/*}}}*/

  ////
  // !private function
  function parse_tag($tag_str)
  {/*{{{*/
    $regexp = " *([^ ]+) *(.*)\\".$this->tag_stop;
    if (!ereg($regexp, $tag_str, $tb))
      return user_error('pp_template::parse_tag : '._("template error"), E_USER_ERROR);
    if (array_key_exists($tb[1], $this->tags))
      $tag = $this->tags[$tb[1]];
    else
      return $this->tag_start.$tag_str;
    $param = $tb[2];
    $regexp = " *([^ =]+) *= *(.*)";
    while (strlen($param) > 0)
    {
      if (!ereg($regexp, $param, $tb))
	break;
      $name = $tb[1];
      $other = $tb[2];
      switch ($other[0])
      {
	case '"':
	case "'":
	case "`":
	  $end = $this->find_char($other, $other[0]);
	  $par = substr($other, 1, $end - 1);
	  $param = substr($other, $end);
	  break;
	case $this->tag_start:
	  $end = $this->find_end($other);
	  $par = substr($other, 0, $end);
	  $param = substr($other, $end);
	  break;
	default:
	  $end = strpos($other, ' ');
	  if ($end === false)
	  {
	    $par = $other;
	    $param = '';
	  }
	  else
	  {
	    $par = substr($other, 0, $end);
	    $param = substr($other, $end);
	  }
	  break;
      }
      $tag['options'][$name] = $par;
    } 

    if (!$tag)
      return user_error("pp_template::parse_tag : "._("template error"), E_USER_ERROR);

    $f = $tag['action'];

    if(method_exists($this, $f))
      return $this->$f($tag);
    elseif(function_exists($f))
      return $f($tag);
    else
      return user_error("pp_template::parse_tag : "._("function")." '".
	  $tag[action]."' "._("does not exist"), E_USER_ERROR);
  }/*}}}*/

  ////
  // !private function
  function escape_tags($string)
  {/*{{{*/
    $string = ereg_replace($this->tag_start, '\\'.$this->tag_start, $string);
    $string = ereg_replace($this->tag_stop, '\\'.$this->tag_stop, $string);
    return $string;
  }/*}}}*/

  ////
  // !private function
  function template_echo($tag)
  {/*{{{*/
    return $tag['options']['text'];
  }/*}}}*/

  ////
  // !private function
  function tag_include($tag)
  {/*{{{*/
    $name = $tag['options']['name'];
    $type = '';
    if(array_key_exists('type', $tag['options']))
      $type = $tag['options']['type'];
    if ($name == '')
      return user_error("pp_template::tag_include : "._("you must specify template name").",\n".
	  _("correct syntax is")." : {include name=\"filename\" type=\"type\"}", E_USER_ERROR);
    switch ($type)
    {
      case 'raw':
	return ($this->parse_template_file($this->tpl_path.$name.'.html'));
      case 'php':
      default:  
	$file = $this->inc_path."$name".'.inc';
	if (!file_exists($file))
	  return user_error(_("no such include file")." : $file", E_USER_ERROR);
	include_once($file);
	$function = 'template_'.$name;
	if(! function_exists($function))
	  return user_error("pp_template::tag_include : ".
	      _("function")." : 'template_$name()' ".
	      _("does not exist"), E_USER_ERROR);
	$s = $function($tag);
	if ($s)
	  return ($this->parse_template($s));
	else
	  return user_error(_("invalid include file").": $file", E_USER_ERROR);
    }
  }/*}}}*/

  ////
  // !private function
  function tag_if($tag)
  {/*{{{*/
    $opt = $tag['options'];
    if (($opt['cond'] == '') || 
	($opt['cond'] == 'false') || 
	($opt['cond'] == 'off') || 
	($opt['cond'] == '0'))
    {
      if(array_key_exists('else', $opt))
	return $opt['else'];
      else
	return ' ';
    }
    else
      return array_key_exists('then', $opt) ? $opt['then'] : ' ';
  }/*}}}*/

  ////
  // !private function
  function tag_dynbloc($tag)
  {/*{{{*/
    $ite = 0;
    $str = '';
    $id = array_key_exists('id', $tag['options']) ? $id = $tag['options']['id'] : '';
    if (strlen($id))
      if(! isset($this->dynbloc_data[$id]))
	user_error("pp_template::tag_dynbloc : dynbloc_data['$id'] "._("not set")." !", E_USER_ERROR);
      else
	$data = $this->dynbloc_data[$id];
    else
      $data = $this->dynbloc_data;
    if(array_key_exists('bloc', $tag['options']))
      $bloc = $tag['options']['bloc'];
    else
      user_error("pp_template::tag_dynbloc : ".
	  _("you must specify `bloc` arg in dynbloc tag"), E_USER_ERROR);
    if(! is_array($data))
      $data = array();
    foreach($data as $v)
    {
      $this->add_tag_array($v);
      $this->add_tag_array(array('dynbloc_ite' => $ite++));
      $str .= $this->parse_template($bloc);
      $this->del_tag_array($v);
      $this->del_tag_array(array('dynbloc_ite'));
    }
    if (strlen($id))
      unset($this->dynbloc_data[$id]);
    else
      unset($this->dynbloc_data);
    return $str;
  }/*}}}*/

  ////
  // !private function
  function tag_replace($tag)
  {/*{{{*/
    $opt = $tag[options];
    switch ($opt[mode])
    {
      case 'str':
	return str_replace($opt['what'], $opt['by'], $opt['in']);
      case 'posix':
      case 'grep':
	return ereg_replace($opt['what'], $opt['by'], $opt['in']);
      case 'perl':
	return preg_replace($opt['what'], $opt['by'], $opt['in']);
      default:
	return $opt['in'];
    }
  }/*}}}*/

  ////
  // !private function
  function tag_uppercase($tag)
  {/*{{{*/
    $opt = $tag['options'];
    $txt = $opt['data'];
    return strtoupper($txt);
  }/*}}}*/

  ////
  // !private function
  function tag_self($tag)
  {/*{{{*/
    $self = '?';
    reset($_GET);
    while(list($k, $v) = each($_GET))
      if($v != NULL)
	$self .= '&'.$k.'='.urlencode($v);
    return $self;
  }/*}}}*/

  ////
  // !private function
  function tag_debug()
  {/*{{{*/
    $s = '';
    if($this->mime_subtype != 'html')
      return -1;
    foreach($this->tags as $v)
    {
      unset($value);
      if($v['action'] == 'template_echo')
      {
	new_($string, 'pp_string');
	$value = $string->cut_text($v['options']['text'], 50);
	$value = strlen($value) ? $value : _("not set").' !';
	$value = $this->escape_tags($value);
	$value = '('._("value").' : <b>`'.(nl2br(htmlentities($value))).'`</b>)';
      }
      else
	$value = '('._("action").' : <b>`'.(nl2br(htmlentities($v['action']))).'`</b>)';
      $s .= '
	<tr>
	<td>
	<font size=1><b>'.$v['name'].'</b> '.addslashes($value).'</font>
	</td>
	</tr>';
    }
    return '
      <table align=center border=1 cellspacing=0 cellpadding=2>
      <tr><th bgcolor="FFCCCC">
      <font size=1 color="red">template debug</font>
      </th></tr>'.
      $s.
      '</table><br>';
  }/*}}}*/

  ////
  // !private function
  function tag_urlencode($tag) { return urlencode($tag['options']['data']); }

  ////
  // !private function
  function tag_urldecode($tag) { return urldecode($tag['options']['data']); }

  ////
  // !private function
  function tag_ua_mozilla()
  {/*{{{*/
    if(array_key_exists('browser', (array) $_SESSION))
      return ($_SESSION['browser'] == 'mozilla') ? 1 : 0;
    else
    {
      new_($agent, 'pp_sniff');
      $this->add_tag_array(
	  array('ua_mozilla' => ($agent->browser == 'mozilla') ? 1 : 0)
	  );
      return ($agent->browser == 'mozilla') ? 1 : 0;
    }
  }/*}}}*/

  ////
  // !private function
  function tag_ua_msie()
  {/*{{{*/
    if(array_key_exists('browser', (array) $_SESSION))
      return ($_SESSION['browser'] == 'msie') ? 1 : 0;
    else
    {
      new_($agent, 'pp_sniff');
      $this->add_tag_array(
	  array('ua_msie' => ($agent->browser == 'msie') ? 1 : 0)
	  );
      return ($agent->browser == 'msie') ? 1 : 0;
    }
  }/*}}}*/

  ////
  // !private function
  function tag_hrdate($tag)
  {/*{{{*/
    $date = $tag['options']['data'];
    $type = array_key_exists('type', $tag['options']) ? $tag['options']['type'] : 1;
    $langue = ( defined('LANGUAGE') && LANGUAGE == 'fr_FR' ) ? 1 : 0;
    $mois = substr($date, 5, 2);
    $annee = substr($date, 0, 4);
    $jour = substr($date, -2);
    if ($type == 2)
    {
      $nom_jour[0] = $langue ? "Dimanche" : "Sunday";
      $nom_jour[1] = $langue ? "Lundi" : "Monday";
      $nom_jour[2] = $langue ? "Mardi" : "Tuesday";
      $nom_jour[3] = $langue ? "Mercredi" : "Wednesday";
      $nom_jour[4] = $langue ? "Jeudi" : "Thursday";
      $nom_jour[5] = $langue ? "Vendredi" : "Friday";
      $nom_jour[6] = $langue ? "Samedi" : "Saturday";
      $nom_mois[1] = $langue ? "janvier" : "January";
      $nom_mois[2] = $langue ? "f&eacute;vrier" : "February";
      $nom_mois[3] = $langue ? "mars" : "March";
      $nom_mois[4] = $langue ? "avril" : "April";
      $nom_mois[5] = $langue ? "mai" : "May";
      $nom_mois[6] = $langue ? "juin" : "June";
      $nom_mois[7] = $langue ? "juillet" : "July";
      $nom_mois[8] = $langue ? "ao&ucirc;t" : "August";
      $nom_mois[9] = $langue ? "septembre" : "September";
      $nom_mois[10] = $langue ? "octobre" : "October";
      $nom_mois[11] = $langue ? "novembre" : "November";
      $nom_mois[12] = $langue ? "d&eacute;cembre" : "December";

      $jour_semaine = $nom_jour[date("w", mktime(0, 0, 0, $mois, $jour, $annee))];
      $mois = $nom_mois[ (int) $mois];
      return $jour_semaine." ".$jour." ".$mois." ".$annee;
    }
    if($type == 3)
      $annee = substr($annee, 2, 2);
    return "$jour/$mois/$annee";
  }/*}}}*/

  ////
  // !private function
  function tag_br($tag) { return nl2br($tag['options']['data']); }
};

?>
