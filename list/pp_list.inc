<?

////
//  $Id: pp_list.inc,v 1.32 2002/03/20 15:45:22 tieum Exp $
//  lists class
//
//  public methods :
//
//  parse_list()
//
//  private methods :
//
//  pp_list()
//  get_vars()
//  set_tags()
//  hidden_fields()
//  get_data()
//  list_search($tag)
//  upd_data($id)
//  del_data($listdel)
//  add_data($listadd)
//  compteur()
//  get_where()
//
//  consts :
//  
//  LIST_MAX_ELEMENTS

class pp_list extends pp_template
{
  var $tpl;
  var $sql_table;
  var $sql_fields;
  var $sql_key;
  // list id to use more than one list on the same page :
  var $id;
  //
  var $max_elements;
  var $bg_line;
  var $message_add;
  var $message_del;
  var $message_upd;
  // private vars :
  var $message;
  var $search_string;
  var $current_page;
  var $current_search_field;
  var $search_fields;
  var $current_order_field;
  var $order_fields;
  var $order;                 // e.g. DESC/ASC
  var $needed_fields;
  var $current_page;
  // wrappers :
  var $del_wrapper;
  var $upd_wrapper;
  var $add_wrapper;
  var $get_wrapper;

  // array like : field_name => fonction_name.
  // ex : $mylist->function_on_field = array('password' => 'MD5');
  // this will use SQL MD5 function with insert & update query.
  var $function_on_field;
  //

  // to add additional 'where' clause when getting data
  var $additional_where_clause;
  //

  function pp_list()
  {
    $this->pp_template();
    $this->tpl = 'please_define_a_template';
    $this->order = 'ASC';
    $this->current_page = 0;
    if(defined('LIST_MAX_ELEMENTS'))
      $this->max_elements = LIST_MAX_ELEMENTS;
    else
      $this->max_elements = 10;
    $this->search = '';
    $this->bg_line = array( 0 => '#DEEEFF',
			    1 => '#FFFFFF');
    $this->message_del = '[ suppression effectuée ]';
    $this->message_add = '[ ajout effectué ]';
    $this->message_upd = '[ modification effectuée ]';
    $this->message = '';
    $this->id = 'please_change_list_id';
    $this->del_wrapper = -1;
    $this->upd_wrapper = -1;
    $this->add_wrapper = -1;
    $this->get_wrapper = -1;
    $this->needed_fields = -1;
    $this->function_on_field = -1;
    $this->additional_where_clause = -1;
  }

  function parse_list()
  {
    global $HTTP_POST_VARS, $HTTP_GET_VARS;
    if(! strlen($this->sql_fields) )
      $this->error("parse_list::parse_list : you must specify sql fields");
    if(! strlen($this->sql_table) )
      $this->error("parse_list::parse_list : you must specify sql table");
    if(! strlen($this->sql_key) )
      $this->error("parse_list::parse_list : you must specify primary key");
    if(! strlen($this->search_fields) )
      $this->error("parse_list::parse_list : you must specify search field");
    if(! isset($this->order_fields) )
      $this->order_fields = $this->sql_key;
    // actions : 
    if(array_key_exists($this->id.'_listdel', $HTTP_POST_VARS))
      $listdel = $HTTP_POST_VARS[$this->id.'_listdel'];
    else
      $listdel = -1;
    if(array_key_exists($this->id.'_listadd', $HTTP_POST_VARS))
      $listadd = $HTTP_POST_VARS[$this->id.'_listadd'];
    else
      $listadd = -1;
    if(array_key_exists($this->id.'_list_ids', $HTTP_POST_VARS))
      $list_ids = $HTTP_POST_VARS[$this->id.'_list_ids'];
    else
      $list_ids = -1;
    if(array_key_exists($this->id.'_was_updated', $HTTP_POST_VARS))
      $was_updated = $HTTP_POST_VARS[$this->id.'_was_updated'];
    else
      $was_updated = -1;
    if(array_key_exists($this->id.'_want_save', $HTTP_POST_VARS))
      $want_save = $HTTP_POST_VARS[$this->id.'_want_save'];
    else
      $want_save = -1;
    if(array_key_exists($this->id.'_want_save', $HTTP_GET_VARS))
      $want_save = $HTTP_GET_VARS[$this->id.'_want_save'];
    if(is_array($listdel) && $want_save)
      $this->del_data($listdel);
    if(is_array($listadd) && $want_save)
      $this->add_data($listadd);
    $updated = 0;
    if(is_array($list_ids) && $want_save)
      foreach($list_ids as $i)
	if($HTTP_POST_VARS[$this->id.'_was_updated'.$i])
	{
	  $this->upd_data($i);
	  $updated = 1;
	}
    $this->get_vars();
    $this->set_tags($want_save);
    $this->get_data();
    $this->add_tag_array(array('confirm_mesg' => _("Do you confirm modifications ?") ));
    $str = $this->parse_template_file(PP_LIB_PATH."/list/list.js");
    $str .= $this->parse_template_file($this->tpl);
    return $str;
  }

