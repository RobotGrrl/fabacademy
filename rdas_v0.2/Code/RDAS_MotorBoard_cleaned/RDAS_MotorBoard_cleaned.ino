/*

RDAS v0.2 - Motor Board code

Erin RobotGrrl
CC BY-SA

http://fabacademy.org/archives/2015/na/students/kennedy.erin

Sorry in advance for the code not having comments and being written
wackily. I wrote it a few hours prior to presenting RDAS as my final
project!

(Note: This code is for the sensor board that was hand wired with the
 perf board and wire wrapping wire. The code for the pcb version of the
 sensor board will be different than this)

*/


#include <Streaming.h>
#include <SoftwareSerial.h>
#include "Promulgate.h"

#define DEBUG true

#define rx_pin A4
#define tx_pin A5

SoftwareSerial comm(rx_pin, tx_pin);

Promulgate sensor_promulgate = Promulgate(&comm, &comm);
Promulgate serial_promulgate = Promulgate(&Serial, &Serial);

struct Side {
  uint8_t en;
  uint8_t in1;
  uint8_t in2;
  uint8_t led1; 
  uint8_t led2;
  uint8_t out_a;
  uint8_t out_b;
};

Side left_motor = { 10, A2, A3, 8, 12, 3, 5 };
Side right_motor = { 9, A0, A1, 6, 7, 2, 4 };

struct MotorTest {  
  boolean done;
  uint8_t win;
  long start_time;
  uint16_t total_time;
  uint16_t ticks;
  uint16_t en;
  uint8_t in1;
  uint8_t in2;
  String name;
};

MotorTest enc_test[4] = {
  { false, 0, 0, 0, 0, left_motor.en, left_motor.in1, left_motor.in2, "L-A" },
  { false, 0, 0, 0, 0, left_motor.en, left_motor.in1, left_motor.in2, "L-B" },
  { false, 0, 0, 0, 0, right_motor.en, right_motor.in1, right_motor.in2, "R-A" },
  { false, 0, 0, 0, 0, right_motor.en, right_motor.in1, right_motor.in2, "R-B" }
};

uint8_t test_speed = 255;
uint8_t test_num = 0;
long current_time = 0;
int blinky_led = 0;
boolean blinky_dir = true;
long last_blinky = 0;

boolean motor_watch = false;
int motor_watchtime = 0;
long motor_starttime = 0;
uint16_t next_step = 0;

volatile uint16_t left_a = 0;
volatile uint16_t left_b = 0;
volatile uint16_t right_a = 0;
volatile uint16_t right_b = 0;



void setup() {
  
  Serial.begin(9600);
  comm.begin(9600);
  
  Serial << "RDAS Motor Board v1" << endl;

  sensor_promulgate.LOG_LEVEL = Promulgate::ERROR_;
  sensor_promulgate.set_rx_callback(received_action);
  sensor_promulgate.set_tx_callback(transmit_complete);

  serial_promulgate.LOG_LEVEL = Promulgate::ERROR_;
  serial_promulgate.set_rx_callback(received_action);
  serial_promulgate.set_tx_callback(transmit_complete);

  pinMode(left_motor.en, OUTPUT);
  pinMode(left_motor.in1, OUTPUT);
  pinMode(left_motor.in2, OUTPUT);
  pinMode(left_motor.led1, OUTPUT);
  pinMode(left_motor.led2, OUTPUT);
  pinMode(left_motor.out_a, INPUT);
  pinMode(left_motor.out_b, INPUT);

  pinMode(right_motor.en, OUTPUT);
  pinMode(right_motor.in1, OUTPUT);
  pinMode(right_motor.in2, OUTPUT);
  pinMode(right_motor.led1, OUTPUT);
  pinMode(right_motor.led2, OUTPUT);
  pinMode(right_motor.out_a, INPUT);
  pinMode(right_motor.out_b, INPUT);
  
  analogWrite(left_motor.en, 0);
  digitalWrite(left_motor.in1, LOW);
  digitalWrite(left_motor.in2, LOW);

  analogWrite(right_motor.en, 0);
  digitalWrite(right_motor.in1, LOW);
  digitalWrite(right_motor.in2, LOW);
  
  goForwards(200, 300);
  
}

void loop() {
  
  current_time = millis();
  
  if(motor_watch) {
    if(current_time-motor_starttime > motor_watchtime) {
      stopMotors();
      motor_watch = false;
      next_step++;
      Serial << "next" << endl;
      sensor_promulgate.transmit_action('^', 'Z', 1, 1, '!');
      serial_promulgate.transmit_action('^', 'Z', 1, 1, '!');
    }
  }
  
  
  if(current_time-last_blinky > 100) {
    blinkEncoderLeds();
    last_blinky = current_time;
  }

  while(comm.available()) {
    char c = comm.read();
    sensor_promulgate.organize_message(c);
  }
  
  while(Serial.available()) {
    char c = Serial.read();
    serial_promulgate.organize_message(c);    
  }
  
  //motorEncoderTest();
  
}

