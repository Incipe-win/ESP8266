
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
void receiveCallback(char *topic, byte *payload, unsigned int length);
void subscribeTopic();
void pubMQTTmsg();
void connectMQTTServer();
void tickerCount();

int count; // Ticker计数用变量

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  // Ticker定时对象
  ticker.attach(1, tickerCount);

  //设置ESP8266工作模式为无线终端模式
  WiFi.mode(WIFI_STA);

  // 连接WiFi
  connectWifi();

  // 设置MQTT服务器和端口号
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(receiveCallback);

  // 连接MQTT服务器
  connectMQTTServer();
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
    subscribeTopic(); // 订阅指定主题
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }
}

// 发布信息
void pubMQTTmsg() {
  // 建立发布主题。主题名称以Taichi-Maker-为前缀，后面添加设备的MAC地址。
  // 这么做是为确保不同用户进行MQTT信息发布时，ESP8266客户端名称各不相同，
  String topicString = "incipe";
  char publishTopic[topicString.length() + 1];
  strcpy(publishTopic, topicString.c_str());

  // 定时向服务器主题发布当前D3引脚状态
  String messageString;
  if (digitalRead(D3)) {
    messageString = "on";
  } else {
    messageString = "off";
  }
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

// 订阅指定主题
void subscribeTopic() {

  // 建立订阅主题。主题名称以Taichi-Maker-Sub为前缀，后面添加设备的MAC地址。
  // 这么做是为确保不同设备使用同一个MQTT服务器测试消息订阅时，所订阅的主题名称不同
  String topicString = "incipe";
  char subTopic[topicString.length() + 1];
  strcpy(subTopic, topicString.c_str());

  // 通过串口监视器输出是否成功订阅主题以及订阅的主题名称
  if (mqttClient.subscribe(subTopic)) {
    Serial.println("Subscribe Topic:");
    Serial.println(subTopic);
  } else {
    Serial.print("Subscribe Fail...");
  }
}

// 收到信息后的回调函数
void receiveCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  Serial.print("Message Length(Bytes) ");
  Serial.println(length);

  if ((char)payload[0] == '1') {    // 如果收到的信息以“1”为开始
    digitalWrite(LED_BUILTIN, LOW); // 则点亮LED。
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // 否则熄灭LED。
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