  /////////////////////////////////////////////////////////////////////////////////
  ////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////

  function get_vars()
  {
    global $HTTP_POST_VARS;
    $this->search_string = array_key_exists($this->id.'_search_string', $HTTP_POST_VARS) ? 
                           $HTTP_POST_VARS[$this->id.'_search_string'] : '';
    $this->current_search_field = array_key_exists($this->id.'_current_search_field', $HTTP_POST_VARS) ? 
                                  $HTTP_POST_VARS[$this->id.'_current_search_field'] : '';
    $this->max_elements = array_key_exists($this->id.'_max_elements', $HTTP_POST_VARS) ?
                          $HTTP_POST_VARS[$this->id.'_max_elements'] : $this->max_elements;
    $this->current_page = array_key_exists($this->id.'_current_page', $HTTP_POST_VARS) ?
                          $HTTP_POST_VARS[$this->id.'_current_page'] : 0 ;
    $this->current_order_field = array_key_exists($this->id.'_current_order_field', $HTTP_POST_VARS) ?
                                 $HTTP_POST_VARS[$this->id.'_current_order_field'] : $this->sql_key;
    $this->order = array_key_exists($this->id.'_order', $HTTP_POST_VARS) ? 
                   $HTTP_POST_VARS[$this->id.'_order'] : '';
    $this->current_page = array_key_exists($this->id.'_want_page', $HTTP_POST_VARS) ? 
                          $HTTP_POST_VARS[$this->id.'_want_page'] : '';
    $this->search = array_key_exists($this->id.'_list_search', $HTTP_POST_VARS) ?
                    $HTTP_POST_VARS[$this->id.'_list_search'] : '';
    $this->start = $this->current_page * $this->max_elements;
  }

