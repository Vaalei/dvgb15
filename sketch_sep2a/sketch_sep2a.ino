#include "kaulab.h"
unsigned long previousMillis = 0;
const long interval = 100; // Time between prints in milliseconds (e.g., 500ms)


void setup() {
  //zInitialize();  
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last time you printed
    int distance = zRobotGetUltraSensor()
    Serial.println(distance);
  }

  // 1 höger sensor
  // 2 vänster sensor
  // 3 båda sensorerna 
  switch(zRobotGetLineSensor()){
    case 0:   // inga
      break;
    case 1:   // 1 höger sensor
      break;
    case 2:   // 2 vänster sensor
      break;
    case 3:   // 3 båda sensorerna 
      break;
  }
  

}



