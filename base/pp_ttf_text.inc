<?
// $Id: pp_ttf_text.inc,v 1.9 2002/02/25 13:20:26 tieum Exp $
////
// Paramètres:
// text    	: texte à afficher
// color   	: couleur du texte      (defaut: noir)
// bgcolor 	: couleur du fond       (defaut: blanc)
// size    	: taille de la police   (defaut: 24)
// font    	: police                (defaut: arial)
// angle   	: angle d'affichage     (defaut: 0)
// margin  	: marge autour du texte (defaut: 0)
// antialias    : antialising           (defaut: 0)
// xsize   	: largeur               (defaut: auto)
// ysize  	: hauteur               (defaut: auto)
// align   	: alignement horizontal (defaut: center)
// valign  	: alignement vertical   (defaut: center)

// consts :
//
// TT_PATH

class pp_ttf_text extends pp_image
{
  var $text;
  var $color;
  var $bgcolor;
  var $size;
  var $font;
  var $fonts_path;
  var $shadow_color;
  var $shadow_decal;
  var $angle;
  var $margin;
  var $antialias;
  var $align;
  var $valign;
  var $force_xpos;
  var $force_ypos;

  // private vars :
  var $txt_xsize;
  var $txt_ysize;

  function pp_ttf_text()
  {
    $this->pp_image();
    // very important not to use truecolor image:
    $this->truecolor = FALSE;
    $this->txt_xsize = -1;
    $this->txt_ysize = -1;
    $this->size = 24;
    $this->angle = 0;
    if(defined('TTF_DEFAULT'))
      $this->font = TTF_DEFAULT;
    else
      $this->font = 'arial.ttf';
    $this->fonts_path = -1;
    if(defined('TTF_PATH'))
      $this->fonts_path = TTF_PATH;
    $this->margin = 0;
    $this->antialias = TRUE;
    $this->xsize = -1;
    $this->ysize = -1;
    $this->bgcolor = "FFFFFF";
    $this->color = "000000";
    $this->align = '';
    $this->valign = '';
    $this->shadow_color = -1;
    $this->shadow_decal = 3;
    $this->mime_subtype = 'png';
    $this->mime_type = 'image';
    $this->force_xpos = -1;
    $this->force_ypos = -1;
    $this->force_xsize = -1;
    $this->force_ysize = -1;
  }

  function create_ttf_text()
  {
    if($this->fonts_path == -1)
      $this->error("pp_ttf_text::create_ttf_text : ".
		   _("please set")." TTF_PATH");
    $font = $this->fonts_path . $this->font;
    if(! file_exists($font))
      $this->error("pp_ttf_text::create_ttf_text : ".
	           _("font file")." '".$font."' "._("does not exist."));
    if(strlen($this->text) < 1)
      $this->error("pp_ttf_text::create_ttf_text : ".
		   _("no text")." !");
    $pos = $this->ttf_text_get_size();
    if($this->force_xpos != -1)
      $pos[0] = $this->force_xpos;
    if($this->force_ypos != -1)
      $pos[1] = $this->force_ypos;
    if($this->force_xsize != -1)
      $this->xsize = $this->force_xsize;
    if($this->force_ysize != -1)
      $this->ysize = $this->force_ysize;
    $this->image_create();
    if ($this->bgcolor == 'trans')
    {
      if($this->antialias == TRUE)
	$this->error("pp_ttf_text::create_ttf_text : ".
	             _("using both transparent background and antialiasing produce poor quality images !"));
      $back = imagecolortransparent($this->image, 
	                            $this->image_allocate_color("000000") );
    }
    else
      $back = $this->image_allocate_color($this->bgcolor);
    $fore = $this->image_allocate_color($this->color);
    if($this->antialias == FALSE)
      $fore = -$fore;
    if($this->shadow_color != -1)
    {
      $shadow = $this->image_allocate_color($this->shadow_color);
      if($this->antialias == FALSE)
	$shadow = -$shadow;
      imagettftext($this->image, $this->size, 
		   $this->angle, $pos[0] + $this->shadow_decal, $pos[1] + $this->shadow_decal, 
		   $shadow, $font, $this->text);
    }
    imagettftext($this->image, $this->size, 
		 $this->angle, $pos[0], $pos[1], 
		 $fore, $font, $this->text);
  }

  function ttf_text_get_size()
  {
    $font = $this->fonts_path . $this->font;
    $sz = imagettfbbox($this->size, $this->angle, $font, $this->text);
    $left = min($sz[0], $sz[2], $sz[4], $sz[6]);
    $right = max($sz[0], $sz[2], $sz[4], $sz[6]);
    $top = min($sz[1], $sz[3], $sz[5], $sz[7]);
    $bottom = max($sz[1], $sz[3], $sz[5], $sz[7]);
    $this->txt_xsize = $right - $left;
    $this->txt_ysize = $bottom - $top;
    // si la taille n'est pas specifié ou qu'elle 
    // ne suffit pas pour afficher tout le texte :
    if($this->xsize == -1 || $this->xsize < ($this->txt_xsize + 2 * $this->margin))
      $this->xsize = $this->txt_xsize + 2 * $this->margin + 2;
    if($this->ysize == -1 || $this->ysize < ($this->txt_ysize + 2 * $this->margin))
      $this->ysize = $this->txt_ysize + 2 * $this->margin;
    if ($this->align == 'right')
      $px = (int) (-$left - $this->margin + ($this->xsize - $this->txt_xsize));
    elseif ($this->align == 'left')
      $px = (int) (-$left / 2);
    else
      $px = (int) (-$left + ($this->xsize - $this->txt_xsize) / 2);
    if ($this->valign == 'bottom')
      $py = (int) (-$top - $this->margin + ($this->ysize - $this->txt_ysize));
    elseif ($this->valign == 'top')
      $py = (int) (-$top);
    else
      $py = (int) (-$top + ($this->ysize - $this->txt_ysize) / 2);
    return array($px - 1, $py);
  }
};

?>
