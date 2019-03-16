#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SGP30.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

// Comment out the following IPs if you don't want to use static IPs
#define STATIC_IP IPAddress(192,168,20,150)
#define STATIC_GATEWAY IPAddress(192,168,0,1)
#define STATIC_MASK IPAddress(255,255,0,0)

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
Adafruit_SGP30 sgp;

WiFiManager wifiManager;
ESP8266WebServer server(80);

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

bool sgpIsConnected = true;

void setup() {
    Serial.begin(9600);
    Serial.println(F("roomstats"));

    bool status;

    Serial.println(F("querying wifi"));

    #if defined(STATIC_IP) && defined(STATIC_GATEWAY) && defined(STATIC_MASK)
    wifiManager.setSTAStaticIPConfig(STATIC_IP, STATIC_GATEWAY, STATIC_MASK);
    #endif
    
    wifiManager.autoConnect();

    Serial.println(F("connected"));
    Serial.print(F("ip: "));
    Serial.println(WiFi.localIP());
    
    // default settings
    // (you can also pass in a Wire library object like &Wire2)
    status = bme.begin(0x76);  
    if (!status) {
      Serial.println(F("bme sensor not found"));
      while (1);
    }

    if (!sgp.begin()) {
      sgpIsConnected = false;
    } else {
      Serial.print(F("sgp serial #"));
      Serial.print(sgp.serialnumber[0], HEX);
      Serial.print(sgp.serialnumber[1], HEX);
      Serial.println(sgp.serialnumber[2], HEX);
    }
    
    server.on("/", []() {
      float temp = bme.readTemperature();
      float hum = bme.readHumidity();
      String message = "{\"temperature\": ";
      message += temp;
      message += ", \"pressure\": ";
      message += (bme.readPressure() / 100.0F);
      message += ", \"humidity\": ";
      message += hum;
      message += ", \"altitude\": ";
      message += bme.readAltitude(SEALEVELPRESSURE_HPA);
      if (sgpIsConnected) {
        sgp.setHumidity(getAbsoluteHumidity(temp, hum));
        if (sgp.IAQmeasure()) {
          message += ", \"co\": ";
          message += sgp.eCO2;
          message += ", \"tvoc\": ";
          message += sgp.TVOC;
        } else {
          Serial.println(F("failed to read from sgp"));
        }
      }
      message += "}";
      server.send(200, "text/plain", message);
    });

    server.begin();
    Serial.println(F("server started"));
}


void loop() { 
    server.handleClient();
}
