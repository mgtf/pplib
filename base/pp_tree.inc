<?
// $Id: pp_tree.inc,v 1.1 2002/01/15 15:22:12 tieum Exp $

class pp_tree_node extends pp_class
{
  var $node_name;
  var $node_id;
  var $node_id_father;
  var $node_id_sons;
  
  function pp_tree_node()
  {
    $this->pp_class();
    $this->node_name = -1;
    $this->node_id = -1;
    $this->node_id_father = -1;
    $this->node_id_sons = array();
  }
};

class pp_tree extends pp_class
{
  var $tree;
  var $nb_nodes;

  function pp_tree()
  {
    $this->pp_class();
    $this->tree = array();
    $this->nb_nodes = 0;
  }

  function dump()
  {
    global $pp;
    $pp->ob->suspend();
    print_r($this->tree);
    echo '<br>@@@@@@@@@@@@@@@@@@@@<br>';
    $n = new pp_tree_node();
    $n = $this->tree[0];
    print_r($n);
    echo '<br>@@@@@@@@@@@@@@@@@@@@<br>';
    print_r($this->tree_get_sons(3));
    return $pp->ob->resume();
  }

  function tree_add_node($name, $id_father = -1)
  {
    $node = new pp_tree_node();
    $id = $this->nb_nodes++;
    $node->node_name = $name;
    $node->node_id = $id;
    $node->node_id_father = $id_father;
    $this->tree[$id] =& $node;
    if($id_father != 1)
    {
      $father = new pp_tree_node;
      $father = & $this->tree[$id_father];
      $father->node_id_sons[] = $id;
    }
    return $id;
  }

  function tree_del_node($id)
  {
    if(! array_key_exists($id, $this->tree))
      return -1;
    $current = & $this->tree[$id];
    foreach($this->tree_get_sons($id) as $son)
      $this->tree[$son] = NULL;
    if(! array_key_exists($current->node_id_fahter, $this->tree))
      return -1;
    $parent = & $this->tree[$current->node_id_father] ;
    reset($parent->node_id_sons);
    $sons = array();
    // pas trouvé mieux pour supprimer l'id du noeud à supprimer 
    // dans la liste des fils du pêre que de la reconstruire en
    // sautant l'id en question : 
    foreach($parent->node_id_sons as $son)
      if($son != $id)
	$sons[] = $son;
    $parent->node_id_sons = $sons;
    //
    $current = NULL;
  }

  function tree_get_brothers($id)
  {
    if(! array_key_exists($id, $this->tree))
      return -1;
    $current = $this->tree[$id];
    if(! array_key_exists($current->node_id_fahter, $this->tree))
      return -1;
    $parent = $this->tree[$current->node_id_father];
    return (array) $parent->node_id_sons;
  }

  function tree_get_sons($id)
  {
    static $id_list;
    if(! array_key_exists($id, $this->tree))
      return;
    $current = $this->tree[$id];
    $id_list = array_merge($current->node_id_sons, $id_list);
    foreach($current->node_id_sons as $id_son)
      $id_list = $this->tree_get_sons($id_son);
    return (array) $id_list;
  }
};

?>
