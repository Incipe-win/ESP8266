#include <ESP8266WiFi.h>
#include <JsonTranslator.h>

const char *ssid = "2021";
const char *password = "qwerty12345";

const char *host = "d1.weather.com.cn";
const int httpPort = 80;

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(i++);
    Serial.print(' ');
  }
  Serial.println("");
  Serial.println("Connection established!");
  Serial.print("IP address:    ");
  Serial.println(WiFi.localIP());
}

void httpRequest(String reqRes) {
  WiFiClient client;

  String httpRequest = String("GET ") + reqRes + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) "
                       "AppleWebKit/537.36 (KHTML, like Gecko) "
                       "Chrome/91.0.4472.77 Safari/537.36\r\n" +
                       "Connection: keep-alive\r\n\r\n";
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.print(httpRequest);

  if (client.connect(host, 80)) {
    Serial.println(" Success!");

    client.print(httpRequest);
    Serial.println("Sending request: ");
    Serial.println(httpRequest);

    String status_response = client.readStringUntil('\n');
    Serial.print("status_response: ");
    Serial.println(status_response);

    if (client.find("\r\n\r\n")) {
      Serial.println("Found Header End. Start Parsing.");
    }

    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }

  } else {
    Serial.println(" connection failed!");
  }
  client.stop();
}

void setup() {
  Serial.begin(9600);
  Serial.println("");
  connectWiFi();
  String reqRes = "/dingzhi/101250901.html?_=1623126588022";
  httpRequest(reqRes);
}

void loop() {}
