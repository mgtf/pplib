<?

////
// $Id: user.inc,v 1.4 2003/08/28 15:22:45 mgf Exp $
// !user class
// Ce fichier est soumis � la licence d'utilisation
// contenue dans le fichier LICENCE
//
// public methods :
//
// private methods :
//
// none
//
// consts :
//
// none


class user extends _stdclass
{
  var $user_id;
  var $user_login;
  var $user_password;
  var $tpl;
  var $db;

  function user()
  {
    $this->class();
    $this->user_id = -1;
    $this->user_login = -1;
    $this->user_password = -1;
    new_($this->tpl, 'template');
    new_($this->db, 'db');
  }

  function create_table($drop_if_exists = FALSE)
  {
    new_($this->db, 'mysql');
    if($drop_if_exist != FALSE)
      $this->db->query('DROP TABLE IF EXISTS user');
    $this->db->query("
      CREATE TABLE 
      `user` (`user_id` INT UNSIGNED NOT NULL AUTO_INCREMENT, 
      `user_login` VARCHAR(12) NOT NULL, 
      `user_password` VARCHAR(12) NOT NULL , 
      PRIMARY KEY (`user_id`, `user_login`, `user_password`), 
      INDEX (`user_id`), 
      UNIQUE (`user_id`))
    ");
  }

  function add()
  {
    if($this->user_login == -1 || $this->user_password == -1)
      user_error("user::add : login or password empty or not set", E_USER_ERROR);
    $this->db->query("
	INSERT 
	INTO user 
	SET  user_login = '".$this->user_login."',
	     user_password = '".$this->user_password."'
	");
    $id = @mysql_insert_id();
    return ($id) ? $id : -1;
  }

  function remove()
  {
    if($this->user_id == -1)
      user_error("user::remove : please specify 'user_id' to remove", E_USER_ERROR);
    $this->db->query("
	DELETE 
	FROM user
	WHERE user_id = '".$this->user_id."'");
    return 1;
  }

  function select()
  {
    if(! $user_id)
      user_error("user::select : please specify 'user_id'", E_USER_ERROR);
    $this->db->query("
	SELECT user_login, user_password
	FROM user
	WHERE user_id = '".$this->user_id."'");
    if($this->db->next_record())
    {
      $this->user_login = $this->db->record['user_login'];
      $this->user_password = $this->db->record['user_password'];
      return 1;
    }
    else
      return -1;
  }

  function get_id()
  {
    if($this->user_login == -1 || $this->user_password == -1)
      user_error("user::add : login or password empty or not set", E_USER_ERROR);
    $this->db->query("
	SELECT user_id
	FROM user
	WHERE user_login = '".$this->user_login."' 
	AND user_password = '".$this->user_password."'");
    if($this->db->next_record())
      return ($this->user_id = $this->db->record['user_id']);
    else
      return -1;
  }
};

?>
