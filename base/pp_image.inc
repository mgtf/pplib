<?
// $Id: pp_image.inc,v 1.10 2002/02/13 12:02:53 tieum Exp $

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
    if(! is_resource($tmp->image) )
      $this->error('pp_image::image_resize : '._("can't create temp image"));
    imagecopyresampled($tmp->image, $this->image, 0, 0, 0, 0, 
	               $w, $h, $this->xsize, $this->ysize);
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

  function myimagecreatefromgif()
  {
    global $pp;
    $pp->ob->suspend();
    system("/usr/bin/gif2png -d -O ".$this->filename);
    system("mv ".$this->filename.'.png '.$this->filename);
    $pp->ob->resume();
    $this->mime_subtype = 'png';
    return imagecreatefrompng($this->filename);
  }

  function image_create_from_file()
  {
    if($this->mime_subtype == -1)
    {
      $ext = split('\.', basename($this->filename)); 
      if(array_key_exists(1, $ext))
	$ext = $ext[1];
      else
	$this->error('pp_file::image_create_from_file : '.
	              _("image")." '".$this->filename."' "._("has no extention"));
      $this->mime_subtype = $ext;
    }
    switch($this->mime_subtype)
    {
      case 'jpg':
	$tmp_img = imagecreatefromjpeg($this->filename);
	$this->truecolor = 1;
	break;
      case 'gif':
	$tmp_img = $this->myimagecreatefromgif($this->filename);
	break;
      case 'png':
	$tmp_img = imagecreatefrompng($this->filename);
	break;
      default:
	return -1;
    }
    if(! is_resource($tmp_img) )
      $this->error('pp_file::image_create_from_file : '.
	           _("can't create image from")." '".$this->filename."'");
    $this->xsize = ImageSX($tmp_img);
    $this->ysize = ImageSY($tmp_img);
    $this->set_mime($this->mime_subtype);
    $this->image_create();
    imagecopy($this->image, $tmp_img, 0, 0, 0, 0, $this->xsize, $this->ysize);
    if(! is_resource($this->image) )
      $this->error(_("can't create image from")." '".$this->filename."'");
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

  //
  // to be continued ..
  //
  // function image_mirror($axe = 'X')
  // {
  //   new_($tmp, 'pp_image');
  //   $tmp->xsize = $this->xsize;
  //   $tmp->ysize = $this->ysize;
  //   $tmp->truecolor = 0;
  //   $tmp->image_create();
  //   for($i = 1; $i <= $tmp->xsize ; $i++)
  //     for($j = 1; $j <= $tmp->ysize ; $j++)
  //     {
  //       // die($i.$j);
  //       // $col = imagecolorat($this->image, $i, $j);
  //       $col = 'CEDEEF';//  imagecolorat($this->image, 10, 10);
  //       imagesetpixel($tmp->image, $tmp->xsize, $tmp->ysize - $j, $col);
  //     }
  //   $this->image_copy($tmp->image);
  //   // $this = $tmp;
  // }

  // function image_copy($id)
  // {
  //   $w = ImageSX($id);
  //   $h = ImageSY($id);
  //   ImageCopy($id, $this->image, 0, 0, 0, 0, $w, $h);
  // }
  //
  //

  function image_get_data()
  {
    if(! array_key_exists($this->mime_subtype, $this->formats))
      $this->error("'".$this->mime_subtype."' "._("format is not supported"));
    global $pp;
    $pp->ob->suspend();
    $f = $this->formats[$this->mime_subtype];
    if($this->mime_subtype == 'jpg' && $this->quality != -1)
      $f($this->image, NULL, $this->quality);
    else
      $f($this->image);
    $img_data = $pp->ob->resume();
    $this->data = $img_data;
    $this->image_update_infos();
    return $this->data;
  }

};

?>
