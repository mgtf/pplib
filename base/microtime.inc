<?php

// $Id: microtime.inc,v 1.1 2002/01/15 15:22:11 tieum Exp $
// example:
//         $timer = new c_Timer;
//         $timer->start();
//         $timer->stop();
//         echo $timer->elapsed();

class pp_microtime extends pp_class {

    var $t_start;
    var $t_stop;
    var $t_elapsed;

    function pp_microtime()
    {
      $this->pp_class();
      $t_start = 0; 
      $t_stop = 0;
      $t_elapsed = 0;
    }

    function microtime_()
    {
      list($usec, $sec) = explode(" ", microtime()); 
      return ((float)$usec + (float)$sec); 
    } 
	
    function start() 
    { 
      $this->t_start = $this->microtime_(); 
    }
    
    function stop()  
    { 
      $this->t_stop  = $this->microtime_(); 
    }

    function elapsed() 
    {
      if ($this->t_elapsed != 0)
	return $this->t_elapsed;
      else
	return ($this->t_elapsed = ($this->t_stop - $this->t_start));
    }
};

?>
