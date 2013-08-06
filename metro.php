<?php
$payload = json_decode(file_get_contents('php://input'), true);
date_default_timezone_set('America/Los_Angeles');
 $payload[3] /= 10000;
 $payload[4] /= 10000;
 $ref= array($payload[3],$payload[4]);

$stations = array();
// Open the CSV of stations
if (($handle = fopen("stations.csv", "r")) !==FALSE) {
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

//Search stations for stations with closest name (might be more than one.)
foreach($stations as $station)
   {
  if ( $station[2] === $stations[key($distances)][2] )
		{
		$xml = new SimpleXMLElement('http://webservices.nextbus.com/service/publicXMLFeed?command=predictions&a=lametro-rail&stopId='.$station[0], LIBXML_NOCDATA, true);

		foreach($xml->predictions as $predictions)
			{
			$current = "{\"0\":\"".$station[2]."\",\"1\":\"".substr($station[1],0,10)."\",\"2\":\"".substr($predictions->direction[0]['title'],6,15)."\",\"3\":\"";
			$times="";
			foreach($predictions->direction[0]->prediction as $key2=>$value)
				{
				$epoch =  substr($value['epochTime'],0,10);
				$times=$times.date('H:i', $epoch)." ";
				}
			$times=substr($times,0,11);	
			$current = $current.$times."\"}";
			if ($times !='') {array_push($lines, $current);}
			array_push($lines_stations, $station[2]);
			
			}
		}
	}
//based on the packet recieved from Pebble, it cycles through the lines for that station.

switch((int)$payload[2]){
	case 0:
		echo $lines[0];
		break
	case 2:
		echo $lines[0];
		break;
	case 3:
		$page = (((int)$payload[1]) % count($lines));
		echo $lines[$page];
		break;

}
?>
