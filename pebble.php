<?php
$agency="lametro-rail";
$payload = json_decode(file_get_contents('php://input'), true);
date_default_timezone_set('America/Los_Angeles');
 $payload[3] /= 10000;
 $payload[4] /= 10000;
 $ref= array($payload[3],$payload[4]);

$stations = array();
// Open the CSV of stations
if (($handle = fopen($agency.".csv", "r")) !==FALSE) {
$key = 0;
// While there is data available loop through unlimited times (0) using separator (,)
while (($data = fgetcsv($handle, 0, ",")) !==FALSE) {
   // Count the total keys in each row
   $c = count($data);
   //Populate the array
   for ($x=0;$x<$c;$x++) {
   $stations[$key][$x] = $data[$x];
      }
   $key++;
} 
fclose($handle);
}

//Calculates distance between two lat/longs
function distance($a, $b)
{
    list($lat1, $lon1) = $a;
    list($lat2, $lon2) = $b;
    $theta = $lon1 - $lon2;
    $dist = sin(deg2rad($lat1)) * sin(deg2rad($lat2)) +  cos(deg2rad($lat1)) * cos(deg2rad($lat2)) * cos(deg2rad($theta));
    $dist = acos($dist);
    $dist = rad2deg($dist);
    $miles = $dist * 60 * 1.1515;
    return $miles;
}

// Find closest station based on given ref lat/long array
$distances = array_map(function($item) use($ref) {
    $a = array_slice($item, -2);
    return distance($a, $ref);
}, $stations);
asort($distances);
$lines=array();
$lines_stations=array();

//Limit to one station based on passed variable from Pebble, make sure to retain key.
$distances=array_unique(array_slice($distances,$payload[5],1,true));

//Search stations for stations with closest name (might be more than one.)
foreach($stations as $station)
   {
  if ( $station[2] === $stations[key($distances)][2] ) //check all the lines for a station of this name (different stop number for each line)
		{
		$xml = new SimpleXMLElement('http://webservices.nextbus.com/service/publicXMLFeed?command=predictions&a='.$agency.'&stopId='.$station[0], LIBXML_NOCDATA, true);

		foreach($xml->predictions as $predictions)
			{
			$current = "{\"1\":\"".substr($station[1],0,10)." Line\",\"2\":\"".substr($predictions->direction[0]['title'],6,20)."\",\"3\":\"";
			$times="";
			foreach($predictions->direction[0]->prediction as $key2=>$value)	//fine the times for next trains and convert to local time
				{
				$epoch =  substr($value['epochTime'],0,10);
				$times=$times.date('H:i', $epoch)." ";
				}
			$times=substr($times,0,11);	 // Keep only the next two times.
			$current = $current.$times."\"}";
			if ($times !='') {array_push($lines, $current);}
			array_push($lines_stations, $station[2]);
			
			}
		}
	}

//Look at which button has been pressed. If no buttons have been press (first time the program is run), return the closest station.
switch((int)$payload[2]){
//return closest station (since $payload[5] will also be 0 at this point.)
	case 0:
	echo "{\"0\":\"".$lines_stations[0]." Station\" }";
	break;
	case 2:
//take a mod of the number of button presses and the number of lines available. (Basically, this causes it to loop.)
	$page = abs((((int)$payload[1]) % count($lines)));
	if ($lines[$page]==NULL){
		echo '{"3":"No Trains","1":"","2":""}';
	}
	else
	{
	echo $lines[$page];
	}
	break;
	case 3:
	$page = abs((((int)$payload[1]) % count($lines)));
	if ($lines[$page]==NULL){
		echo '{"3":"No Trains","1":"","2":""}';
	}
	else
	{
	echo $lines[$page];
	}	break;
	}

?>
