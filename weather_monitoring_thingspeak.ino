/*Weather monitoring system with thingspeak.
   Created by the SriTu Hobby team.
   https://www.srituhobby.com
*/

#include <SFE_BMP180.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include "DHT.h"


String apiKey = "IJQ10XGXNAXBMAX1";
const char *ssid =  "wifipcg";
const char *pass =  "hello123";
const char* server = "api.thingspeak.com";


DHT dht(D2, DHT11);
SFE_BMP180 bmp;
double T, P;
char status;
WiFiClient client;


void setup() {
  Serial.begin(115200);
  delay(10);
  bmp.begin();
  Wire.begin();
  dht.begin();
  WiFi.begin(ssid, pass);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}
void loop() {
  //BMP180 sensor
  status =  bmp.startTemperature();
  if (status != 0) {
    delay(status);
    status = bmp.getTemperature(T);
    // Calculates pressure values
    status = bmp.startPressure(3);// 0 to 3
    if (status != 0) {
      delay(status);
      status = bmp.getPressure(P, T);
      if (status != 0) {
      }
    }
  }

  //DHT11 sensor
//Temperature and humidity values are obtained through the sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  
//The DHT11 sensor is checked
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  //Rain sensor
//Gets the rainfall values through the sensor
  int r = analogRead(A0);
//Converts these values from 0 to 100
  r = map(r, 0, 1024, 0, 100);

//This code sends sensor values to the Thingspeak app
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(P, 2);
    postStr += "&field4=";
    postStr += String(r);
    postStr += "\r\n\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n");
    client.print(postStr);

//These values are printed on the serial monitor
    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("absolute pressure: ");
    Serial.print(P, 2);
    Serial.println("mb");
    Serial.print("Rain");
    Serial.println(r);

  }
  client.stop();
  delay(1000);
}
