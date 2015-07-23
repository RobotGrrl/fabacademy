/*

RDAS v0.2 - Sensor Board code

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

#define DEBUG false
#define QUIET true

#define comm_rx A5
#define comm_tx A4

#define xbee_rx 4
#define xbee_tx 5

SoftwareSerial comm(comm_rx, comm_tx);

Promulgate motor_promulgate = Promulgate(&comm, &comm);
Promulgate serial_promulgate = Promulgate(&Serial, &Serial);

int led_r = 11;
int led_l = 6;
int sonar = A0;
int spkr = 8;
int blue_led = 12;
int unfold_button = 10;

int sonar_thresh = 15;
boolean sonar_wait = false;
int sonar_step = 0;
int sonar_val = 0;
boolean leds_on = false;
int sonar_trig = 0;
long last_sonar_trig = 0;

uint16_t next_step = 0;
boolean folded = true;

long current_time = 0;
long last_receive = 0;
long last_xbee = 0;
boolean not_sent_yet = true;

int MOTOR_SPEED = 100;

void setup() {
  
  Serial.begin(9600);
  comm.begin(9600);
    
  motor_promulgate.LOG_LEVEL = Promulgate::ERROR_;
  motor_promulgate.set_rx_callback(received_action);
  motor_promulgate.set_tx_callback(transmit_complete);
  
  serial_promulgate.LOG_LEVEL = Promulgate::ERROR_;
  serial_promulgate.set_rx_callback(received_action);
  serial_promulgate.set_tx_callback(transmit_complete);
  
  pinMode(led_r, OUTPUT);
  pinMode(led_l, OUTPUT);
  pinMode(sonar, INPUT);
  pinMode(spkr, OUTPUT);
  pinMode(blue_led, OUTPUT);
  pinMode(unfold_button, INPUT);
  
  playTone(700, 100);
  delay(50);
  playTone(300, 50);
  playTone(200, 50);
  playTone(100, 50);
  
}

void loop() {
  
  
  current_time = millis();
  
  
  if(digitalRead(unfold_button) == HIGH) {
    digitalWrite(blue_led, LOW);
    folded = true;
  } else {
   digitalWrite(blue_led, (current_time%1000) < 100 || ((current_time%1000) > 200 && (current_time%1000) < 300));// || ((current_time%1000) > 400 && (current_time%1000) < 500));
    
    if(folded == true) {
      delay(1000);
      motor_promulgate.transmit_action('#', 'F', MOTOR_SPEED, 2000, '!');
      analogWrite(led_r, 255);
      analogWrite(led_l, 255);
    }
    
    folded = false;
  }
  
  
  if(current_time-last_xbee > 10000 && not_sent_yet == true) {
    motor_promulgate.transmit_action('#', 'F', MOTOR_SPEED, 1000, '!');
    analogWrite(led_r, 255);
    analogWrite(led_l, 255);
    not_sent_yet = false;
  }
  
  
  
  sonar_val = analogRead(sonar);
  //Serial << sonar_val << endl;
  delay(50);  
  
  
  if(sonar_val < sonar_thresh) {
    sonar_trig++;
    last_sonar_trig = current_time;
    if(DEBUG) Serial << "sonar_trig: " << sonar_trig << endl;
  }
  
  if(current_time-last_sonar_trig > 500) sonar_trig = 0;
  
  if(sonar_trig > 10 && sonar_wait == false) {
    // stop, backup, and turn
    motor_promulgate.transmit_action('#', 'S', 1, 1, '!');
    //serial_promulgate.transmit_action('#', 'O', 1, 1, '!'); // for some reason the TX to the headband is borked
    playTone(900, 100);
    delay(50);
    playTone(700, 100);
    delay(50);
    sonar_wait = true;
    sonar_trig = 0;
  }
  
  
  if(current_time-last_receive > 3000 && sonar_wait != true) {
    // kick it
    motor_promulgate.transmit_action('#', 'S', 1, 1, '!');
    analogWrite(led_r, 0);
    analogWrite(led_l, 0);
  }

  while(comm.available()) {
    char c = comm.read();
    motor_promulgate.organize_message(c);
  }
  
  while(Serial.available()) {
    char c = Serial.read();
    //playTone(700, 50);
    //delay(50);
    serial_promulgate.organize_message(c);
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
    
    if(folded) return;
    
    motor_promulgate.transmit_action('#', cmd, key, val, '!');
    
    if(cmd == 'P') {
      analogWrite(led_r, 255);
      analogWrite(led_l, 255);
    } else if(cmd == 'F') {
      analogWrite(led_r, 255);
      analogWrite(led_l, 255);
    } else if(cmd == 'B') {
      analogWrite(led_r, 10);
      analogWrite(led_l, 10);
    } else if(cmd == 'R') {
      for(int i=0; i<5; i++) {
        analogWrite(led_r, 255);
        delay(100);
        analogWrite(led_r, 10);
        delay(100);
      }
      analogWrite(led_r, 255);
    } else if(cmd == 'L') {
      for(int i=0; i<5; i++) {
        analogWrite(led_l, 255);
        delay(100);
        analogWrite(led_l, 10);
        delay(100);
      }
      analogWrite(led_l, 255);
    } else if(cmd == 'S') {
      analogWrite(led_r, 0);
      analogWrite(led_l, 0);
    }
    
    last_xbee = current_time;
    not_sent_yet = true;
    
  }
  
  
  
  if(action == '^') {
  
    if(cmd == 'Z') { // motor finished moving
    
      if(sonar_wait) {
        
        if(sonar_step == 0) {
          // backup 
          motor_promulgate.transmit_action('#', 'B', MOTOR_SPEED, 3000, '!');
          analogWrite(led_r, 10);
          analogWrite(led_l, 10);
          sonar_step++;
        } else if(sonar_step == 1) {
          // turn
          motor_promulgate.transmit_action('#', 'R', MOTOR_SPEED, 3000, '!'); 
          for(int i=0; i<5; i++) {
            analogWrite(led_r, 255);
            delay(100);
            analogWrite(led_r, 10);
            delay(100);
          }
          analogWrite(led_r, 255);
          sonar_step++;
        } else if(sonar_step == 2) {
          // forwards
          motor_promulgate.transmit_action('#', 'F', MOTOR_SPEED, 1000, '!');
          analogWrite(led_r, 255);
          analogWrite(led_l, 255);
          sonar_step++;
        } else if(sonar_step == 3) {
          sonar_wait = false;
          sonar_step = 0;
          last_receive = millis();
          motor_promulgate.transmit_action('#', 'S', 1, 1, '!');
          analogWrite(led_r, 0);
          analogWrite(led_l, 0);
          delay(100);
        }
        
      } else {
      
        next_step++;
        last_receive = millis();
        
        if(folded) return;
        
        if(current_time-last_xbee < 10000) return;
        
        if(next_step%2 == 0) {
          motor_promulgate.transmit_action('#', 'F', MOTOR_SPEED, 1000, '!');
          analogWrite(led_r, 255);
          analogWrite(led_l, 255);
        } else {
          motor_promulgate.transmit_action('#', 'R', MOTOR_SPEED, 2000, '!'); 
          for(int i=0; i<5; i++) {
            analogWrite(led_r, 255);
            delay(100);
            analogWrite(led_r, 10);
            delay(100);
          }
          analogWrite(led_r, 255);
          analogWrite(led_l, 255);
          last_receive = millis();
        }
        
        
        
      
      }
      
    }
    
  }
  
}

void transmit_complete() {
  if(DEBUG) {
    Serial << "--transmit complete--" << endl;
  }
}



void playTone(int tone, int duration) {
  
  if(!QUIET) {
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
      digitalWrite(spkr, HIGH);
      delayMicroseconds(tone);
      digitalWrite(spkr, LOW);
      delayMicroseconds(tone);
    }
  }

}


