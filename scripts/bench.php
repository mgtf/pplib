#!/usr/bin/php4 -q
<? 

define('APPNAME', 'millidata');
define('SYSLOG', true);
define('DEVEL_MODE', false);
define('DEBUG', true);
define('BASE_PATH', '');
define('IP', '62.93.232.25');
define('DBHOST', 'localhost');
define('DBDB', 'null');
define('DBUSER', 'null');
define('DBPWD', 'null');
define('SITE_NAME', 'bench');
define('DOMAIN', 'null');
define('CACHE_PATH', '/tmp/');

chdir('..');
include('pp.inc');
set_time_limit(0);

require_once('base/microtime.inc');

echo "-- iotalib Benchmarking tool --<br><br>";
echo "1. test parsing speed...<br>";

$sum = 0;
for($j = 0 ; $j < 5; $j++) {
  $timer = new microtime;
  $timer->start('bench'.$j);

  for($i = 0; $i < 2000; $i++) {
    $pp->tpl->dynbloc_data[$i] = array(
	'line' => rand()
	);
  }

  $pp->tpl->parse_template('<table>{dynbloc bloc=`<tr><td>{dynbloc_ite}</td><td>{line}</td></tr>`}</table>');


  $timer->stop('bench'.$j);
  $elapsed = round($timer->elapsed('bench'.$j), 3);

  printf('temps écoulé : %.3f sec<br>', $elapsed);
  $sum += $elapsed;
}
printf('Moyenne : %.3f sec', $sum / 5);


?>
