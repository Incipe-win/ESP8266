#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

String file_name = "/incipe/notes.txt";

void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("LittleFS format start");
  LittleFS.format();
  Serial.println("LittleFS format finish");

  if (LittleFS.begin()) {
    Serial.println("LittleFS Started.");
  } else {
    Serial.println("LittleFS Failed to Start.");
  }

  // File dataFile = LittleFS.open(file_name, "w");
  // dataFile.println("Hello IOT World.");
  // dataFile.close();
  Serial.println("Finished Writing data to LittleFS");
}

void loop() {
  // put your main code here, to run repeatedly:
}