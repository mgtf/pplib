<?php

////
// $Id: microtime.inc,v 1.4 2003/08/28 15:22:44 mgf Exp $
// Ce fichier est soumis à la licence d'utilisation
// contenue dans le fichier LICENCE
//
// example:
//         $timer = new c_Timer;
//         $timer->start(FOO);
//         $timer->stop(FOO);
//         echo $timer->elapsed(FOO);

class microtime extends class {

    var $t_start;
    var $t_stop;
    var $t_elapsed;

    function microtime()
    {
      $this->class();
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
