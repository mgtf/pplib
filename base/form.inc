<?

////
//  $Id: form.inc,v 1.3 2002/02/19 17:21:20 tieum Exp $
//  form class
//
//  public methods :
//
//  add_field($type, $name, $options = '', $value = '', $img_src = '')
//  parse_form($options = '')
//
//  private methods :
//
//  none
//
//  consts :
//
//  none


class pp_form extends pp_class
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

  function pp_form()
  {
    $this->pp_class();
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
	$this->error("pp_form::add_field : no type specified");
    }
    $this->data .= $data;
  }

  function parse_form($fields_only = 0)
  {
    $this->send_to_mail = (strstr($this->action, 'mailto:') == FALSE) ? 0 : 1;
    if($this->has_file)
      $this->enctype = 'enctype="multipart/form-data"';
    elseif($this->send_to_mail)
      $this->enctype = 'text/plain';
    else
      // pas nécessaire à priori
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
