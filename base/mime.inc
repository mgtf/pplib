<?

////
//  $Id: mime.inc,v 1.2 2002/02/12 15:45:17 tieum Exp $
//  mime type class
//
//  public methods :
//
//  none
//
//  private methods :
//
//  none
//
//  consts :
//
//  none

class pp_mime_el extends pp_class
{
  var $mime_type;
  var $mime_subtype;
  var $mime_extentionss;
  var $tag_start;
  var $tag_stop;
  var $blank_char;
  var $comment_start;
  var $comment_end;

  function pp_mime_el()
  {
    $this->pp_class();
    $this->mime_type = -1;
    $this->mime_subtype = -1;
    $this->mime_extentions = -1;
  }

  function set_mime_el($mime_type, $mime_subtype, $mime_extentions, $tag_start = -1, 
                    $tag_stop = -1, $blank_char = -1, $comment_start = -1, $comment_end = -1)
  {
    if(strlen($mime_type) && 
       strlen($mime_subtype) && 
       is_array($mime_extentions))
    {
      $this->mime_type = $mime_type;
      $this->mime_subtype = $mime_subtype;
      $this->mime_extentions = $mime_extentions;
      $this->tag_start = $tag_start;
      $this->tag_stop = $tag_stop;
      $this->blank_char = $blank_char;
      $this->comment_start = $comment_start;
      $this->comment_end = $comment_end;
      return 1;
    }
    else
      return -1;
  }

};

class pp_mime extends pp_mime_el
{
  var $mimes;

  function pp_mime()
  {
    $this->pp_mime_el();
    // $mime_type, $mime_subtype, $mime_extentions, $tag_start, $tag_stop, $blank_char, $comment_start, $comment_end)
    $this->add_mime('text', 'html', array('html', 'shtml', 'htm'), '{', '}', '&nbsp;', '<!--', '-->');
    $this->add_mime('text', 'csv', array('csv'), '[', ']', ' ', ' : ', "\n");
    $this->add_mime('text', 'tex', array('tex'), '[', ']', '~', ' % ', "\n");
    $this->add_mime('image', 'jpeg', array('jpeg', 'jpg', 'jpe'));
    $this->add_mime('image', 'png', array('png'));
    $this->add_mime('image', 'tiff', array('tiff', 'tif'));
    $this->add_mime('video', 'mpeg', array('mpeg', 'mpg', 'mpe'));
    $this->add_mime('video', 'quicktime', array('qt', 'mov'));
    $this->add_mime('video', 'x-msvideo', array('avi'));
    // ...
    // ...
  }

  function add_mime($mime_type, $mime_subtype, $mime_extentions, $tag_start = -1, 
                    $tag_stop = -1, $blank_char = -1, $comment_start = -1, $comment_end = -1)
  {
    new_($new_mime, 'pp_mime_el');
    if($new_mime->set_mime_el($mime_type, $mime_subtype, $mime_extentions, $tag_start, 
			      $tag_stop, $blank_char, $comment_start, $comment_end) == -1 )
      $this->error("pp_mime::add_mime : "._("can't add mime type"));
    $this->mimes[] = $new_mime;
  }

  function set_mime($ext)
  {
    if(! strlen($ext))
      return -1;
    foreach($this->mimes as $mime_type)
    {
      if(in_array($ext, $mime_type->mime_extentions))
      {
	$this->mime_type = $mime_type->mime_type;
	$this->mime_subtype = $mime_type->mime_subtype;
	$this->mime_extentions = $mime_type->mime_extentions;
	$this->tag_start = $mime_type->tag_start;
	$this->tag_stop = $mime_type->tag_stop;
	$this->blank_char = $mime_type->blank_char;
	$this->comment_start = $mime_type->comment_start;
	$this->comment_end = $mime_type->comment_end;
	return 1;
      }
    }
    $this->error("pp_mime::get_mime : "._("this format is not supported"));
  }

};

?>
