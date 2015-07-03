/* Rapidly Deployable Automation System - Drive Bot
 * ------------------------------------------------
 *
 * Erin K - March 30, 2015
 */

#include <Streaming.h>
#include <SoftwareSerial.h>

const uint8_t motor_left_enable = 11;
const uint8_t motor_left_a = 12;
const uint8_t motor_left_b = 8;

const uint8_t headlights = 9;
const uint8_t led = 13;

const uint8_t motor_right_enable = 10;
const uint8_t motor_right_a = 7;
const uint8_t motor_right_b = 6;

const uint8_t xbee_rx = 5;
const uint8_t xbee_tx = 4;

const uint8_t encoder_left_a = 3;
const uint8_t encoder_right_a = 2;

const uint8_t ultrasonic_sensor = A0;
const uint8_t encoder_left_b = A1;
const uint8_t encoder_right_b = A2;

// ---

SoftwareSerial xbee(xbee_tx, xbee_rx);

uint16_t ultrasonic_val = 0;

long current_time = 0;
long last_spin = 0;
boolean spin_dir = true;
long last_send = 0;


int speed_val = 0;
long last_rx = 0;



float sensVal;           // for raw sensor values 
float filterVal = 0.0015;       // this determines smoothness  - .0001 is max  1 is off (no smoothing)
float smoothedVal;     // this holds the last loop value just use a unique variable for every different sensor that needs smoothing
long last_trigger = 0;
int trigger_count = 0;
long last_play = 0;


boolean headband_mode = false;


void setup() {
  Serial.begin(9600);
  xbee.begin(9600);
  
  pinMode(motor_left_enable, OUTPUT);
  pinMode(motor_left_a, OUTPUT);
  pinMode(motor_left_b, OUTPUT);
  
  pinMode(headlights, OUTPUT);
  
  pinMode(motor_right_enable, OUTPUT);
  pinMode(motor_right_a, OUTPUT);
  pinMode(motor_right_b, OUTPUT);
  
  pinMode(encoder_left_a, INPUT);
  pinMode(encoder_right_a, INPUT);
  
  pinMode(ultrasonic_sensor, INPUT);
  pinMode(encoder_left_b, INPUT);
  pinMode(encoder_right_b, INPUT);
  
  Serial << "Welcome to RDAS" << endl;
  
  analogWrite(headlights, 255);
  
  for(int i=0; i<5; i++) {
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(50);
  }
  
  
  /*
  move_left_motor(true, 128);
  move_right_motor(true, 128);
  delay(5000);
  move_left_motor(true, 0);
  move_right_motor(true, 0);
  */
  
}

void loop() {

  current_time = millis();
  
  ultrasonic_val = analogRead(ultrasonic_sensor);
  //analogWrite(headlights, map(ultrasonic_val, 0, 1023, 0, 256));
  
  //Serial << ultrasonic_val << endl;
  //delay(100);
  
  
  
  smoothedVal = smooth(ultrasonic_val/2, filterVal, smoothedVal);   // second parameter determines smoothness  - 0 is off,  .9999 is max smooth 
  //Serial << smoothedVal << endl;

  if(smoothedVal < 8) {
    trigger_count++;
    last_trigger = current_time;
  }
  
  if(current_time-last_trigger > 500) {
    trigger_count = 0;
  }
  
  if(trigger_count > 20 && headband_mode == false) {
    //Serial << "triggered!" << endl;
    
    if(current_time-last_play > 7000) {
      
      analogWrite(headlights, 10);
      
      
      move_left_motor(false, 255);
      move_right_motor(false, 255);
    
      delay(2000);
      
      move_left_motor(false, 255);
      move_right_motor(true, 255);
    
      delay(4000);
     
        move_left_motor(false, 0);
      move_right_motor(true, 0);
      delay(50);
      
      analogWrite(headlights, 255);
      
      
      last_play = current_time;
    }
   
    
  }
  
  
  
  
  
  
  
  
  
  
  uint16_t slice = millis() % 1000;
  digitalWrite(led, slice < 100 || (slice > 200 && slice < 300));
  
  
  if(current_time-last_spin > 3000 && headband_mode == false) {
    
    if(spin_dir) {
      move_left_motor(true, 128);
      move_right_motor(false, 128);
    } else {
      move_left_motor(false, 128);
      move_right_motor(true, 128);
    }
    
    spin_dir = !spin_dir;
    last_spin = current_time;
  } else if(headband_mode == false) {
    move_left_motor(true, 128);
      move_right_motor(true, 128);
  }
  
  
  
  
  
  
  
  /*
  if(current_time-last_send > 1000) {
    xbee.println("hi");
    Serial << "hi" << endl;
    last_send = current_time;
  }
  */
  
  if(xbee.available() > 0) {
    speed_val = xbee.parseInt();
    Serial << speed_val << endl;
    
    if(speed_val == 0) {
      char c = xbee.read();
      Serial << c << endl;
      if(c == 'A') {
        shake();
      } else if(c == 'B') {
        shake();
      }
    }
    
    last_rx = current_time;
    headband_mode = true;
//    Serial << (char)xbee.read();
  }
  
  
  if(current_time-last_rx < 200) {
    move_left_motor(true, speed_val);
    move_right_motor(true, speed_val);
  } else {
    headband_mode = false;
  }
  
  
  delay(20);
  
}


void shake() {
 
  for(int i=0; i<3; i++) {
    move_left_motor(false, 128);
    move_right_motor(true, 128);
    delay(500);
    move_left_motor(true, 128);
    move_right_motor(false, 128);
    delay(500);
  }
  
}


void move_left_motor(boolean dir, uint8_t top_speed) {
  if(top_speed == 0) {
    digitalWrite(motor_left_enable, LOW);
    digitalWrite(motor_left_a, LOW);
    digitalWrite(motor_left_b, LOW);
    
  } else {
    analogWrite(motor_left_enable, top_speed);
    
    if(dir) {
      digitalWrite(motor_left_a, HIGH);
      digitalWrite(motor_left_b, LOW);
    } else {
      digitalWrite(motor_left_a, LOW);
      digitalWrite(motor_left_b, HIGH);
    }
    
  }  
}

void move_right_motor(boolean dir, uint8_t top_speed) {
  if(top_speed == 0) {
    digitalWrite(motor_right_enable, LOW);
    digitalWrite(motor_right_a, LOW);
    digitalWrite(motor_right_b, LOW);
    
  } else {
    analogWrite(motor_right_enable, top_speed);
    
    if(dir) {
      digitalWrite(motor_right_a, HIGH);
      digitalWrite(motor_right_b, LOW);
    } else {
      digitalWrite(motor_right_a, LOW);
      digitalWrite(motor_right_b, HIGH);
    }
    
  }
}



int smooth(int data, float filterVal, float smoothedVal){


  if (filterVal > 1){      // check to make sure param's are within range
    filterVal = .99;
  }
  else if (filterVal <= 0){
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;
}



