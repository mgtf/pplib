<?

////
// $Id: mime.inc,v 1.12 2003/09/02 10:06:40 mgf Exp $
// !mime type class
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//
// public methods :
//
// none
//
// private methods :
//
// none
//
// consts :
//
// none

class mime_el extends _stdclass {
  var $mime_type;
  var $mime_subtype;
  var $mime_extentions;
  var $tag_start;
  var $tag_stop;
  var $blank_char;
  var $comment_start;
  var $comment_end;
  var $return_char;

  function mime_el() {
    $this->_stdclass();
    $this->mime_type = -1;
    $this->mime_subtype = -1;
    $this->mime_extentions = -1;
  }

  function set_mime_el($mime_type, $mime_subtype, $mime_extentions, $tag_start = -1, 
                    $tag_stop = -1, $blank_char = -1, $comment_start = -1, $comment_end = -1, $return_char = "\n") {
    if(strlen($mime_type) && 
       strlen($mime_subtype) && 
       is_array($mime_extentions)) {
      $this->mime_type = $mime_type;
      $this->mime_subtype = $mime_subtype;
      $this->mime_extentions = $mime_extentions;
      $this->tag_start = $tag_start;
      $this->tag_stop = $tag_stop;
      $this->blank_char = $blank_char;
      $this->comment_start = $comment_start;
      $this->comment_end = $comment_end;
      $this->return_char = $return_char;
      return 1;
    } else {
      return -1;
    }
  }

};

class mime extends mime_el {
  var $mimes;

  function mime() {
    $this->mime_el();
    // $mime_type, $mime_subtype, $mime_extentions, $tag_start, $tag_stop, $blank_char, $comment_start, $comment_end)
    $this->add_mime('text', 'html', array('html', 'shtml', 'htm'), '{', '}', '&nbsp;', '<!--', '-->', '<br>');
    $this->add_mime('text', 'xhtml', array('xhtml'), '{', '}', '&nbsp;', '<!--', '-->', '<br />');
    $this->add_mime('text', 'xml', array('xml'), '<', '>', ' ', '<!--', '-->', "\n");
    $this->add_mime('text', 'csv', array('csv'), '[', ']', ' ', ' : ', "\n", "\n");
    $this->add_mime('text', 'tex', array('tex'), '[', ']', '~', ' % ', "\n", "\n");
    $this->add_mime('application', 'pdf', array('pdf'));
    $this->add_mime('application', 'msword', array('doc'));
    $this->add_mime('image', 'jpeg', array('jpeg', 'jpg', 'jpe'));
    $this->add_mime('image', 'gif', array('gif'));
    $this->add_mime('image', 'png', array('png'));
    $this->add_mime('image', 'tiff', array('tiff', 'tif'));
    $this->add_mime('video', 'mpeg', array('mpeg', 'mpg', 'mpe'));
    $this->add_mime('video', 'quicktime', array('qt', 'mov'));
    $this->add_mime('video', 'x-msvideo', array('avi'));
    // ...
    // ...
  }

  function add_mime($mime_type, $mime_subtype, $mime_extentions, $tag_start = -1, 
                    $tag_stop = -1, $blank_char = -1, $comment_start = -1, $comment_end = -1, $return_char = "\n") {
    new_($new_mime, 'mime_el');
    if($new_mime->set_mime_el($mime_type, $mime_subtype, $mime_extentions, $tag_start, 
			      $tag_stop, $blank_char, $comment_start, $comment_end, $return_char) == -1 )
      user_error("mime::add_mime : "._("can't add mime type"), E_USER_ERROR);
    $this->mimes[] = $new_mime;
  }

  function get_mime($ext) {
    if(! strlen($ext))
      return 'application/octet-stream';
    foreach($this->mimes as $mime_type)
      if(in_array($ext, $mime_type->mime_extentions))
	return $mime_type->mime_type.'/'.$mime_type->mime_subtype;
    return 'application/octet-stream';
  }

  function set_mime($ext) {
    if(! strlen($ext))
      return -1;
    foreach($this->mimes as $mime_type) {
      if(in_array($ext, $mime_type->mime_extentions)) {
	$this->mime_type = $mime_type->mime_type;
	$this->mime_subtype = $mime_type->mime_subtype;
	$this->mime_extentions = $mime_type->mime_extentions;
	$this->tag_start = $mime_type->tag_start;
	$this->tag_stop = $mime_type->tag_stop;
	$this->blank_char = $mime_type->blank_char;
	$this->comment_start = $mime_type->comment_start;
	$this->comment_end = $mime_type->comment_end;
	$this->return_char = $mime_type->return_char;
	return 1;
      }
    }
    user_error("mime::set_mime : "._("this mime type is not supported"), E_USER_ERROR);
  }

  function get_doctype($mime_subtype) {
    switch($mime_subtype) {
      case 'html':
	return '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'."\n";
      case 'xhtml':
	return
	  '<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" '.
	  '"http://www.w3.org/TR/2000/REC-xhtml1-20000126/DTD/xhtml1-transitional.dtd">'."\n";
      case 'xml':
	return '<?xml version="1.0"?>'."\n";
    }
  }
};

?>
