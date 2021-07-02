#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer esp8266_server(80);

void handleRoot() {
  esp8266_server.send(200, "text/plain", "Hello from ESP8266");
}

void handleNotFound() {
  esp8266_server.send(404, "text/plain", "404: Not found");
}

void setup(void) {
  Serial.begin(9600);

  wifiMulti.addAP("2021", "qwerty12345");
  wifiMulti.addAP("taichi-maker2", "87654321");
  wifiMulti.addAP("taichi-maker3", "13572468");

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

void loop(void) { esp8266_server.handleClient(); }