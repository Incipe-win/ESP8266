#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <FS.h>
#include <LittleFS.h>

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer esp8266_server(80);

String getContentType(String filename) {
  if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String resource) {

  if (resource.endsWith("/")) {
    resource = "/index.html";
  }

  String contentType = getContentType(resource);

  if (LittleFS.exists(resource)) {
    File file = LittleFS.open(resource, "r");
    esp8266_server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleUserRequest() {

  String reqResource = esp8266_server.uri();
  Serial.print("reqResource: ");
  Serial.println(reqResource);

  bool fileReadOK = handleFileRead(reqResource);

  if (!fileReadOK) {
    esp8266_server.send(404, "text/plain", "404 Not Found");
  }
}

void handleADC() {
  int a = analogRead(A0);
  String adcValue = String(a);

  esp8266_server.send(200, "text/plain", adcValue);
}

void handleLED() {
  String ledState = "OFF";
  String LED_State = esp8266_server.arg("LEDstate");
  Serial.println(LED_State);

  if (LED_State == "1") {
    digitalWrite(LED_BUILTIN, LOW);
    ledState = "ON";
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    ledState = "OFF";
  }

  esp8266_server.send(200, "text/plain", ledState);
}

void setup() {
  Serial.begin(9600);
  Serial.println("");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

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

  if (LittleFS.begin()) {
    Serial.println("LittleFS Started.");
  } else {
    Serial.println("LittleFS Failed to Start.");
  }

  esp8266_server.on("/setLED", handleLED);
  esp8266_server.on("/readADC", handleADC);

  esp8266_server.onNotFound(handleUserRequest);

  esp8266_server.begin();
  Serial.println("HTTP server started");
}

void loop(void) { esp8266_server.handleClient(); }
