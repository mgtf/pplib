<?

////
//  $Id: pp_list.inc,v 1.2 2002/01/18 10:23:03 tieum Exp $
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
  var $current_page;

  function pp_list()
  {
    $this->pp_template();
    $this->tpl = 'list_default.html';
    $this->order = 'ASC';
    $this->current_page = 0;
    if(defined('LIST_MAX_ELEMENTS'))
      $this->max_elements = LIST_MAX_ELEMENTS;
    else
      $this->max_elements = 10;
    $this->search = '';
    $this->bg_line = array( 0 => '#CEDEEF',
			    1 => '#FFFFFF');
    $this->message_del = '[ suppression effectuée ]';
    $this->message_add = '[ ajout effectué ]';
    $this->message_upd = '[ modification effectuée ]';
    $this->id = 'wow';
  }

  function parse_list()
  {
    global $HTTP_POST_VARS;
    if(! strlen($this->sql_fields) )
      $this->error("parse_list::parse_list : you must specify sql fields");
    if(! strlen($this->sql_table) )
      $this->error("parse_list::parse_list : you must specify sql table");
    if(! strlen($this->sql_key) )
      $this->error("parse_list::parse_list : you must specify primary key");
    if(! strlen($this->order_fields) )
      $this->order_fields = $this->sql_key;
    // actions : 
    $listdel = $HTTP_POST_VARS[$this->id.'_listdel'];
    $listadd = $HTTP_POST_VARS[$this->id.'_listadd'];
    $list_ids = $HTTP_POST_VARS[$this->id.'_list_ids'];
    $was_updated = $HTTP_POST_VARS[$this->id.'_was_updated'];
    $want_save = $HTTP_POST_VARS[$this->id.'_want_save'];
    if(is_array($listdel) && $want_save)
      $this->del_data($listdel);
    if(is_array($listadd) && $want_save)
      $this->add_data($listadd);
    if(is_array($list_ids) && $want_save)
      foreach($list_ids as $i)
	if($HTTP_POST_VARS[$id.'_was_updated'.$i])
	  $this->upd_data($i);
    //
    $this->get_vars();
    $this->set_tags();
    $this->get_data();
    $str = ($this->current_environment[name] == 'html') ? $this->parse_template_file("list.js") : '';
    $str .= $this->parse_template_file($this->tpl);
    return $str;
  }

  /////////////////////////////////////////////////////////////////////////////////
  ////////////// PRIVATE FUNCTIONS ////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////

  function get_vars()
  {
    global $HTTP_POST_VARS;
    $this->search_string = ($HTTP_POST_VARS[$this->id.'_search_string']) ? 
                            $HTTP_POST_VARS[$this->id.'_search_string'] : '';
    $this->current_search_field = $HTTP_POST_VARS[$this->id.'_current_search_field'];
    $this->max_elements = ($HTTP_POST_VARS[$this->id.'_max_elements']) ? 
                          $HTTP_POST_VARS[$this->id.'_max_elements'] : $this->max_elements;
    $this->current_page = ($HTTP_POST_VARS[$this->id.'_current_page']) ? 
                          $HTTP_POST_VARS[$this->id.'_current_page'] : 0 ;
    $this->current_order_field = ($HTTP_POST_VARS[$this->id.'_current_order_field']) ? 
                                 ($HTTP_POST_VARS[$this->id.'_current_order_field']) : $this->sql_key;
    $this->order = $HTTP_POST_VARS[$this->id.'_order'];
    if($HTTP_POST_VARS[$this->id.'_want_page'])
      $this->current_page = $HTTP_POST_VARS[$this->id.'_want_page'];
    $this->search = $HTTP_POST_VARS[$this->id.'_list_search'];
    $this->start = $this->current_page * $this->max_elements;
  }

  function set_tags()
  {
    $this->add_tag_array(array($this->id.'_search_string' => $this->search_string,
			       $this->id.'_current_search_field' => $this->current_search_field,
			       $this->id.'_max_elements' => $this->max_elements,
			       $this->id.'_list_mesg' => $this->message,
			       $this->id.'_current_order_field' => $this->current_order_field,
			       $this->id.'_order' => $this->order,
			       'id' => $this->id,
			       $this->id.'_want_page' => 0,
			       $this->id.'_want_save' => 0,
			       $this->id.'_list_submit' => '
			       <input type=submit value="enregistrer" onclick="document.list'.$this->id.'.'.$this->id.'_want_save.value =\'1\'">
			       <font size=1><input type="button" value="inverser la séléction" onclick="javascript:invertsel(\''.$this->id.'\')"></font>
    '));
    $this->add_tag($this->id.'_list_hidden_fields', hidden_fields);
    $this->add_tag($this->id.'_list_search', list_search);
    $this->add_tag($this->id.'_list_compteur', compteur);
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
	$list_check_update = 'onChange="document.list'.$this->id.'.'.$this->id.'_was_updated'.$k.'.value =\'1\'"';
	$data[$i][$this->id.'_bgcolor'] = $this->bg_line[($i % 2)];
	$data[$i][$this->id.'_id_list'] = $k;
	$data[$i][$this->id.'_list_check_update'] = $list_check_update;
	$data[$i][$this->id.'_list_del'] = '<input type="checkbox" name="'.$this->id.'_listdel['.$k.']">';
        // echo "@@@".$this->id.'_'.$f."@@@";
	$data[$i][$this->id.'_'.$f] = ($sql->Record[$f]) ? 
	                stripslashes( strtr($sql->Record[$f], array('"' => '')) )
			: '';
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
    $form->data .= '<font size=1>Rechercher </font>';
    $form->add_field("text", $this->id."_search_string", "size=\"10\"");
    $form->data .= '<font size=1>dans </font>';
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
    $form->data .= '<font size=1>en affichant </font>';
    $form->add_field("select", $this->id."_max_elements", "onChange=document.".$this->id."_list_search.submit()");
    foreach($choices as $choice)
      $form->add_field("option", $choice, 
		       ($choice == $this->max_elements) ? 'SELECTED' : '', $choice);
    $form->add_field("end_select", "");
    // classer par :
    $form->data .= '<font size=1>éléments par page.<br>Classer par </font>';
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
    $form->data .= '<font size=1> par ordre </font>';
    $form->add_field("select", $this->id."_order", "onChange=document.".$this->id."_list_search.submit()");
    $form->add_field('option', 'croissant', ($this->order == 'ASC') ? 'SELECTED' : '', 'ASC');
    $form->add_field('option', 'décroissant', ($this->order == 'DESC') ? 'SELECTED' : '', 'DESC');
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
	$q .= ', '.$f.' = \''.$data.'\'';
      }
    $q .= ' WHERE '.$this->sql_key.' = '.$id;
    new_($up, 'pp_db', $q);
    $up->free();
    $this->message .= $this->message_upd;
  }

  function del_data($listdel)
  {
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
  }

  function add_data($listadd)
  {
    $q = 'INSERT INTO '.$this->sql_table.' SET '.$this->sql_key.' = \'\'';
    foreach( (explode(',', strtr($this->sql_fields, array(' ' => '')))) as $f)
      if(! strstr($f, '(') )
      {
	$data = addslashes($listadd[$this->sql_table.'_'.$f]);
	$do_query += strlen($data);
	$q .= ', '.$f.' = \''.$data.'\'';
      }
    if($do_query > 0)
    {
      new_($add, 'pp_db', $q);
      $add->free();
      $this->message .= $this->message_add;
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
      return '<b>Total : '.$nb.'</b></nobr>';
    new_($f, 'pp_form');
    $f->add_field('hidden', $this->id.'_want_page');
    if($this->current_page)
      $f->add_field('submit', '[Précédent]', 
		    'onclick="document.list'.$this->id.'.'.$this->id.'_want_page.value =\''.($this->current_page - 1).'\'"');
    for ($i = 0; $i < $nbp; $i++)
      if($i == $this->current_page)
	$f->add_field('button', ' '.$i.' ');
      else
	$f->add_field('submit', ' '.$i.' ', 'onclick="document.list'.$this->id.'.'.$this->id.'_want_page.value =\''.$i.'\'"');
    if(($nbp - 1) > $this->current_page)
      $f->add_field('submit', '[Suivant]',
		    'onclick="document.list'.$this->id.'.'.$this->id.'_want_page.value =\''.($this->current_page + 1).'\'"');
    $res .= $f->data;
    $res .='<b>Total : '.$nb.'</b></nobr>';
    $res .= '</nobr>';
    return ($res);
  }

  function get_where()
  {
    return (($this->search_string) ? 
	   ($this->current_search_field." LIKE '%".$this->search_string."%'") : 
	   "1");
  }

};

?>
