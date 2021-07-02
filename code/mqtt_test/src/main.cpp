#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// 设置wifi接入信息(请根据您的WiFi信息进行修改)
const char *ssid = "2021";
const char *password = "qwerty12345";
const char *mqttServer = "test.ranye-iot.net";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ESP8266连接wifi
void connectWifi() {

  WiFi.begin(ssid, password);

  //等待WiFi连接,成功连接后输出成功信息
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");
  Serial.println("");
}

void connectMQTTServer() {
  // 根据ESP8266的MAC地址生成客户端ID（避免与其它ESP8266的客户端ID重名）
  String clientId = "esp8266-" + WiFi.macAddress();

  // 连接MQTT服务器
  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.println("ClientId:");
    Serial.println(clientId);
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }
}

void setup() {
  Serial.begin(9600);

  //设置ESP8266工作模式为无线终端模式
  WiFi.mode(WIFI_STA);

  // 连接WiFi
  connectWifi();

  // 设置MQTT服务器和端口号
  mqttClient.setServer(mqttServer, 1883);

  // 连接MQTT服务器
  connectMQTTServer();
}

void loop() {
  if (mqttClient.connected()) { // 如果开发板成功连接服务器
    mqttClient.loop();          // 保持客户端心跳
  } else {                      // 如果开发板未能成功连接服务器
    connectMQTTServer();        // 则尝试连接服务器
  }
}