void blinkEncoderLeds() {
  
  // just blinking for fun- not really showing the encoders
  // (which weren't working)
  
  digitalWrite(left_motor.led1, LOW);
  digitalWrite(left_motor.led2, LOW);
  digitalWrite(right_motor.led1, LOW);
  digitalWrite(right_motor.led2, LOW);
  
  switch(blinky_led) {
    case 0:
    digitalWrite(left_motor.led2, HIGH);
    break;
    case 1:
    digitalWrite(left_motor.led1, HIGH);
    break;
    case 2:
    digitalWrite(right_motor.led2, HIGH);
    break;
    case 3:
    digitalWrite(right_motor.led1, HIGH);
    break;
  }
  
  if(blinky_dir) {
    blinky_led++;
  } else {
    blinky_led--; 
  }
  
  if(blinky_led > 3) {
    blinky_dir = !blinky_dir;
    blinky_led = 2;
  }
  
  if(blinky_led < 0) {
    blinky_dir = !blinky_dir;
    blinky_led = 1;
  }
  
  /*
  if(digitalRead(left_motor.out_a) == HIGH) {
    digitalWrite(left_motor.led1, HIGH);
  } else {
    digitalWrite(left_motor.led1, LOW);
  }
  if(digitalRead(left_motor.out_b) == HIGH) {
    digitalWrite(left_motor.led2, HIGH);
  } else {
    digitalWrite(left_motor.led2, LOW);
  }
  if(digitalRead(right_motor.out_a) == HIGH) {
    digitalWrite(right_motor.led1, HIGH);
  } else {
    digitalWrite(right_motor.led1, LOW);
  }
  if(digitalRead(right_motor.out_b) == HIGH) {
    digitalWrite(right_motor.led2, HIGH);
  } else {
    digitalWrite(right_motor.led2, LOW);
  }
  */
}

void goPlaces(int s1, int s2) {
  
  int temp = 0;
  int new_speed = 0;
  
  if(s1 < 50) {
    // backwards
    temp = abs(s1-50);
    new_speed = map(temp, 0, 50, 0, 255);
    analogWrite(left_motor.en, new_speed);
    digitalWrite(left_motor.in1, LOW);
    digitalWrite(left_motor.in2, HIGH);
  } else {
   // forwards
    temp = abs(s1-100);
    new_speed = map(temp, 0, 50, 0, 255);
    analogWrite(left_motor.en, new_speed);
    digitalWrite(left_motor.in1, HIGH);
    digitalWrite(left_motor.in2, LOW);
  }
  
  if(s2 < 50) {
    // backwards
    temp = abs(s2-50);
    new_speed = map(temp, 0, 50, 0, 255);
    analogWrite(right_motor.en, new_speed);
    digitalWrite(right_motor.in1, HIGH);
    digitalWrite(right_motor.in2, LOW);
  } else {
   // forwards
    temp = abs(s2-100);
    new_speed = map(temp, 0, 50, 0, 255);
    analogWrite(right_motor.en, new_speed);
    digitalWrite(right_motor.in1, LOW);
    digitalWrite(right_motor.in2, HIGH);
  }
  
}

void goForwards(int s, int d) {
  
  analogWrite(left_motor.en, s);
  digitalWrite(left_motor.in1, HIGH);
  digitalWrite(left_motor.in2, LOW);
  
  analogWrite(right_motor.en, s);
  digitalWrite(right_motor.in1, LOW);
  digitalWrite(right_motor.in2, HIGH);
  
  motor_watch = true;
  motor_watchtime = d;
  motor_starttime = millis();
  
}

void goBackwards(int s, int d) {
  
  analogWrite(left_motor.en, s);
  digitalWrite(left_motor.in1, LOW);
  digitalWrite(left_motor.in2, HIGH);
  
  analogWrite(right_motor.en, s);
  digitalWrite(right_motor.in1, HIGH);
  digitalWrite(right_motor.in2, LOW);
  
  motor_watch = true;
  motor_watchtime = d;
  motor_starttime = millis();
  
}

void turnRight(int s, int d) {
  
  analogWrite(left_motor.en, s);
  digitalWrite(left_motor.in1, LOW);
  digitalWrite(left_motor.in2, HIGH);
  
  analogWrite(right_motor.en, s);
  digitalWrite(right_motor.in1, LOW);
  digitalWrite(right_motor.in2, HIGH);
  
  motor_watch = true;
  motor_watchtime = d;
  motor_starttime = millis();
  
}

