<?
// $Id: output_buffering.inc,v 1.1 2002/01/15 15:22:11 tieum Exp $

class pp_output_buffering extends pp_class
{
  var $data;
  var $output_callback;
  
  function pp_output_buffering($my_output_callback = -1)
  {
    $this->pp_class();
    if($my_output_callback == -1)
      $this->error("pp_output_buffering::pp_output_buffering : ".
	           _("no output callback function defined"));
    $this->data = '';
    $this->output_callback = $my_output_callback;
  }

  function start()
  {
    ob_start($this->output_callback);
  }

  function stop()
  {
    ob_end_flush();
  }

  function clean()
  {
    ob_end_clean();
  }

  function suspend()
  {
    $this->data = ob_get_contents();
    ob_end_clean();
    ob_start();
  }

  function resume()
  {
    $tmp = ob_get_contents();
    $this->clean();
    $this->start();
    print $this->data;
    $this->data = '';
    return $tmp;
  }
};

?>
