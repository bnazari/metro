<?php
//$agency="lametro-rail";
 $agency="lametro";
$xml_string="";
  	$xml = new SimpleXMLElement('http://webservices.nextbus.com/service/publicXMLFeed?command=routeList&a='.$agency, LIBXML_NOCDATA, true);
		foreach($xml->route as $route)
			{
			$xml2 = new SimpleXMLElement('http://webservices.nextbus.com/service/publicXMLFeed?command=routeConfig&a='.$agency.'&r='.$route['tag'], LIBXML_NOCDATA, true);
			foreach($xml2->route->stop as $stop)
				{
				
				echo $stop['stopId'].",".$route['title'].",".$stop['title'].",".$stop['lat'].",".$stop['lon'];
				echo "\n";
				}
			
			}


?>
