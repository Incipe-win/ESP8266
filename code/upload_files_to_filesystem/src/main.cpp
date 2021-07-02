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

bool handleFileRead(String path) {
  if (path.endsWith("/")) {
    path = "/index.html";
  }

  String contentType = getContentType(path);
  Serial.print("contentType: ");
  Serial.println(contentType);

  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    esp8266_server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleUserRequet() {

  String webAddress = esp8266_server.uri();

  bool fileReadOK = handleFileRead(webAddress);

  if (!fileReadOK) {
    esp8266_server.send(404, "text/plain", "404 Not Found");
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("");

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

  esp8266_server.onNotFound(handleUserRequet);

  esp8266_server.begin();
  Serial.println("HTTP server started");
}

void loop(void) { esp8266_server.handleClient(); }
