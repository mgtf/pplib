<?php

// $Id: pp_microtime.inc,v 1.2 2002/02/20 13:53:07 tieum Exp $
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
	
    function start($name = 'default') 
    { 
      $this->t_start[$name] = $this->microtime_(); 
    }
    
    function stop($name = 'default')  
    { 
      $this->t_stop[$name]  = $this->microtime_(); 
    }

    function elapsed($name = 'default') 
    {
      if ($this->t_elapsed != 0)
	return $this->t_elapsed[$name];
      else
	return ($this->t_elapsed[$name] = ($this->t_stop[$name] - $this->t_start[$name]));
    }
};

?>
