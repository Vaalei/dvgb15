#include <Arduino.h>
#include "kaulab.h"
unsigned long timer = 0;
const long speed = 60;

enum lap_direction {CLOCKWISE, COUNTERCLOCKWISE};
enum direction {LEFT, RIGHT, STRAIGHT};
enum robot_mode {NORMAL, AVOID};
enum line_value {ONLINE, RIGHT, LEFT, OFFLINE};

struct robot {
  lap_direction dir;
  robot_mode mode;
  line_value line;
  int distance;
  direction turn_direction;
  long avoid_start_time;
};

robot robotState;

void motorController(int leftSpeed, int rightSpeed) {
  zRobotSetMotorSpeed(1, leftSpeed);
  zRobotSetMotorSpeed(2, -rightSpeed);
}

void readLineValue(){
  int lineval = zRobotGetLineSensor();
  robotState.line = static_cast<line_value>(lineval);
  if (robotState.mode == AVOID && robotState.line != OFFLINE && (millis() - robotState.avoid_start_time > 1200)) {
    robotState.mode = NORMAL;       // Switch back to NORMAL mode after avoidance and line is detected
  }
}

void readDistanceValue(){
  int distance = zRobotGetUltraSensor();
  if (robotState.mode == AVOID) {
    return; // If already in AVOID mode, don't change it based on distance
  }
  robotState.distance = distance;
  if (distance < 20) {
    robotState.mode = AVOID;
    robotState.avoid_start_time = millis(); // Record the time when avoidance started
  } else {
    robotState.mode = NORMAL;
  }
}

void updateRobotState() {
  Serial.println("Line Sensor Value: " + String(robotState.line));
  switch(robotState.line){
    case ONLINE:   // inga dvs båda på linjen
    zRobotSetMotorSpeed(1, -speed);
    zRobotSetMotorSpeed(2, speed);
      break;
    case RIGHT:   // 1 höger sensor, höger utanför linjen
    zRobotSetMotorSpeed(1, -speed);
    zRobotSetMotorSpeed(2, 0);
      break;
    case LEFT:   // 2 vänster sensor, vänster utanför linjen
    zRobotSetMotorSpeed(1, 0);
    zRobotSetMotorSpeed(2, speed);
      break;
    case OFFLINE:   // 3 båda sensorerna, båda utanför??
    zRobotSetMotorSpeed(1, 0);
    zRobotSetMotorSpeed(2, 0);
      break;
  }
  switch (robotState.mode) {
    case NORMAL:
      break;
    case AVOID:
      // Make robot turn and then go in a cricle around the object
      if (millis() - robotState.avoid_start_time < 1000) {
        if (robotState.dir == CLOCKWISE) {
          motorController(speed, -speed); // Turn right in place
        } else {
          motorController(-speed, speed); // Turn left in place
        }

      } else {
        // After turning, go straight for a while
        if (robotState.dir == CLOCKWISE) {
          motorController(0.5 * speed, speed); // Move forward
        } else {
          motorController(-speed, 0.5 * (-speed)); // Move backward
        }
      }

      break;
  }
}

void setup() {
  zInitialize();
  Serial.begin(9600);
  zScheduleTask(readLineValue, 2, 10);
  zScheduleTask(readDistanceValue, 10, 10);
  zScheduleTask(updateRobotState, 2, 10);
  zStart();

  // put your setup code here, to run once:

}

void loop() {
/*
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Save the last time you printed
    int distance = zRobotGetUltraSensor();
    Serial.print(distance);
    Serial.print(", ");
    Serial.println(zRobotGetLineSensor());

  }
*/
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
