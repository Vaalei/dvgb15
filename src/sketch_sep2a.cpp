#include <Arduino.h>
#include "kaulab.h"
unsigned long timer = 0;
const long speed = 80;
const long slow_speed = 45;
const long turning_speed = 45;
const long turnup_time = 3000;
const long slow_time = 0;
const long max_offline_time = 2000;


enum lap_direction {CLOCKWISE, COUNTERCLOCKWISE};
enum direction {DLEFT, DRIGHT, DSTRAIGHT};
enum robot_mode {NORMAL, AVOID};
enum line_value {ONLINE, RIGHT, LEFT, OFFLINE};

struct robot {
  lap_direction dir = CLOCKWISE;
  robot_mode mode = NORMAL;
  line_value line;
  int distance;
  direction turn_direction;
  long avoid_start_time;
  long avoid_end_time;
  long offline_time = -1;
  line_value last_turn;
};

robot robotState;

void motorController(int leftSpeed, int rightSpeed) {
  zRobotSetMotorSpeed(1, -leftSpeed);
  zRobotSetMotorSpeed(2, rightSpeed);
}

void readLineValue(){
  int lineval = zRobotGetLineSensor();
  robotState.line = static_cast<line_value>(lineval);
  if (robotState.mode == AVOID && robotState.line != OFFLINE && (millis() - robotState.avoid_start_time > 1200)) {
    robotState.avoid_end_time = millis(); // Record the time when avoidance ended
    robotState.mode = NORMAL;       // Switch back to NORMAL mode after avoidance and line is detected
  }
}

void readDistanceValue(){
  int distance = zRobotGetUltraSensor();
  if (robotState.mode == AVOID) {
    return; // If already in AVOID mode, don't change it based on distance
  }
  robotState.distance = distance;
  if (distance < 25) {
    robotState.mode = AVOID;
    robotState.avoid_start_time = millis(); // Record the time when avoidance started

  } else {
    //robotState.mode = NORMAL;
  }
}

void updateRobotState() {
  Serial.println("Line Sensor Value: " + String(robotState.line));

  switch (robotState.mode) {
    case NORMAL:
      if (robotState.line != OFFLINE) {
        robotState.offline_time = -1; // Reset offline time if the robot is back online
      }
      switch(robotState.line){
      case ONLINE:   // inga dvs båda på linjen
        if (robotState.avoid_end_time != 0 && millis() - robotState.avoid_end_time < turnup_time) {
          if (robotState.dir == CLOCKWISE) {
            motorController(0, speed); // Turn right in place
          } else {
            motorController(speed, 0); // Turn left in place
          }
          break;
        }
        if (millis() - robotState.avoid_end_time < turnup_time + slow_time) {
          motorController(slow_speed, slow_speed); // Move straight slower for a while after avoidance
        }
        motorController(speed, speed); // Move straight
        break;
      case RIGHT:   // 1 höger sensor, höger utanför linjen
        motorController(speed, speed*0.5);
        robotState.last_turn = RIGHT; // Record the last turn direction
        break;
      case LEFT:   // 2 vänster sensor, vänster utanför linjen
        motorController(speed*0.5, speed);
        robotState.last_turn = LEFT; // Record the last turn direction
        break;
      case OFFLINE:   // 3 båda sensorerna, båda utanför??
        robotState.offline_time = millis(); // Record the time when the robot went offline
        if (robotState.offline_time != -1 && millis() - robotState.offline_time > max_offline_time) {
          // If the robot has been offline for too long, stop it
          motorController(0, 0);
        } else {
          if (robotState.last_turn == LEFT) {
            motorController(0, speed); // Turn left
          } else if (robotState.last_turn == RIGHT) {
            motorController(speed, 0); // Turn right
          }
        }
        break;
      }
      break;
    case AVOID:
      // Make robot turn and then go in a cricle around the object
      if (millis() - robotState.avoid_start_time < 500) {
        if (robotState.dir == CLOCKWISE) {
          motorController(-speed, speed); // Turn right in place
        } else {
          motorController(speed, -speed); // Turn left in place
        }

      } else {
        // After turning, go straight for a while
        if (robotState.dir == CLOCKWISE) {
          motorController(speed, turning_speed);
        } else {
          motorController(turning_speed, speed);
        }
      }

      break;
  }
}

void setup() {
  zInitialize();
  Serial.begin(9600);
  zScheduleTask(readLineValue, 3, 10);
  zScheduleTask(readDistanceValue, 6, 10);
  zScheduleTask(updateRobotState, 3, 10);
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
