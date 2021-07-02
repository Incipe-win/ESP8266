#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

// 设置wifi接入信息(请根据您的WiFi信息进行修改)
const char *ssid = "2021";
const char *password = "qwerty12345";
const char *mqttServer = "test.ranye-iot.net";

Ticker ticker;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void connectWifi();
void tickerCount();
void connectMQTTServer();
void pubMQTTmsg();

int count; // Ticker计数用变量

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

  // Ticker定时对象
  ticker.attach(1, tickerCount);
}

void loop() {
  if (mqttClient.connected()) { // 如果开发板成功连接服务器
    // 每隔3秒钟发布一次信息
    if (count >= 3) {
      pubMQTTmsg();
      count = 0;
    }
    // 保持心跳
    mqttClient.loop();
  } else {               // 如果开发板未能成功连接服务器
    connectMQTTServer(); // 则尝试连接服务器
  }
}

void tickerCount() { count++; }

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

// 发布信息
void pubMQTTmsg() {
  static int value; // 客户端发布信息用数字

  // 建立发布主题。主题名称以Taichi-Maker-为前缀，后面添加设备的MAC地址。
  // 这么做是为确保不同用户进行MQTT信息发布时，ESP8266客户端名称各不相同，
  // String topicString = "Taichi-Maker-Pub-" + WiFi.macAddress();
  String topicString = "incipe";
  char publishTopic[topicString.length() + 1];
  strcpy(publishTopic, topicString.c_str());

  // 建立发布信息。信息内容以Hello World为起始，后面添加发布次数。
  String messageString = "Hello World " + String(value++);
  char publishMsg[messageString.length() + 1];
  strcpy(publishMsg, messageString.c_str());

  // 实现ESP8266向主题发布信息
  if (mqttClient.publish(publishTopic, publishMsg)) {
    Serial.println("Publish Topic:");
    Serial.println(publishTopic);
    Serial.println("Publish message:");
    Serial.println(publishMsg);
  } else {
    Serial.println("Message Publish Failed.");
  }
}

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