  function set_tags($want_save)
  {
    $self = $this->parse_template('{self}');
    if(strlen($this->message) < 1 && $want_save == 1)
      $this->message = '[ aucune modification effectuée ]';
    $this->add_tag_array(array($this->id.'_search_string' => $this->search_string,
			       $this->id.'_current_search_field' => $this->current_search_field,
			       $this->id.'_max_elements' => $this->max_elements,
			       $this->id.'_list_mesg' => $this->message,
			       $this->id.'_current_order_field' => $this->current_order_field,
			       $this->id.'_order' => $this->order,
			       'id' => $this->id,
			       $this->id.'_want_page' => 0,
			       $this->id.'_want_save' => 0,
			       $this->id.'_list_submit_add' => '
			       <a href="javascript:'.$this->id.'_confirm_del()"><img src="/icons/save.gif"></a>',
			       $this->id.'_list_submit' => '
			       <a href="javascript:'.$this->id.'_confirm_del()"><img src="/icons/save.gif"></a>
			       <font size=1><img src="/icons/invert.gif" alt="'.
			       _("invert selection").
			       '" onclick="javascript:'.$this->id.'_invertsel(\''.$this->id.'\')"></font>
    '));
    $this->add_tag($this->id.'_list_hidden_fields', 'hidden_fields');
    $this->add_tag($this->id.'_list_search', 'list_search');
    $this->add_tag($this->id.'_list_compteur', 'compteur');
  }

  function hidden_fields()
  {
    new_($s, 'pp_form');
    $s->add_field('hidden', $this->id.'_current_search_field');
    $s->add_field('hidden', $this->id.'_max_elements');
    $s->add_field('hidden', $this->id.'_search_string');
    $s->add_field('hidden', $this->id.'_current_order_field');
    $s->add_field('hidden', $this->id.'_order');
    $s->add_field('hidden', $this->id.'_want_save');
    return $s->data;
  }

  function get_data()
  {
    new_($sql, 'pp_db');
    $table = $this->sql_table;
    $fields = $this->sql_table.'.'.$this->sql_key.' AS "'.$this->sql_table.'_'.$this->sql_key.'"';
    $nl = 0;
    foreach( (explode(',', strtr($this->sql_fields, array(' ' => '')))) as $f)
    {
      if( strstr($f, '(') )
      {
	preg_match('/(.*)\((.*)\)/', $f, $r);
	$link_table[$nl][0] = $r[2];
	$link_table[$nl][1] = $r[1];
	$nl++;
      }
      else
      {
	$fields .= ', '.$this->sql_table.'.'.$f.' AS "'.$this->sql_table.'_'.$f.'"';
	$fl[] = $this->sql_table.'_'.$f;
      }
    }
    $sql->query("SELECT ".$fields."
		 FROM   ".$this->sql_table."
		 WHERE  ".$this->get_where()."
		 ORDER BY ".$this->current_order_field." ".$this->order."
		 LIMIT  ".$this->start.", ".$this->max_elements); 
    $i = 0;
    $data = NULL;
    while($sql->next_record())
    {
      $k = $sql->record[$this->sql_table.'_'.$this->sql_key];
      if($nl)
      {
	foreach($link_table as $x)
	{
	  $fieldas = $x[0].'_'.$x[1];
	  $field = $x[0].'.'.$x[1];
	  new_($sql_l, 'pp_db', "SELECT ".$field." AS '".$fieldas."' FROM ".$x[0]." WHERE ".$this->sql_key." = ".$k);
	  if($sql_l->num_rows())
	  {
	    $data[$i][$field] = '<table border=0 width=100% cellspacing=0 cellpadding=0>'."\n";
	    while($sql_l->next_record())
	      $data[$i][$field] .= '<tr><td bgcolor="'.$this->bg_line[($c++ % 2)].'">'.$sql_l->record[$field].'</td></tr>'."\n";
	    $data[$i][$field] .= '</table>'."\n";
	  }
	  else
	    $data[$i][$field] = '&nbsp;';
	  $sql_l->free();
	}
      }
      foreach($fl as $f)
      {
	$list_check_update = 'onChange="document.'.$this->id.'.'.$this->id.'_was_updated'.$k.'.value =\'1\'"';
	$data[$i][$this->id.'_bgcolor'] = $this->bg_line[($i % 2)];
	$data[$i][$this->id.'_id_list'] = $k;
	$data[$i][$this->id.'_list_check_update'] = $list_check_update;
	$data[$i][$this->id.'_list_del'] = '<input type="checkbox" name="'.$this->id.'_listdel['.$k.']">';
	$data[$i][$this->id.'_'.$f] = ($sql->record[$f]) ? 
	                stripslashes( strtr($sql->record[$f], array('"' => '')) )
			: '';
      }
      if($this->get_wrapper != -1)
      {
	$func = $this->get_wrapper;
	$d = $func($k);
	while(list($k, $v) = each($d))
	  $data[$i][$k] = $v;
      }
      $i++;
    }
    $sql->free();
    $this->dynbloc_data = $data;
  }

  function list_search($tag)
  {
    new_($form, 'pp_form');
    $form->action = '';
    $form->data .= '<font class="list">'._("Search").' </font>';
    $form->add_field("text", $this->id."_search_string", "size=\"10\"");
    $form->data .= '<font class="list">'._("into").' </font>';
    $form->add_field("select", $this->id."_current_search_field", 'onChange=document.'.$this->id.'_list_search.submit()');
    foreach( (explode(',', $this->search_fields)) as $f)
    {
      if( strstr($f, '(') )
      {
	preg_match('/(.*)\((.*)\)/', $f, $r);
	$field = strtr($r[1], array(' ' => ''));
	$name = $r[2];
      }
      else
      {
	$field = $f;
        $name = $f;
      }
      $form->add_field('option', $name, ($this->current_search_field == $field) ? 'SELECTED' : '', $field);
    }
    $form->add_field('end_select', '');
    // menu nbre d'elements par page : 
    $choices = array(5, 10, 15, 20, 25, 30, 40, 50, 75, 100);
    $form->data .= '<font class="list">'._("displaying").' </font>';
    $form->add_field("select", $this->id."_max_elements", "onChange=document.".$this->id."_list_search.submit()");
    foreach($choices as $choice)
      $form->add_field("option", $choice, 
		       ($choice == $this->max_elements) ? 'SELECTED' : '', $choice);
    $form->add_field("end_select", "");
    // classer par :
    $form->data .= '<font class="list">'._("elements per page").'.<br>'._("Sort by").' </font>';
    $form->add_field("select", $this->id."_current_order_field", "onChange=document.".$this->id."_list_search.submit()");
    foreach( (explode(',', $this->order_fields)) as $f)
    {
      if( strstr($f, '(') )
      {
	preg_match('/(.*)\((.*)\)/', $f, $r);
	$field = strtr($r[1], array(' ' => ''));
	$name = $r[2];
      }
      else
      {
	$field = $f;
        $name = $f;
      }
      $form->add_field('option', $name, ($this->current_order_field == $field) ? 'SELECTED' : '', $field);
    }
    $form->add_field('end_select', '');
    // ordre croissant/décroissant :
    $form->data .= '<font class="list"> '._("order").' </font>';
    $form->add_field("select", $this->id."_order", "onChange=document.".$this->id."_list_search.submit()");
    $form->add_field('option', _("ascendent"), ($this->order == 'ASC') ? 'SELECTED' : '', 'ASC');
    $form->add_field('option', _("descendent"), ($this->order == 'DESC') ? 'SELECTED' : '', 'DESC');
    $form->add_field('end_select', '');
    //
    return $form->parse_form('name="'.$this->id.'_list_search"');
  }

  function upd_data($id)
  {
    if( ! $id)
      return;
    global $HTTP_POST_VARS;
    $q = 'UPDATE '.$this->sql_table.' SET '.$this->sql_key.' = \''.$id.'\'';
    foreach( (explode(',', strtr($this->sql_fields, array(' ' => '')))) as $f)
      if(! strstr($f, '(') )
      {
        $data = $HTTP_POST_VARS[$this->sql_table.'_'.$f.$id];
	// test if we must apply function when update ( like PASSWORD() )
	if(is_array($this->function_on_field) && 
	   array_key_exists($f, $this->function_on_field))
	{
	  $fonc = $this->function_on_field[$f];
	  $q .= ', '.$f.' = '.$fonc.'(\''.$data.'\')';
	}
	else
	  $q .= ', '.$f.' = \''.$data.'\'';
      }
    $q .= ' WHERE '.$this->sql_key.' = '.$id;
    new_($up, 'pp_db', $q);
    $this->message .= $this->message_upd;
    // update wrapper
    if($this->upd_wrapper != -1 && function_exists($this->upd_wrapper))
    {
      $wrapper = $this->upd_wrapper;
      $wrapper($id);
    }
    //
  }

  function del_data($listdel)
  {
    $tables = NULL;
    new_($del, 'pp_db');
    foreach( (explode(',', strtr($this->sql_fields, array(' ' => '')))) as $f)
      if( strstr($f, '(') )
      {
	preg_match('/(.*)\((.*)\)/', $f, $r);
	$tables[] = $r[2];
      }
    reset($listdel);
    while(list($k, $v) = each($listdel))
    {
      if(is_array($tables))
      {
	foreach($tables as $table)
	{
	  $del->query("DELETE
		       FROM
		       ".$table."
		       WHERE
		       ".$this->sql_key." = ".$k);
	}
      }
      $del->query("DELETE FROM ".$this->sql_table." WHERE ".$this->sql_key." = $k");
    }
    $this->message .= $this->message_del;
    if($this->del_wrapper != -1 && function_exists($this->del_wrapper))
    {
      $wrapper = $this->del_wrapper;
      $wrapper($listdel);
    }
  }

  function add_data($listadd)
  {
    // test if at least one needed field is empty :
    if($this->needed_fields != -1)
    {
      foreach( (explode(',', strtr($this->needed_fields, array(' ' => '')))) as $nf)
	if(strlen($listadd[$this->sql_table.'_'.$nf]) == 0)
	  return (0);
    }
    //
    $do_query = NULL;
    $q = 'INSERT INTO '.$this->sql_table.' SET '.$this->sql_key.' = \'\'';
    foreach( (explode(',', strtr($this->sql_fields, array(' ' => '')))) as $f)
      if(! strstr($f, '(') )
      {
	$data = addslashes($listadd[$this->sql_table.'_'.$f]);
	$do_query += strlen($data);
	// test if we must apply function when insert ( like PASSWORD() )
	if(is_array($this->function_on_field) && 
	   array_key_exists($f, $this->function_on_field))
	{
	  $fonc = $this->function_on_field[$f];
	  $q .= ', '.$f.' = '.$fonc.'(\''.$data.'\')';
	}
	else
	  $q .= ', '.$f.' = \''.$data.'\'';
	//
      }
    if($do_query > 0)
    {
      new_($add, 'pp_db', $q);
      $add->free();
      $this->message .= $this->message_add;
      if($this->add_wrapper != -1 && function_exists($this->add_wrapper))
      {
        $wrapper = $this->add_wrapper;
        $wrapper($add->insert_id());
      }
      return (1);
    }
    return (-1);
  }

  function compteur()
  {
    global $HTTP_POST_VARS;
    new_($db, 'pp_db');
    $nb = $db->get("COUNT(".$this->sql_key.")" , $this->sql_table, $this->get_where());
    $res = '<nobr>';
    if(! $nb)
      return;
    $nbp = (int) ($nb / $this->max_elements);
    if ($nbp * $this->max_elements != $nb)
      $nbp++;
    if($nbp <= 1)
      return '<font class="list">'._("Total").' : '.$nb.'</font></nobr>';
    new_($f, 'pp_form');
    $f->add_field('hidden', $this->id.'_want_page');
    if($this->current_page)
      $f->data .= 
	'<input type="image" src="/icons/previous.gif" 
	  onclick="document.'.$this->id.'.'.$this->id.'_want_page.value =\''.($this->current_page - 1).'\'">';
    for ($i = 0; $i < $nbp; $i++)
      if($i == $this->current_page)
	$f->add_field('button', ' '.$i.' ', 'style="background-color:#DEEEFF;font: bold 10px"');
      else
	$f->add_field(
	    'submit', ' '.$i.' ', 
	    'style="background-color:#CDDEEE;font: 10px" 
	     onclick="document.'.$this->id.'.'.$this->id.'_want_page.value =\''.$i.'\'"');
    if(($nbp - 1) > $this->current_page)
      $f->data .= 
	'<input type="image" src="/icons/next.gif" 
	  onclick="document.'.$this->id.'.'.$this->id.'_want_page.value =\''.($this->current_page + 1).'\'">';
    $res .= $f->data;
    $res .='<b>Total : '.$nb.'</b></nobr>';
    $res .= '</nobr>';
    return ($res);
  }

  function get_where()
  {
    $where = '1';
    if($this->search_string)
      $where .= $this->current_search_field." LIKE '%".$this->search_string."%'";
    if($this->additional_where_clause != -1)
      $where .= ' AND '.$this->additional_where_clause;
    return (strlen($where)) ? $where : 1;
  }

};

?>
