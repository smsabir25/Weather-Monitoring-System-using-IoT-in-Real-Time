#include <ESP8266WiFi.h>
#include <SFE_BMP180.h>
#include <ThingSpeak.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "DHT.h"

#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
SFE_BMP180 bmp;
double T, P;
char status;
const char* ssid = "wifipcg";           // replace with your wifi ssid and wpa2 key
const char* password = "hello123";
const char* server = "api.thingspeak.com";
unsigned long myChannelNumber = 1937721;
const char* myWriteAPIKey = "ZG62CHYWJONCUPK8";
WiFiClient client;

void setup() {
    Serial.begin(115200);
    delay(10);
    dht.begin();
    bmp.begin();
    Wire.begin();

    // Connect to WiFi network
    WiFi.disconnect();
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    ThingSpeak.begin(client);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    pinMode(LED_BUILTIN, OUTPUT);       // Initialize the LED_BUILTIN pin as an output
}

void loop() {
    // Blink nodemcu LED
    digitalWrite(LED_BUILTIN, LOW);     // Turn the LED on (Note that LOW is the voltage level but actually the LED is on; this is because it is active low on the ESP-01)
    delay(1000);                        // Wait for 1 sec
    digitalWrite(LED_BUILTIN, HIGH);    // Turn the LED off by making the voltage HIGH

    // BMP180 sensor
    status = bmp.startTemperature();
    if (status != 0) {
        delay(status);                  // Wait for the measurement to complete:
        status = bmp.getTemperature(T);
        status = bmp.startPressure(3);  // Calculates pressure values from 0 to 3
        if (status != 0) {
            delay(status);
            status = bmp.getPressure(P, T);
            if (status != 0) {
            }
        }
    }
    delay(1000);                        // wait for 1sec                     

    // DHT11 sensor
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("failed to read DHT");
    }
    delay(1000);                        // wait for 1sec

    // Rainfall Sensor   
    int r = analogRead(A0);             //Gets the rainfall values through the sensor
    r = map(r, 0, 1024, 0, 100);        //Converts these values from 0 to 100
    int R = 100-r;

    // Display data to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println("'C");
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.println("%");
    Serial.print("Absolute Pressure: ");
    Serial.print(P, 2);
    Serial.println("mbar");
    Serial.print("Rainfall: ");
    Serial.println(R);
    Serial.println("");
    
    // Sent data to ThingSpeak.com cloud
    ThingSpeak.setField(1, (float)P);
    ThingSpeak.setField(2, (float)t);
    ThingSpeak.setField(3, (int)h);
    ThingSpeak.setField(4, (int)R);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);  // Update in ThingSpeak

    Serial.println("Waiting..........");
    delay(15000);  // thingspeak needs minimum 15 sec delay between updates
}
