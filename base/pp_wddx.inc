<?
// $Id: pp_wddx.inc,v 1.1 2002/02/07 15:42:27 tieum Exp $

class pp_wddx extends pp_data
{
  var $wddx_items;
  var $wddx_id;

  function pp_wddx()
  {
    $this->pp_data();
    $this->wddx_id = -1;
    $this->wddx_items = array();
  }

  function wddx_serialize()
  {
    $this->wddx_id = wddx_packet_start('proportail content');
    if($this->wddx_id == -1)
      $this->error('article::article_serialize : can\'t create wddx packet');
    $data = $this->wddx_items;
    wddx_add_vars($this->wddx_id, 'data');
    unset($data);
    $this->data = wddx_packet_end($this->wddx_id);
  }

  function wddx_deserialize()
  {
    if($this->data == -1)
      $this->error('pp_wddx::wddx_deserialize : no data');
    $data = wddx_deserialize($this->data);
    return $data;
  }
};

?>
