#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#define buttonPin D3

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer esp8266_server(80);

bool pinState;

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);

  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");
  Serial.println("Connecting ...");

  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.print(i++);
    Serial.print(' ');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  esp8266_server.begin();
  esp8266_server.on("/", handleRoot);
  esp8266_server.onNotFound(handleNotFound);

  Serial.println("HTTP esp8266_server started");
}

void loop() {
  esp8266_server.handleClient();
  pinState = digitalRead(buttonPin);
}

void handleRoot() {
  String displayPinState;

  if (pinState == HIGH) {
    displayPinState = "Button State: HIGH";
  } else {
    displayPinState = "Button State: LOW";
  }

  esp8266_server.send(200, "text/plain", displayPinState);
}

void handleNotFound() {
  esp8266_server.send(404, "text/plain", "404: Not found");
}