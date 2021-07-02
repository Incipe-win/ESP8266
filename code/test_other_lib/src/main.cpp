#include "font/FxLED_32.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <NTPCLient.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <TimeLib.h>
#include <WiFiManager.h>
#include <WiFiUdp.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite clk = TFT_eSprite(&tft);

#define WIDGHT 160
#define HEIGHT 128

const int bg_color = TFT_WHITE;

// const char *ssid = "2021";
// const char *password = "qwerty12345";
static const char ntpServerName[] = "ntp6.aliyun.com";
const int timeZone = 8;
WiFiUDP Udp;
unsigned int localPort = 8000;
#define NTP_PACKET_SIZE 48
byte packetBuffer[NTP_PACKET_SIZE];
int backLight_hour = 0;
time_t prevDisplay = 0; // 显示时间

// void connect_wifi() {
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to ");
//   Serial.print(ssid);
//   Serial.println(" ...");

//   int i = 0;
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(i++);
//     Serial.print(' ');
//   }
//   Serial.println("");
//   Serial.println("Connection established!");
//   Serial.print("IP address:    ");
//   Serial.println(WiFi.localIP());
// }

void auto_connect_wifi() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("incipe", "123456789");
  Serial.println("");
  Serial.print("ESP8266 Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
}

void tft_init() {
  tft.init(ST7735_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(bg_color);
  // clk.loadFont(FxLED_32);
}

String get_info(const char *host, int port, String reqRes) {
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

  String res;

  if (client.connect(host, port)) {
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
        // String line = client.readStringUntil('\n');
        // Serial.println(line);
        res += client.readStringUntil('\n');
      }
    }

  } else {
    Serial.println(" connection failed!");
  }
  client.stop();
  return res;
}

void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); // NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime() {
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0)
    ; // discard any previously received packets
  // Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  // Serial.print(ntpServerName);
  // Serial.print(": ");
  // Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      // Serial.println(secsSince1900 - 2208988800UL + timeZone *
      // SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // 无法获取时间时返回0
}

//星期
String week() {
  String wk[7] = {"日", "一", "二", "三", "四", "五", "六"};
  String s = "周" + wk[weekday() - 1];
  return s;
}

//月日
String monthDay() {
  String s = String(month());
  s = s + "月" + day() + "日";
  return s;
}

String num2str(int digits) {
  String s = "";
  if (digits < 10)
    s = s + "0";
  s = s + digits;
  return s;
}

//时分
String hourMinute() {
  String s = num2str(hour());
  backLight_hour = s.toInt();
  s = s + ":" + num2str(minute());
  return s;
}

void printDigits(int digits) {
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void digitalClockDisplay() {
  clk.setColorDepth(8);
  clk.createSprite(140, 128);
  clk.fillSprite(bg_color);
  // clk.setTextDatum(TL_DATUM);
  clk.setTextColor(TFT_BLACK, bg_color);
  clk.drawString(hourMinute(), 0, 38, 7); //绘制时和分
  clk.pushSprite(-1, 0);
  clk.deleteSprite();

  clk.createSprite(40, 128);
  clk.fillSprite(bg_color);
  clk.loadFont(FxLED_32);
  // clk.setTextDatum(BR_DATUM);
  clk.setTextColor(TFT_BLACK, bg_color);
  // Serial.println(num2str(second()));
  clk.drawString(num2str(second()), 0, 38);
  clk.unloadFont();
  clk.pushSprite(140, 30);
  clk.deleteSprite();
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("");

  // connect_wifi();

  tft_init();
  auto_connect_wifi();

  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}

void loop() {
  if (now() != prevDisplay) {
    prevDisplay = now();
    digitalClockDisplay();
  }
}