void turnLeft(int s, int d) {
  
  analogWrite(left_motor.en, s);
  digitalWrite(left_motor.in1, HIGH);
  digitalWrite(left_motor.in2, LOW);
  
  analogWrite(right_motor.en, s);
  digitalWrite(right_motor.in1, HIGH);
  digitalWrite(right_motor.in2, LOW);
  
  motor_watch = true;
  motor_watchtime = d;
  motor_starttime = millis();
  
}

void stopMotors() {
  
  analogWrite(left_motor.en, 0);
  digitalWrite(left_motor.in1, LOW);
  digitalWrite(left_motor.in2, LOW);
  
  analogWrite(right_motor.en, 0);
  digitalWrite(right_motor.in1, LOW);
  digitalWrite(right_motor.in2, LOW);
  
}



void motorEncoderTest() {
 resetVals();
  
  while(test_num < 10) {
  
  current_time = millis();
  
  for(int i=0; i<4; i++) {
    
   enc_test[0].ticks = left_a;
   enc_test[1].ticks = left_b;
   enc_test[2].ticks = right_a;
   enc_test[3].ticks = right_b;
  
   if(enc_test[i].ticks < 24) {
     analogWrite(enc_test[i].en, test_speed);
     digitalWrite(enc_test[i].in1, HIGH);
     digitalWrite(enc_test[i].in2, LOW);
   } else if(enc_test[i].ticks >= 24 && enc_test[i].done != true) {
     enc_test[i].total_time = current_time-enc_test[i].start_time;
     enc_test[i].done = true;
   }
  
  if(current_time-enc_test[i].start_time >= 5000 && enc_test[i].done != true) {
    enc_test[i].total_time = 9999;
    enc_test[i].done = true;
  }
  
  /*
  // if one encoder is done, and the other is not, then this effects the speed of the motor - 
  // turning it on and off again. so will just wait for the 5 seconds to be up, then turn off
  // the motor
  if(enc_test[i].done) {
    analogWrite(enc_test[i].en, 0);
    digitalWrite(enc_test[i].in1, LOW);
    digitalWrite(enc_test[i].in2, LOW);
  }
  */
  
  }
  
  if(enc_test[0].done == true && enc_test[1].done == true && enc_test[2].done == true && enc_test[3].done == true) {
    
    for(int i=0; i<4; i++) {
      
      analogWrite(enc_test[i].en, 0);
      digitalWrite(enc_test[i].in1, LOW);
      digitalWrite(enc_test[i].in2, LOW);
      
      if(enc_test[i].total_time == 9999) {
        enc_test[i].win = 0;
      } else {
        enc_test[i].win = 1;
      }
      
      Serial << enc_test[i].name << "," << test_num << "," << test_speed << "," << enc_test[i].win << "," << enc_test[i].ticks << "," << enc_test[i].total_time << endl;
      
    }
    
    test_num++;
    resetVals();
    delay(1000);
    
  }
  
  } 
}

void resetVals() {
 
  current_time = millis();
  left_a = 0;
  left_b = 0;
  right_a = 0;
  right_b = 0;
  
  for(int i=0; i<4; i++) {
    enc_test[i].done = false;
    enc_test[i].win = 0;
    enc_test[i].start_time = current_time;
    enc_test[i].total_time = 0;
  }
  
}



void received_action(char action, char cmd, uint8_t key, uint16_t val, char delim) {
  
  if(DEBUG) {
    Serial << "---CALLBACK---" << endl;
    Serial << "action: " << action << endl;
    Serial << "command: " << cmd << endl;
    Serial << "key: " << key << endl;
    Serial << "val: " << val << endl;
    Serial << "delim: " << delim << endl;
  }
  
  // #P<s>,<d>!
  // <s> from 0-255, <d> from 0-65536
  // P: goPlaces
  // F: goForwards
  // B: goBackwards
  // L: turnLeft
  // R: turnRight
  // S: stopMotors
  
  // this is sent to the sensor board when the motors are finished moving:
  // ^Z1,1!
  
  if(action == '#') {
  
    if(cmd == 'P') { // goPlaces
      goPlaces(key, val);
    } else if(cmd == 'F') { // goForwards
      goForwards(key, val);
    } else if(cmd == 'B') { // goBackwards
      goBackwards(key, val);
    } else if(cmd == 'L') { // turnLeft
      turnLeft(key, val);
    } else if(cmd == 'R') { // turnRight
      turnRight(key, val);
    } else if(cmd == 'S') { // stopMotors
      stopMotors();
      sensor_promulgate.transmit_action('^', 'Z', 1, 1, '!');
      serial_promulgate.transmit_action('^', 'Z', 1, 1, '!');
    }
  
  }
  
}

void transmit_complete() {
  if(DEBUG) {
    Serial << "--transmit complete--" << endl;
  }
}



