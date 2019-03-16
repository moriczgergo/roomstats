# roomstats
Temperature, humidity, altitude, pressure, and CO2 measurement IoT firmware for ESP8266.

## Requirements

 * A BME280/BMP280 (or compatible) sensor
 * An SGP-30 sensor (optional)
 * Adafruit Sensor, BME280 and SGP30 libraries
 * WiFiManager library

## Wiring

ESP D1 -> BME280 SCL
ESP D2 -> BME280 SDA
ESP 3V3 -> BME280 VIN
ESP GND -> BME280 GND

If you have an SGP-30 too:
ESP D1 -> SGP30 SCL
ESP D2 -> SGP30 SDA
ESP 3V3 -> SGP30 VIN (make sure your board has a voltage regulator, since the chip is 1.8V)
ESP GND -> SGP30 GND

## Usage

On first boot, [WifiManager](https://github.com/tzapu/WiFiManager) will start. Configure your WiFi settings.

Then, you can reach the board on it's IP logged in serial/set in the static IP settings. Port 80 will get you a JSON with the data acquired by the board:
 * temperature: Temperature in celsius
 * humidity: Humidity percentage
 * pressure: Pressure in hPa
 * altitude: Altitude from sea level in meters
 * co: CO2 level in ppm (only with SGP-30)
 * tvco: indoor total VOC (only with SGP-30)

Note that some sensors may need to be calibrated.

## Configuration

**IMPORTANT**: You may need to change the I2C address of the BME280 specified in `bme.begin`.

You can configure the following variables in roomstats.ino:
 * Static IP address: Change `STATIC_IP`, `STATIC_GATEWAY` and `STATIC_MASK`. Comment them out to enable DHCP.
 * I2C address of BME280: Change `0x76` in `bme.begin` call.

## Housing

You can 3D print the `Box.stl` and `Box Lid.stl` files to make a housing for the board. The box is designed around the Apple iPhone charger (slim version, EU version, not sure if there's a difference).

You can solder on a USB jack to the ESP's 5V and GND pins, or you can use a really short microUSB cable.

![roomstats board w/ housing](https://i.imgur.com/hvtiESm.jpg)
