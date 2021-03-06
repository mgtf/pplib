<?

////
// $Id: form.inc,v 1.7 2003/08/28 15:22:44 mgf Exp $
// !form class
// Ce fichier est soumis � la licence d'utilisation
// contenue dans le fichier LICENCE
//
// public methods :
//
// add_field($type, $name, $options = '', $value = '', $img_src = '')
// parse_form($options = '')
//
// private methods :
//
// none
//
// consts :
//
// none


class form extends _stdclass
{
  var $name;
  var $num_fields;
  var $method;
  var $action;
  var $enctype;
  var $has_file; // to check if form contain "file" type field
  var $data;
  var $before;
  var $after;

  // enctype="multipart/form-data"

  function form()
  {
    $this->_stdclass();
    $this->method = 'POST';
    $this->has_file = 0;
    $this->data = '';
    $this->before = '';
    $this->after = '';
    $this->action = '';
  }

  function add_field($type, $name, $options = '', $value = '', $img_src = '')
  {
    $data = '';
    if(strlen($value))
    {
      $value = 'value="'.$value.'"';
      $val = $value;
    }
    else
    {
      $value = 'value="{'.$name.'}"';
      $val = '{'.$name.'}';
    }
    switch($type)
    {
      case 'text':
        $data = $this->before.
	        '<input type="text" name="'.$name.'" '.$value.' '.$options.'>'.
		$this->after."\n";
        break;
      case 'textarea':
        $data = $this->before.
	        '<textarea name='.$name.' '.$options.'>'.$val.'</textarea>'.
		$this->after."\n";
        break;
      case 'checkbox':
        $data = $this->before.
	        '<input type="checkbox" name="'.$name.'" '.$value.' '.$options.'>'.
		$this->after."\n";
        break;
      case 'select':
        $data = $this->before.
	        '<select name="'.$name.'" '.$options.'>'.
		$this->after."\n";
        break;
      case 'end_select':
        $data = '</select '.$options.'>';
        break;
      case 'option':
        $data = $this->before.
	        '<option '.$value.' '.$options.'>'.$name.'</option>'.
		$this->after."\n";
        break;
      case 'radio':
        $data = $this->before.
	        '<input type="radio" name="'.$name.'" '.$value.' '.$options.'>'.
		$this->after."\n";
        break;
      case 'image':
        $data = $this->before.
	        '<input type="image" src="'.$img_src.'" '.$options.'>'.
		$this->after."\n";
        break;
      case 'submit':
        $data = $this->before.
	        '<input type="submit" name="submit" value="'.$name.'" '.$options.'>'.
		$this->after."\n";
        break;
      case 'hidden':
        $data = $this->before.
	        '<input type="hidden" name="'.$name.'" '.$value.' '.$options.'>'.
		$this->after."\n";
        break;
      case 'button':
        $data = $this->before.
	        '<input type="button" value="'.$name.'" '.$options.'>'.
		$this->after."\n";
	break;
      case 'file':
	$data = $this->before.
	        '<input type="file" name="">'.
		$this->after."\n";
	$this->has_file++;
	break;
      case 'null':
	$data = $this->before.'&nbsp;'.$this->after;
	break;
      default:
	user_error("form::add_field : no type specified", E_USER_ERROR);
    }
    $this->data .= $data;
  }

  function parse_form($options = '')
  {
    $this->send_to_mail = (strstr($this->action, 'mailto:') == FALSE) ? 0 : 1;
    if($this->has_file)
      $this->enctype = 'enctype="multipart/form-data"';
    elseif($this->send_to_mail)
      $this->enctype = 'text/plain';
    else
      // pas n�cessaire � priori
      // $this->enctype = 'application/x-www-form-urlencoded';
      $this->enctype = '';
    $data = '<form '.$options.' '.$this->enctype.' action="'.$this->action.'" method="'.$this->method.'">'."\n".
            $this->data.
	    '</form>'."\n";
    unset($this->data);
    return $data;
  }

};

?>
