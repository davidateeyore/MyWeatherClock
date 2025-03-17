# MyWeatherClock
a simple device using ESP32-CYD and modular sensors such as DHT11, Freetronics BaroSensor..

I was messing about with an ESP32-CYD - Cheap Yellow Display trying out the NTP disciplined clock code using the builtin WiFi and tried
 adding some extra bits. The next change will be trying out a combo temp, pressure, humidity module like the AHT20-BMP280 unit available from AliExpress among others. 

 The code is a bit rough but gives the general idea, the picture shows the DHT11/22? the BaroSensor is out of shot.

 Things to take note of: not all CYD boards are the same! I had to mess about with IO ports to get another CYD working, there is a lot of useful information out there.
update 17MAR2025 (St Patricks Day!)
the new version code is not quite finished yet - the new sensor is a combo unit that does temperature, pressure and humidity (AHT20_BMP280). I am also looking at better fonts/graphics for the CYD..
