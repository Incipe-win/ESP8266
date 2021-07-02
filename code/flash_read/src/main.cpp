#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

String file_name = "/incipe/notes.txt";

void setup() {
  Serial.begin(9600);
  Serial.println("");
  if (LittleFS.begin()) {
    Serial.println("LittleFS Started.");
  } else {
    Serial.println("LittleFS Failed to Start.");
  }
  if (LittleFS.exists(file_name)) {
    Serial.print(file_name);
    Serial.println("FOUND.");
  } else {
    Serial.print(file_name);
    Serial.println("NOT FOUND.");
  }
  File dataFile = LittleFS.open(file_name, "r");
  for (int i = 0; i < dataFile.size(); ++i) {
    Serial.print((char)dataFile.read());
  }
  dataFile.close();
}

void loop() {
  // put your main code here, to run repeatedly:
}