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
    Serial.println(" FOUND.");

    File dataFile = LittleFS.open(file_name, "a");
    dataFile.println("This is Appended Info.");
    dataFile.close();
    Serial.println("Finished Appending data to LittleFS");
  } else {
    Serial.print(file_name);
    Serial.println(" NOT FOUND.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}