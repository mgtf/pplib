<?

////
//  $Id: user.inc,v 1.1 2002/01/15 15:22:12 tieum Exp $
//  user class
//
//  public methods :
//
//  private methods :
//
//  none
//
//  consts :
//
//  none


class pp_user extends pp_class
{
  var $user_id;
  var $user_login;
  var $user_password;
  var $tpl;
  var $db;

  function pp_user()
  {
    $this->pp_class();
    $this->user_id = -1;
    $this->user_login = -1;
    $this->user_password = -1;
    new_($this->tpl, 'pp_template');
    new_($this->db, 'pp_db');
  }

  function create_table($drop_if_exists = FALSE)
  {
    new_($this->db, 'pp_mysql');
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
      $this->error("user::add : login or password empty or not set");
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
      $this->error("user::remove : please specify 'user_id' to remove");
    $this->db->query("
	DELETE 
	FROM user
	WHERE user_id = '".$this->user_id."'");
    return 1;
  }

  function select()
  {
    if(! $user_id)
      $this->error("user::select : please specify 'user_id'");
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
      $this->error("user::add : login or password empty or not set");
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
