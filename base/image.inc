<?
// $Id: image.inc,v 1.1 2002/01/15 15:22:11 tieum Exp $

class pp_image extends pp_file
{
  var $image;
  var $xsize;
  var $ysize;
  var $truecolor;
  var $alphablending;
  // pour les images de type 'jpg'.
  var $quality;
  // si dither = 1 => image plus tachetée mais avec 
  // une meilleure approximation des couleurs.
  var $dither; 
  var $interlace;
  var $family;
  var $formats;
  var $colors_total;

  // private :
  var $tpl;

  function pp_image()
  {
    $this->pp_file();
    $this->image = -1;
    $this->truecolor = TRUE;
    $this->interlace = FALSE;
    $this->dither = FALSE;
    $this->alphablending = FALSE;
    $this->colors_total = -1;
    $this->xsize = -1;
    $this->ysize = -1;
    $this->formats = array(
	'gif' => 'imagegif',
	'png' => 'imagepng',
	'jpg' => 'imagejpeg',
	'jpeg' => 'imagejpeg'
	);
    $this->mime_type = 'image';
    $this->quality = -1;
  }

  function image_resize($w = 'p', $h = 'p')
  {
    if(strstr($w, '%') && $h == 'p')
    {
      $w = substr($w , 0, strlen($w) - 1);
      $ratio = ($w / 100);
      $w = (int) ($this->xsize * $ratio);
      $h = (int) ($this->ysize * $ratio);
    }
    else
    {
      if(strstr($w, '%'))
	$this->error(_("can't use '%' with another arg"));
      if($w == 'p' && $h == 'p')
	$this->error(_("you must specify at least one size"));
      $ratio = ($this->xsize / $this->ysize);
      $h = ($h == 'p') ? (int) ($w / $ratio) : $h;
      $w = ($w == 'p') ? (int) ($h * $ratio) : $w;
    }
    new_($tmp, get_class($this));
    $tmp->xsize = $w;
    $tmp->ysize = $h;
    $tmp->mime_subtype = $this->mime_subtype;
    $tmp->truecolor = 1;
    $tmp->image_create();
    imagecopyresampled($tmp->image, $this->image, 0, 0, 0, 0, $w, $h,
                       $this->xsize, $this->ysize);
    $this = $tmp;
    $this->image_update_infos();
    $this->image_get_data();
  }

  function image_to_file($filename, $force = 0)
  {
    $this->filename = $filename;
    $this->write($force);
  }

  function image_allocate_color($color)
  {
    str_replace('#', '', $color);
    $color = hexdec($color);
    $red = ($color & 0xFF0000) >> 16;
    $green = ($color & 0xFF00) >> 8;
    $blue = ($color & 0xFF);
    return (imagecolorallocate($this->image, $red, $green, $blue));
  }

  function image_free()
  {
    imagedestroy($this->image);
  }

  function image_convert($format)
  {
    if($format == $this->format)
      $this->warning("pp_image::image_convert : "._("image is already a")." .".$format);
    if(! array_key_exists($format, $this->formats))
      $this->error("pp_image::image_convert : "._("format")." ".$format." "._("is not supported"));
    $f = $this->format.'2'.$format;
    $this->$f();
  }

  function image_update_infos()
  {
    $this->xsize = ImageSX($this->image);
    $this->ysize = ImageSY($this->image);
    $this->colors_total = imagecolorstotal($this->image);
  }

  function image_create_from_file($filename)
  {
    $ext = split('\.', basename($filename)); 
    $ext = $ext[1];
    $this->mime_subtype = $ext;
    switch($this->mime_subtype)
    {
      case 'jpg':
	$create_func = 'imagecreatefromjpeg';
	$this->truecolor = 1;
	break;
      case 'gif':
	$create_func = 'imagecreatefromgif';
	break;
      case 'png':
	$create_func = 'imagecreatefrompng';
	break;
      default:
	return -1;
    }
    $tmp_img = $create_func($filename);
    $this->xsize = ImageSX($tmp_img);
    $this->ysize = ImageSY($tmp_img);
    $this->set_mime($ext);
    $this->image_create();
    imagecopy($this->image, $tmp_img, 0, 0, 0, 0, $this->xsize, $this->ysize);
    imagedestroy($tmp_img);
    $this->image_update_infos();
    $this->image_get_data();
  }

  function image_create()
  {
    if( (! $this->xsize > 0) || (! $this->ysize > 0) )
      $pp->error("pp_image::image_create :"._("no image size"));
    if($this->truecolor == TRUE) 
    {
      $this->image = imagecreatetruecolor($this->xsize, $this->ysize);
      if($this->alphablending == TRUE)
	imagealphablending($this->image, TRUE);
    }
    else
      $this->image = imagecreate($this->xsize, $this->ysize);
    if(! is_resource($this->image))
      $this->error("pp_image::image_create : "._("can't create image"));
    if($this->interlace)
      imageinterlace($this->image, TRUE);
    $this->image_update_infos();
    return 1;
  }

  function image_get_data()
  {
    if(! array_key_exists($this->mime_subtype, $this->formats))
      $this->error("'".$this->mime_subtype."' "._("format is not supported"));
    if(strlen($this->data) > 0)
      return $this->data;
    global $pp;
    $pp->ob->suspend();
    $f = $this->formats[$this->mime_subtype];
    if($this->mime_subtype == 'jpg' && $this->quality != -1)
      $f($this->image, NULL, $this->quality);
    else
      $f($this->image);
    $img = $pp->ob->resume();
    $this->data = $img;
    $this->image_update_infos();
    return $this->data;
  }

};

?>
