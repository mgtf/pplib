<?

////
//  $Id: mysql.inc,v 1.12 2002/04/04 09:29:11 tieum Exp $
//  mysql class
//  
//  public methods :
//
//  pp_db($query = "")
//  link_id() 
//  query_id() 
//  connect($database = "", $host = "", $user = "", $password = "") 
//  free() 
//  query($Query_String) 
//  next_record() 
//  seek($pos = 0) 
//  lock($table, $mode="write") 
//  unlock() 
//  affected_rows() 
//  num_rows() 
//  num_fields() 
//  nextid($seq_name) 
//  metadata($table='',$full=false) 
//
//  private methods :
//
//  table_names() 
//
//  consts :
//
//  DBHOST
//  DBDB
//  DBUSER
//  DBPWD
//

class pp_db extends pp_class {

  /* public: connection parameters */
  var $host;
  var $database;
  var $user;
  var $password;

  /* public: configuration parameters */
  var $auto_free     = 0;     ## Set to 1 for automatic mysql_free_result()

  /* public: result array and current row number */
  var $record   = array();
  var $row;

  /* private: link and query handles */
  var $link_id  = 0;
  var $query_id = 0;

  /* public: constructor */
  function pp_db($query = "") 
  {
    $this->pp_class();
    if((! defined("DBHOST") )||
       (! defined("DBDB") )||
       (! defined("DBUSER") ) ||
       (! defined("DBPWD")) )
      user_error("pp_db::pp_db : "._("db access was not configured"), E_USER_ERROR);
    $this->host = DBHOST;
    $this->database = DBDB;
    $this->user     = DBUSER;
    $this->password = DBPWD;
    $this->query($query);
    $this->debug = 0;
  }

  function get($field, $table, $where)
  {
    $row = $this->get_row($field, $table, $where);
    if (!$row)
      return -1;
    return ($row[0]);
  }

  function get_row($fields, $table, $where)
  {
    $query = "SELECT $fields
	      FROM   $table
	      WHERE  $where ";
    if (strstr($where, 'LIMIT') === false)
      $query .= "LIMIT  1";
    if ($this->debug)
      user_error($query, E_USER_WARNING);
    $this->query($query);
    if (!$this->next_record())
      return -1;
    $row = $this->record;
    $this->free();
    if (!$row)
      return -1;
    return ($row);
  }

  function connect($database = "", $host = "", $user = "", $password = "") {
    /* Handle defaults */
    if ("" == $database)
      $database = $this->database;
    if ("" == $host)
      $host     = $this->host;
    if ("" == $user)
      $user     = $this->user;
    if ("" == $password)
      $password = $this->password;

    /* establish connection, select database */
    if ( 0 == $this->link_id ) {
      $this->link_id = @mysql_pconnect($host, $user, $password);
      if (!$this->link_id)
	user_error("pp_db::connect : "._("can't connect to")." '$host'", E_USER_ERROR);
      if (! @mysql_select_db($database,$this->link_id)) {
	user_error("pp_db::connect : "._("cannot use database")." ".$this->database, E_USER_ERROR);
      }
    }

    return $this->link_id;
  }

  /* public: discard the query result */
  function free() 
  {
    if(is_resource($this->query_id))
      @mysql_free_result($this->query_id);
    $this->query_id = 0;
  }

  /* public: perform a query */
  function query($query) 
  {
    if ($query == "")
      return 0;
    if(!$this->connect() )
      return 0;
    if ($this->query_id)
      $this->free();
    $this->query_id = @mysql_query($query,$this->link_id);
    $this->row   = 0;
    $errno = mysql_errno();
    $error = mysql_error();
    if (! $this->query_id)
      user_error(
	  "pp_db::query : "._("invalid sql").": ".$query."\n".
	  $error."(".$errno.")", E_USER_ERROR);
    else
      return $this->query_id;
  }

  /* public: get last insert id */
  function insert_id()
  {
    return mysql_insert_id($this->link_id);
  }

  /* public: walk result set */
  function next_record() 
  {
    if (!$this->query_id)
      user_error("pp_db::next_record : "._("next_record called with no query pending"), E_USER_ERROR);
    $this->record = @mysql_fetch_array($this->query_id);
    $this->row   += 1;
    $stat = is_array($this->record);
    if (! $stat && $this->auto_free) {
      $this->free();
    }
    return $stat;
  }

  /* public: position in result set */
  function seek($pos = 0) {
    $status = @mysql_data_seek($this->query_id, $pos);
    if ($status)
      $this->row = $pos;
    else {
      user_error("pp_db::seek : seek($pos) failed: result has ".$this->num_rows()." rows", E_USER_ERROR);

      /* half assed attempt to save the day,
       * but do not consider this documented or even
       * desireable behaviour.
       */
      @mysql_data_seek($this->query_id, $this->num_rows());
      $this->row = $this->num_rows;
      return 0;
    }

    return 1;
  }

  /* public: table locking */
  function lock($table, $mode="write") {
    $this->connect();

    $query="lock tables ";
    if (is_array($table)) {
      while (list($key,$value)=each($table)) {
        if ($key=="read" && $key!=0) {
          $query.="$value read, ";
        } else {
          $query.="$value $mode, ";
        }
      }
      $query=substr($query,0,-2);
    } else {
      $query.="$table $mode";
    }
    $res = @mysql_query($query, $this->link_id);
    if (!$res) {
      user_error("pp_db::lock : lock($table, $mode) failed.", E_USER_ERROR);
      return 0;
    }
    return $res;
  }

  function unlock() {
    $this->connect();

    $res = @mysql_query("unlock tables");
    if (!$res) {
      user_error("pp_db::unlock : unlock failed.", E_USER_ERROR);
      return 0;
    }
    return $res;
  }


  /* public: evaluate the result (size, width) */
  function affected_rows() {
    return @mysql_affected_rows($this->link_id);
  }

  function num_rows() {
    return @mysql_num_rows($this->query_id);
  }

  function num_fields() {
    return @mysql_num_fields($this->query_id);
  }

  function table_names() {
    $this->query("SHOW TABLES");
    $i=0;
    while ($info=mysql_fetch_row($this->query_id))
    {
      $return[$i]["table_name"]= $info[0];
      $return[$i]["tablespace_name"]=$this->database;
      $return[$i]["database"]=$this->database;
      $i++;
    }
    return $return;
  }
};

?>
