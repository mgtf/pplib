<?

////
//  $Id: pp_data.inc,v 1.2 2002/04/04 09:29:11 tieum Exp $
//  data class
//
//  public methods :
//
//  private methods :
//
//  none
//
//  consts :
// 
//  STORAGE_PATH


class pp_data extends pp_file
{
  var $db;
  var $storage_type; // file or db
  var $data_size; // data size
  var $data_field;
  var $data; // data !
  var $id;
  var $storage_path;
  var $file;

  function pp_data()
  {
    $this->pp_file();
    $this->storage_type = 'file';
    if(defined('STORAGE_PATH'))
      $this->storage_path = STORAGE_PATH;
    $this->data = '';
    $this->data_size = -1;
  }

  function data_create_table($drop_if_exist = FALSE)
  {
    new_($this->db, 'pp_db');
    if($drop_if_exist == TRUE)
      $this->db->query('DROP TABLE IF EXISTS data');
    $this->db->query("
      CREATE TABLE data (
	data_id int(10) unsigned NOT NULL auto_increment,
	storage_type tinyint(1) unsigned NOT NULL default '0',
	size tinyint(1) unsigned NOT NULL default '0',
	tinyblobfield tinyblob,
	blobfield blob,
	mediumblobfield mediumblob,
	longblobfield longblob,
	PRIMARY KEY  (data_id),
	UNIQUE KEY data_id (data_id),
	KEY data_id_2 (data_id,storage_type),
	KEY size (size)
      )
    ");
  }

  function data_get($id)
  {
    if($id <= 0)
      return -1;
    new_($this->db, 'pp_db');
    $this->db->query("SELECT * FROM data WHERE data_id = '$id'");
    if(! $this->db->next_record())
      return -1;
    $this->id = $id;
    $this->data_size = $this->db->record['size'];
    switch($this->data_size)
    {
      case 1: $this->data = $this->db->record['tinyblobfield']; break;
      case 2: $this->data = $this->db->record['blobfield']; break;
      case 3: $this->data = $this->db->record['mediumblobfield']; break;
      case 4: $this->data = $this->db->record['longblobfield']; break;
    }
    return (strlen($this->data) ? 1 : -1);
  }

  function data_store()
  {
    if(! $this->storage_path)
      user_error("pp_data::store : "._("constant STORAGE_PATH not set"), E_USER_ERROR);
    new_($this->db, 'pp_db');
    $this->get_data_size();
    switch($this->storage_type)
    {
      case 'db':
	$this->get_data_size();
	$this->db->query("INSERT INTO data 
	                  SET         size = '".$this->size."', 
			              ".$this->data_field." = '".$this->data."', 
				      storage_type = '0'");
	$id = @mysql_insert_id();
	return ($id >= 0) ? $id : -1;
	break;
      case 'file':
	$this->db->query("INSERT INTO data SET storage_type = '1'");
	$id = @mysql_insert_id();
	if($id <= 0)
	  user_error("pp_data::store : "._("can't get last insert id"), E_USER_ERROR);
	$this->filename = $this->storage_path.$id;
	$this->write();
	return ($id >= 0) ? $id : -1;
	break;
    }
  }

  function get_data_size()
  {
    if(! $datasize = strlen($this->data))
      user_error("pp_data::store : "._("no data to store"), E_USER_ERROR);
    if( $datasize < 2^8 )
    {
      $this->data_size = 1;
      $this->data_field = 'tinyblobfield';
    }
    elseif($datasize < 2^16 )
    {
      $this->data_size = 2;
      $this->data_field = 'blobfield';
    }
    elseif($datasize < 2^24 )
    {
      $this->data_size = 3;
      $this->data_field = 'mediumblobfield';
    }
    elseif($datasize < 2^32 )
    {
      $this->data_size = 4;
      $this->data_field = 'longblobfield';
    }
    else
      user_error("data::store : "._("data size too big"), E_USER_ERROR);
  }

  function data_update($id, $data)
  {
    new_($this->db, 'pp_db');
    $storage_type = $this->db->get('storage_type', 'data', 'data_id = '.$id);
    switch($storage_type)
    {
      case 0:
	$this->data_data_size($data);
	$old_size = $this->db->get('size', 'data', 'data_id = '.$id);
	switch($old_size)
	{
	  case 1: $old_field = 'tinyblobfield'; break;
	  case 2: $old_field = 'blobfield'; break;
	  case 3: $old_field = 'mediumblobfield'; break;
	  case 4: $old_field = 'longblobfield'; break;
	  default: return -1;
	}
	if($this->field != $old_field)
	  $this->db->query("UPDATE data 
	                    SET ".$this->field." = '$data', $old_field = '', size = '".$this->size."'
	                    WHERE id_data = '$id'");
	else
	  $this->db->query("UPDATE data SET ".$this->field." = '$data' WHERE id_data = '$id'");
	return 1;
      case 1:
	$this->filename = $this->storage_path.$id;
	$this->data = $data;
	$this->write();
      default:
	return -1;
    }
  }

  function data_delete($id)
  {
    if($id <= 0)
      return -1;
    new_($this->db, 'pp_db');
    $this->db->query("DELETE FROM data WHERE data_id = '$id'");
    $filename = $this->storage_path.$id;
    if(file_exists($filename))
      unlink($filename);
    return 1;
  }
};

?>
