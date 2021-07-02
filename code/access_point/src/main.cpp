#include <Arduino.h>
#include <ESP8266WiFi.h>

const char *ssid = "incipe";
const char *password = "12345678";

void setup() {
  Serial.begin(9600);

  WiFi.softAP(ssid, password);

  Serial.print("Access Point: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {}