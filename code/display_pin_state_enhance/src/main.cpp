#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#define buttonPin D3

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer esp8266_server(80);

bool pinState;

String sendHTML(bool buttonState) {

  String htmlCode = "<!DOCTYPE html> <html>\n";
  htmlCode += "<head><meta http-equiv='refresh' content='5'/>\n";
  htmlCode += "<title>ESP8266 Butoon State</title>\n";
  htmlCode += "<style>html { font-family: Helvetica; display: inline-block; "
              "margin: 0px auto; text-align: center;}\n";
  htmlCode += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto "
              "30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  htmlCode += "</style>\n";
  htmlCode += "</head>\n";
  htmlCode += "<body>\n";
  htmlCode += "<h1>ESP8266 BUTTON STATE</h1>\n";

  if (buttonState) {
    htmlCode += "<p>Button Status: HIGH</p>\n";
  } else {
    htmlCode += "<p>Button Status: LOW</p>\n";
  }

  htmlCode += "</body>\n";
  htmlCode += "</html>\n";

  return htmlCode;
}

void handleRoot() { esp8266_server.send(200, "text/html", sendHTML(pinState)); }

void handleNotFound() {
  esp8266_server.send(404, "text/plain", "404: Not found");
}

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println("");

  pinMode(buttonPin, INPUT_PULLUP);

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
  esp8266_server.on("/", handleRoot);
  esp8266_server.onNotFound(handleNotFound);

  Serial.println("HTTP esp8266_server started");
}

void loop() {
  esp8266_server.handleClient();
  pinState = digitalRead(buttonPin);
}