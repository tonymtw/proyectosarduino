

/**************************************************************
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************
 * This example runs directly on ESP8266 chip.
 *
 * Note: This requires ESP8266 support package:
 *   https://github.com/esp8266/Arduino
 *
 * Please be sure to select the right ESP8266 module
 * in the Tools -> Board menu!
 *
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 *
 **************************************************************/

//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "Seeed_BME280.h"
#include <Wire.h>


BME280 bme280;
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "your_key_blynk";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "your_SID";
char pass[] = "your_password"
void setup()
{
  
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  Serial.begin(9600);
  if(!bme280.init()){
  Serial.println("Device error!");
  }
}

void loop()
{
  Blynk.run();
  
  //get and print temperatures
  float temp = bme280.getTemperature()-1.3;
  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.println("C");//The unit for  Celsius because original arduino don't support speical symbols
  Blynk.virtualWrite(1, temp); // virtual pin 0
  Blynk.virtualWrite(2, temp); // virtual pin 4
  
  
  //get and print atmospheric pressure data
  float pressure = bme280.getPressure(); // pressure in Pa
  float p = pressure/100.0 ; // pressure in hPa
  Serial.print("Pressure: ");
  Serial.print(p);
  Serial.println("hPa");
  Blynk.virtualWrite(3, p); // virtual pin 1

  
  //get and print altitude data
  float altitude = bme280.calcAltitude(pressure);
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println("m");
  Blynk.virtualWrite(4, altitude); // virtual pin 2

  //get and print humidity data
  float humidity = bme280.getHumidity();
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");
  Blynk.virtualWrite(5, humidity); // virtual pin 3
  ESP.deepSleep(1 * 60 * 1000000); // // deepSleep time is defined in microseconds. Multiply seconds by 1e6 
// delay(10000);
}

