<?

////
//  $Id: pp_template.inc,v 1.10 2002/02/12 14:33:22 tieum Exp $
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
//  brace_end($str)
//  parse_tag($tag_str)
//  template_include($tag)
//  template_if($tag)
//  template_dynbloc($tag)
//  template_replace($tag)
//  template_debug($tag)
//
//  consts : 
//
//  TEMPLATE_TIMEOUT
//  INC_PATH
//  TPL_PATH
//  DEBUG
//

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
  {
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
    $this->add_tag('include', 'template_include');
    $this->add_tag('dynbloc', 'template_dynbloc');
    $this->add_tag('if', 'template_if');
    $this->add_tag('replace', 'template_replace');
    $this->add_tag('debug', 'template_debug');
    $this->add_tag('trans', 'template_translate');
    $this->add_tag('self', 'template_self');
  }

  function add_tag($name, $action, $options = array())
  {
    if( ! function_exists($action) && ! method_exists($this, $action))
      $this->error("pp_template::add_tag : "._("function")." '".$action."' "._("does not exist"));
    if( (! strlen($name)) || (! strlen($action)) )
    {
      if(! strlen($name))
	$this->error("pp_template::add_tag : \$name "._("is not defined").", action = '$action'");
      else
	$this->error("pp_template::add_tag : \$action "._("is not defined").", name = '$name'");
    }
    $this->del_tag($name);
    $this->tags[$name]['action'] = $action;
    $this->tags[$name]['name'] = $name;
    $this->tags[$name]['options'] = $options;
  }

  function add_tag_array($array)
  {
    if(! is_array($array))
      $this->error("pp_template::add_tag_array : "._("arg is not an array"));
    while (list($key, $value) = each($array))
    {
      if( $this->add_tag($key, 'template_echo', array("text" => $value)) == -1 )
	return (-1);
    }
    return (1);
  }

  function tag_exists($tag)
  {
    return isset($this->tags[$name]);
  }

  function del_tag($name)
  {
    if(isset($this->tags))
      unset($this->tags[$name]);
  }

  function del_tag_array($array)
  {
    if(! is_array($array))
    {
      $this->error("pp_template::del_tag_array : "._("arg is not an array"));
      return (-1);
    }
    while (list($key, $value) = each($array))
      $this->del_tag($key);
    return (1);
  }

  ////
  // !lit le contenu de $filename, execute parse_template() et renvoie la chaine parsee
  function parse_template_file($filename)
  {
    if(! strlen($filename))
      $this->error("pp_template::parse_template_file : "._("no file to read."));
    $this->filename = $filename;
    $this->read();
    return ($this->parse_template($this->data));
  }

  ////
  // !parse la chaine entrée en parametre et renvoie la chaine parsee
  function parse_template($str)
  {
    if (!$this->parse_iteration)
      $this->parse_iteration = 0;

    $result = $this->do_parse_template($str);
    while ($result != $str)
    {
      $str = $result;
      $result = $this->do_parse_template($str);
    }
    return ($this->data = $result);
  }

  ////
  // !parse la chaine entrée en parametre et l'ecris dans le fichier
  function parse_template_to_file($str, $filename, $force = 0)
  {
    if(! strlen($filename))
      $this->error("pp_template::parse_template_to_file : "._("no file to write."));
    if(! strlen($str))
      $this->error("pp_template::parse_template_to_file : "._("no data to write."));
    $this->filename = $filename;
    $this->parse_template($str);
    return $this->write($force);
  }

  ////
  // !parse le fichier entré $file_src et l'ecris dans $file_dest
  function parse_template_file_to_file($file_src, $file_dest, $force = 0)
  {
    return $this->parse_template_to_file($this->parse_template_file($file_src), 
					 $file_dest, 
					 $force);
  }

  ///////////////////////////////////////////////////////////////////////////////
  ///////// PRIVATE FUNCTIONS ///////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // !private function
  function do_parse_template($str)
  {
    $begin = $this->find_begin($str);
    if ($begin === false)
      return ($str);
    $result = '';
    $remaining = $str;
    for (; !($begin === false); $begin = $this->find_begin($remaining))
    {
      $this->parse_iteration++;
      if ($this->parse_iteration >= $this->timeout)
	$this->error('pp_template::do_parse_template : '.
	             _("template timeout")." ("._("max iteration").' = '.$this->timeout.')');
      $end = $this->find_end($remaining, $begin);
      $before = substr($remaining, 0, $begin);
      $tag_string = substr($remaining, $begin + 1, $end - $begin - 1);
      $remaining = substr($remaining, $end);
      $result .= $before;
      $result .= $this->parse_tag($this->parse_template($tag_string));
    }
    $result .= $remaining;
    $this->parse_iteration = 0;
    return ($result);
  }

  ////
  // !private function
  function find_begin($str)
  {
    return (strpos($str, $this->tag_start));
  }

  ////
  // !private function
  function find_end($str, $begin = 0)
  {
    $len = strlen($str);
    $nb = 1;
    for ($i = $begin + 1; ($nb) && ($i < $len); $i++)
    {
      switch ($str[$i])
      {
	case $this->tag_start:
	  $nb++;
	  break;
	case $this->tag_stop:
	  $nb--;
	  break;
      }
    }
    if ($nb < 0)
      return 0;
    return ($i);
  }

  ////
  // !private function
  function dquote_end($str)
  {
    $len = strlen($str);
    for ($i = 1; $i < $len; $i++)
    if (($str[$i] == '"') && ($str[$i - 1] != '\\'))
      return ($i);
  }

  ////
  // !private function
  function squote_end($str)
  {
   $len = strlen($str);
   for ($i = 1; $i < $len; $i++)
    if (($str[$i] == "'") && ($str[$i - 1] != '\\'))
     return ($i);
  }

  ////
  // !private function
  function bquote_end($str)
  {
   $len = strlen($str);
   for ($i = 1; $i < $len; $i++)
    if (($str[$i] == "`") && ($str[$i - 1] != '\\'))
     return ($i);
  }

  ////
  // !private function
  function brace_end($str)
  {
   $len = strlen($str);
   $nb = 1;
   for ($i = 1; ($i < $len) && ($nb); $i++)
    {
     switch ($str[$i])
      {
      case $this->tag_start:
       $nb++;
       break;
      case $this->tag_stop:
       $nb--;
       break;
      }
    }
   return ($i);
  }

  ////
  // !private function
  function parse_tag($tag_str)
  {
    $regexp = " *([^ ]+) *(.*)\\".$this->tag_stop;
    if (!ereg($regexp, $tag_str, $tb))
      return $this->error('pp_template::parse_tag : '._("template error"));
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
	  $end = $this->dquote_end($other);
	  $par = stripslashes(substr($other, 1, $end - 1));
	  $param = substr($other, $end);
	  break;
	case "'":
	  $end = $this->squote_end($other);
	  $par = stripslashes(substr($other, 1, $end - 1));
	  $param = substr($other, $end);
	  break;
	case "`":
	  $end = $this->bquote_end($other);
	  $par = stripslashes(substr($other, 1, $end - 1));
	  $param = substr($other, $end);
	  break;
	case $this->tag_start:
	  $end = $this->brace_end($other);
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
      return $this->error("pp_template::parse_tag : "._("template error"));

    $f = $tag['action'];

    if(method_exists($this, $f))
      return $this->$f($tag);
    elseif(function_exists($f))
       return $f($tag);
    else
      return $this->error("pp_template::parse_tag : "._("function")." '".$tag[action]."' "._("does not exist"));
  }

  ////
  // !private function
  function template_echo($tag)
  {
    return ($tag['options']['text']);
  }

  ////
  // !private function
  function template_include($tag)
  {
    $name = $tag[options][name];
    $type = $tag[options][type];
    if ($name == '')
      return $this->error("pp_template::template_include : "._("you must specify template name").",\n".
			  _("correct syntax is")." : {include name=\"filename\" type=\"type\"}");
    switch ($type)
    {
      case raw:
	return ($this->parse_template_file($this->tpl_path.$name.'.html'));
      case php:
      default:  
	$file = $this->inc_path."$name".'.inc';
	if (!file_exists($file))
	  return $this->error("No such include file: $file");
	
	include_once($file);
	if(! function_exists("template_$name"))
	  return $this->error("pp_template::template_include : ".
	                       _("function")." : 'template_$name()' ".
			       _("does not exist"));
	$s = 'return template_'.$name.'($tag, $tags);';
	$s = eval($s);
	if ($s)
	 return ($this->parse_template($s, $tags));
	else
	  return $this->error(_("invalid include file").": $include_rep/$name");
    }
  }

  function template_if($tag)
  {
    $opt = $tag['options'];
    if (($opt['cond'] == '') || 
	($opt['cond'] == 'false') || 
	($opt['cond'] == 'off') || 
	($opt['cond'] == '0'))
    {
      if(array_key_exists('else', $opt))
	return $opt['else'];
      else
	return '';
    }
    else
      return $opt['then'];
    // switch ($opt['mode'])
    // {
    //   case 'str':
    //     return ($opt['cond'] != '') ? $opt['then'] : $opt['else'];
    //   case 'bool':
    //     if (($opt['cond'] == '') || ($opt['cond'] == 'false') || ($opt['cond'] == 'off'))
    //       return $opt['else'];
    //     else
    //       return $opt['then'];
    //   case 'int':
    //     if ((((int) $opt['cond']) == $opt['cond']) && ($opt['cond'] != 0))
    //       return $opt['then'];
    //     else
    //       return $opt['else'];
    //   case 'php':
    //     return (eval("return (".$opt['cond']).")" ? $opt['then'] : $opt['else']);
    //   default:
    //     if (($opt['cond'] == '') || ($opt['cond'] == 'false') || ($opt['cond'] == 'off')
    //         || ($opt['cond'] == '0'))
    //       return $opt['else'];
    //     else
    //       return $opt['then'];
    // }
  }

  ////
  // !private function
  function template_dynbloc($tag)
  {
    $ite = 0;
    $str = '';
    $id = array_key_exists('id', $tag['options']) ? $id = $tag['options']['id'] : '';
    if (strlen($id))
      $data = $this->dynbloc_data[$id];
    else
      $data = $this->dynbloc_data;
    $bloc = $tag['options']['bloc'];
    if(! is_array($data))
      return '';
    while( list($k, $v) = each($data) )
    {
      $this->add_tag_array($v);
      $this->add_tag_array(array('dynbloc_ite' => $ite++));
      $str .= $this->parse_template($bloc);
      $this->del_tag_array($v);
      $this->del_tag_array(array('dynbloc_ite'));
    }
    $this->dynbloc_data = NULL;
    return $str;
  }

  ////
  // !private function
  function template_replace($tag)
  {
    $opt = $tag[options];
    switch ($opt[mode])
    {
      case 'str':
	return str_replace($opt[what], $opt[by], $opt[in]);
      case 'posix':
      case 'grep':
	return ereg_replace($opt[what], $opt[by], $opt[in]);
      case 'perl':
	return preg_replace($opt[what], $opt[by], $opt[in]);
      default:
	return $opt[in];
    }
  }

  ////
  // !private function
  function template_translate($tag)
  {
    $opt = $tag['options'];
    $txt = $opt['data'];
    eval('$txt = _("'.$txt.'");');
    return $txt;
  }

  ////
  // !private function
  function template_self($tag)
  {
    $str = '?';
    global $HTTP_GET_VARS;
    while(list($k, $v) = each($HTTP_GET_VARS))
      $str .= '&'.$k.'='.$v;
    return $str;
  }

  ////
  // !private function
  function template_debug()
  {
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
	$value = strlen($value) ? $value : 'not set !';
	$value = '('._("value").' : <b>\''.(nl2br(htmlspecialchars($value))).'\'</b>)';
      }
      else
        $value = '('._("action").' : <b>\''.$v['action'].'\'</b>)';
      $s .= '<tr>
	       <td>
		 <font size=1><b>'.$v['name'].'</b> '.$value.'</font>
	       </td>
	     </tr>';
    }
    return '<table align=center border=1 cellspacing=0 cellpadding=2>
	    <tr><th bgcolor="FFCCCC">
	    <font size=1 color="red">template debug</font>
	    </th></tr>'.
           $s.
	   '</table><br>';
  }

};

?>
