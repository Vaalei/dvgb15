#include <Arduino.h>
#include "kaulab.h"
unsigned long timer = 0;
const long speed = 80;
const long slow_speed = 45;
const long turning_speed = 45;
const long turnup_time = 3000;
const long slow_time = 0;
const long max_offline_time = 2000;
const long TURNOUT_TIME = 600; 

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
  line_value prev_line = ONLINE;
  int left_count = 2;
  int right_count = 2;

};

robot robotState;

void motorController(int leftSpeed, int rightSpeed) {
  zRobotSetMotorSpeed(1, -leftSpeed);
  zRobotSetMotorSpeed(2, rightSpeed);
}

void checkLapDirection() {

  if (robotState.left_count > robotState.right_count) {
    robotState.dir = CLOCKWISE;
    zSetOneLed(1, 0, 255, 0); 
    
  }
  if (robotState.right_count > robotState.left_count) {
    robotState.dir = COUNTERCLOCKWISE;
    zSetOneLed(1, 0, 0, 255); 
  }
}

void readLineValue(){
  int lineval = zRobotGetLineSensor();
  robotState.line = static_cast<line_value>(lineval);
  checkLapDirection(); // new counter 
  if (robotState.mode == AVOID && robotState.line != OFFLINE && (millis() - robotState.avoid_start_time > 1200)) {
    robotState.avoid_end_time = millis(); 
    robotState.mode = NORMAL;       
  }
}



void readDistanceValue(){
  int distance = zRobotGetUltraSensor();
  if (robotState.mode == AVOID) {
    return; 
  }
  robotState.distance = distance;
  if (distance < 25) {
    robotState.mode = AVOID;
    robotState.avoid_start_time = millis(); 

  } else {
    
  }
}

void updateRobotState() {
  Serial.println("Line Sensor Value: " + String(robotState.line));

  switch (robotState.mode) {
    case NORMAL:
      if (robotState.line != OFFLINE) {
        robotState.offline_time = -1; 
      }
      switch(robotState.line){
      case ONLINE:   
        if (robotState.avoid_end_time != 0 && millis() - robotState.avoid_end_time < turnup_time) {
          if (robotState.dir == CLOCKWISE) {
            motorController(0, speed); 
          } else {
            motorController(speed, 0); 
          }
          break;
        }
        if (millis() - robotState.avoid_end_time < turnup_time + slow_time) {
          motorController(slow_speed, slow_speed); 
        }
        motorController(speed, speed); 
        break;
      case RIGHT:   
        motorController(speed, speed*0.5);
        robotState.last_turn = RIGHT; 
        if (robotState.left_count > 0) {
          robotState.right_count++;
          robotState.left_count--;
        }
        break;
      case LEFT:  
        motorController(speed*0.5, speed);
        robotState.last_turn = LEFT; 
         if (robotState.right_count > 0) {
          robotState.left_count++;
          robotState.right_count--;
        }
        break;
      case OFFLINE:   
        robotState.offline_time = millis(); 
        if (robotState.offline_time != -1 && millis() - robotState.offline_time > max_offline_time) {
          
          motorController(0, 0);
        } else {
          if (robotState.last_turn == LEFT) {
            motorController(0, speed); 
          } else if (robotState.last_turn == RIGHT) {
            motorController(speed, 0); 
          }
        }
        break;
      }
      break;
    case AVOID:
     
      if (millis() - robotState.avoid_start_time < TURNOUT_TIME) {
        if (robotState.dir ==  CLOCKWISE) {
          motorController(-speed, speed); 
        } else {
          motorController(speed, -speed); 
        }

      } else {
       
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

 

}

void loop() {



}

