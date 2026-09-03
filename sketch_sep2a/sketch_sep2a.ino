#include "kaulab.h"
unsigned long previousMillis = 0;
const long interval = 100; // Time between prints in milliseconds (e.g., 500ms)
const long speed = 60;

enum lap_direction {DIRECTION_LEFT, DIRECTION_RIGHT};
enum robot_mode {NORMAL, AVOID};
enum line_value {ONLINE, RIGHT, LEFT, OFFLINE};

struct robot {
  lap_direction direction;
  robot_mode mode;
  line_value line;
}

void readLineValue(){
  int lineval = zRobotGetLineSensor();
  robot.line = lineval;
  
}

void readDistanceValue(){

}

void updateRobotState() {
  switch(){
    case 0:   // inga dvs båda på linjen

      break;
    case 1:   // 1 höger sensor, höger utanför linjen

      break;
    case 2:   // 2 vänster sensor, vänster utanför linjen

      break;
    case 3:   // 3 båda sensorerna, båda utanför?? 

      break;
  }
}

void setup() {
  zInitialize();  
  Serial.begin(9600);
  zScheduleTask(readLineValue, 50, 10);
  zScheduleTask(readDistanceValue, 100, 10);
  zScheduleTask(updateRobotState, 50, 10);
  // put your setup code here, to run once:

}

void loop() {

  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last time you printed
    int distance = zRobotGetUltraSensor();
    Serial.print(distance);
    Serial.print(", ");
    Serial.println(zRobotGetLineSensor());

  }

  // 1 höger sensor
  // 2 vänster sensor
  // 3 båda sensorerna 
  // ger output om den ser vit eller inte svart
  

}

/*
  switch(){
    case 0:   // inga dvs båda på linjen
      zRobotSetMotorSpeed(1, -speed);
      zRobotSetMotorSpeed(2, speed);
      break;
    case 1:   // 1 höger sensor, höger utanför linjen
      zRobotSetMotorSpeed(1, -speed);
      zRobotSetMotorSpeed(2, 0);
      break;
    case 2:   // 2 vänster sensor, vänster utanför linjen
      zRobotSetMotorSpeed(1, 0);
      zRobotSetMotorSpeed(2, speed);
      break;
    case 3:   // 3 båda sensorerna, båda utanför?? 
      zRobotSetMotorSpeed(1, 0);
      zRobotSetMotorSpeed(2, 0);
      break;
  }
*/
