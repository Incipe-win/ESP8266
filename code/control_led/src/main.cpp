#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer esp8266_server(80);

void handleRoot() {
  esp8266_server.send(200, "text/html",
                      "<form action=\"/LED\" method=\"POST\"><input "
                      "type=\"submit\" value=\"Toggle LED\"></form>");
}

void handleLED() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  esp8266_server.sendHeader("Location", "/");
  esp8266_server.send(303);
}

void handleNotFound() {
  esp8266_server.send(404, "text/plain", "404: Not found");
}

void setup(void) {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  wifiMulti.addAP("2021", "qwerty12345");
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
  esp8266_server.on("/", HTTP_GET, handleRoot);
  esp8266_server.on("/LED", HTTP_POST, handleLED);
  esp8266_server.onNotFound(handleNotFound);

  Serial.println("HTTP esp8266_server started");
}

void loop(void) { esp8266_server.handleClient(); }