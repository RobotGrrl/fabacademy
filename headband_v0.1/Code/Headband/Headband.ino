/*

Headband v0.1

Uses accel, gyro to create pitch data
Pitch data sent to robot through xbee when button 1 is pressed
Pulse sensor changes leds from green to red when heartbeat detected

Rough sketch of the code - very scatter brained

*/

#include <Adafruit_NeoPixel.h>
#include <Adafruit_TiCoServo.h>
#include <Streaming.h>
#include <SoftwareSerial.h>

int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat

// these variables are volatile because they are used during the interrupt service routine!
volatile int BPM;                   // used to hold the pulse rate
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // holds the time between beats, must be seeded! 
volatile boolean Pulse = false;     // true when pulse wave is high, false when it's low
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// NeoPixel parameters. These are configurable, but the pin number must
// be different than the servo(s).
#define N_LEDS       5
#define LED_PIN       6

Adafruit_NeoPixel  strip = Adafruit_NeoPixel(N_LEDS, LED_PIN);
Adafruit_TiCoServo servo_L;
Adafruit_TiCoServo servo_R;

#define X_PIN A1
#define Y_PIN A2
#define Z_PIN A3

#define GYRO_PIN A5

#define BUTTON_3 8
#define BUTTON_2 7
#define BUTTON_1 5
#define LDR A4

#define XBEE_RX 2
#define XBEE_TX 3

SoftwareSerial xbee(XBEE_RX, XBEE_TX);

float xval = 0;
float yval = 0;
float zval = 0;

float xcount = 0;
float ycount = 0;
float zcount = 0;

float xavg = 0;
float yavg = 0;
float zavg = 0;

uint16_t xthresh = 120;
uint16_t ythresh = 50;
uint16_t zthresh = 30;

long last_trigger_x = 0;
long last_trigger_y = 0;
long last_trigger_z = 0;


uint8_t window = 50;
uint8_t count = 0;

long current_time = 0;
long last_servo_update = 0;
long last_send = 0;

boolean pos_dir = true;
int pos = 0;


//The minimum and maximum values that came from
//the accelerometer while standing still
//You very well may need to change these
int minVal = 265;
int maxVal = 402;

//to hold the caculated values
double x;
double y;
double z;


float x_home = 0;
float y_home = 0;
float z_home = 0;

float x_acc = 0;
float y_acc = 0;
float z_acc = 0;

float acc_scale = 1.0;
float acc_pitch = 0;
float acc_roll = 0;

float gyro_home = 0;
float gyro_scale = 0.01;
float gyro_rate = 0;
float gyro_voltage = 0;
float gyro_zero_voltage = 0;
float max_voltage = 5;
float rotation_thresh = 3;
float gyro_pitch = 0;
long last_measure_time = 0;

float pitch = 0;
float alpha = 0.8;

int max_range = 20;
int min_speed = 0;
int max_speed = 255;

int speed_val = 0;
float pitch_start = 0;

long max_control_time = 10000;
long button3_press_time = 0;
long button2_press_time = 0;
long button1_press_time = 0;
boolean headband_speed_control = false;
boolean ears_wiggle_mode = false;
boolean rainbow_mode = false;


void setup() {
  
  pinMode(blinkPin, OUTPUT);
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);
  pinMode(Z_PIN, INPUT);
  pinMode(GYRO_PIN, INPUT);
  pinMode(BUTTON_3, INPUT);
  pinMode(BUTTON_2, INPUT);
  pinMode(BUTTON_1, INPUT);
  pinMode(LDR, INPUT);
  
  Serial.begin(9600);
  xbee.begin(9600);
  
  interruptSetup();                 // sets up to read Pulse Sensor signal every 2mS 

  servo_L.attach(SERVO_L_PIN, SERVO_L_MIN, SERVO_L_MAX);
  servo_R.attach(SERVO_R_PIN, SERVO_R_MIN, SERVO_R_MAX);
  
  servo_L.write(SERVO_L_HOME);
  servo_R.write(SERVO_R_HOME);
  
  strip.begin();
  
  x_home = analogRead(X_PIN);
  y_home = analogRead(Y_PIN);
  z_home = analogRead(Z_PIN);  
  
  gyro_home = analogRead(GYRO_PIN);
  gyro_zero_voltage = ( gyro_home * max_voltage ) / 1023;
  
}

void loop() {

  current_time = millis();
  strip.clear();

  if(QS == true) {                       // Quantified Self flag is true when arduino finds a heartbeat
    
    //Serial << "BPM: " << BPM << " IBI: " << IBI << endl;
  
    if(current_time - last_servo_update >= 1000) {
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 255, 0, 0);
    }
    strip.show();
    delay(20);
    }
       
    QS = false;                      // reset the Quantified Self flag for next time    
  
  } else if(headband_speed_control != true) {
  
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 0, 255, 0);
     }
     strip.show();
    
  }
     
        
  
  xval = 5*(analogRead(X_PIN)/1023);
  yval = 5*(analogRead(Y_PIN)/1023);
  zval = 5*(analogRead(Z_PIN)/1023);
  xcount += xval;
  ycount += yval;
  zcount += zval;
  count++;

  x_acc = (xval-x_home)/acc_scale;
  y_acc = (yval-y_home)/acc_scale;
  z_acc = (zval-z_home)/acc_scale;
  
  gyro_voltage = ( analogRead(GYRO_PIN) * max_voltage ) / 1023;
  gyro_rate = (gyro_voltage - gyro_zero_voltage) / gyro_scale;
  
  //Serial << "gyro: " << gyro_rate << endl;
  
  //Serial << "Acc X: " << x_acc << "Acc Y: " << y_acc << "Acc Z: " << z_acc << endl;
  
  acc_pitch = atan(x_acc / sqrt( pow(y_acc,2) + pow(z_acc,2) ) );
  acc_roll = atan(y_acc / sqrt( pow(x_acc,2) + pow(z_acc,2) ) );
  //float pitch = atan(xval / sqrt( pow(yval,2) + pow(zval,2) ) );
  //float roll = atan(yval / sqrt( pow(xval,2) + pow(zval,2) ) );
  acc_pitch = acc_pitch * (180/PI);
  acc_roll = acc_roll * (180/PI);
  
  //Serial << "pitch: " << pitch << " roll: " << roll << endl;
  //delay(100);
  
  
  if(gyro_rate > rotation_thresh || gyro_rate < -rotation_thresh) {
    gyro_pitch = gyro_pitch + gyro_rate * (current_time-last_measure_time)/1000.0;
    last_measure_time = current_time;
  }
  
  if(gyro_pitch > 360) gyro_pitch -= 360;
  if(gyro_pitch < -360) gyro_pitch += 360;
  
  //Serial << "pitch: " << current_angle << endl;
  //delay(100);
  
  
  
  pitch = alpha*gyro_pitch + (1-alpha)*acc_pitch;
  //Serial << "pitch: " << pitch << endl;
  //delay(100);
  
  
  
  
  
  
  
  if(digitalRead(BUTTON_3) == HIGH) {
    
    x_home = analogRead(X_PIN);
    y_home = analogRead(Y_PIN);
    z_home = analogRead(Z_PIN);  
    
    gyro_home = analogRead(GYRO_PIN);
    gyro_zero_voltage = ( gyro_home * max_voltage ) / 1023;
    
    
    xval = 5*(analogRead(X_PIN)/1023);
  yval = 5*(analogRead(Y_PIN)/1023);
  zval = 5*(analogRead(Z_PIN)/1023);
    
     x_acc = (xval-x_home)/acc_scale;
  y_acc = (yval-y_home)/acc_scale;
  z_acc = (zval-z_home)/acc_scale;
  
  gyro_voltage = ( analogRead(GYRO_PIN) * max_voltage ) / 1023;
  gyro_rate = (gyro_voltage - gyro_zero_voltage) / gyro_scale;
    
    acc_pitch = atan(x_acc / sqrt( pow(y_acc,2) + pow(z_acc,2) ) );
  acc_roll = atan(y_acc / sqrt( pow(x_acc,2) + pow(z_acc,2) ) );
  //float pitch = atan(xval / sqrt( pow(yval,2) + pow(zval,2) ) );
  //float roll = atan(yval / sqrt( pow(xval,2) + pow(zval,2) ) );
  acc_pitch = acc_pitch * (180/PI);
  acc_roll = acc_roll * (180/PI);
    
    if(gyro_rate > rotation_thresh || gyro_rate < -rotation_thresh) {
    gyro_pitch = gyro_pitch + gyro_rate * (current_time-last_measure_time)/1000.0;
    last_measure_time = current_time;
  }
  
  if(gyro_pitch > 360) gyro_pitch -= 360;
  if(gyro_pitch < -360) gyro_pitch += 360;
  
    pitch_start = alpha*gyro_pitch + (1-alpha)*acc_pitch;
    
    Serial << "yes" << endl;
    
    button3_press_time = current_time;
    headband_speed_control = true;
  }
  
  
  if(current_time-button3_press_time < max_control_time && headband_speed_control == true) {
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 0, 0, 255);
    }
    strip.show();
    delay(20);
    
    speed_val = (int)map(pitch, pitch_start-10, pitch_start+50, min_speed, max_speed);
    if(speed_val < min_speed) speed_val = min_speed;
    if(speed_val > max_speed) speed_val = max_speed;
    
    if(current_time-last_send > 100) {
      Serial << speed_val << endl;
      xbee << speed_val << endl;
      last_send = current_time;
    }
  
    
    //Serial << speed_val << endl;
    //delay(100);
    
  }
  
  
  if(current_time-button3_press_time > max_control_time && headband_speed_control == true) {
    headband_speed_control = false;
  }
  
  
  
  
  if(digitalRead(BUTTON_2) == HIGH) {
    
    xbee << "A" << endl;
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 0, 0, 255);
    }
    strip.show();
    delay(3000);
    button2_press_time = current_time;
    ears_wiggle_mode = true;
  }
  
  if(current_time-button2_press_time < max_control_time && ears_wiggle_mode == true) {
   
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 0, 0, 255);
    }
    strip.show();
    
    
    
    servo_L.attach(SERVO_L_PIN);
    servo_R.attach(SERVO_R_PIN);
    
    for(int i=0; i<30; i++) {
        servo_L.write(SERVO_L_HOME+i);
        servo_R.write(SERVO_R_HOME+i);
        delay(10);
      }
      
      for(int i=30; i>-30; i--) {
        servo_L.write(SERVO_L_HOME+i);
        servo_R.write(SERVO_R_HOME+i);      
        delay(10);
      }
      
      for(int i=-30; i<0; i++) {
        servo_L.write(SERVO_L_HOME+i);
        servo_R.write(SERVO_R_HOME+i);      
        delay(10);
      }
      
      servo_L.detach();
      servo_R.detach();
   
   
   last_servo_update = current_time;
    
  }
  
  if(current_time-button2_press_time > max_control_time && ears_wiggle_mode == true) {
    ears_wiggle_mode = false;
  }
  
  
  
  
  
  if(digitalRead(BUTTON_1) == HIGH) {
    
    xbee << "B" << endl;
    
    button1_press_time = current_time;
    rainbow_mode = true;
  }
  
  if(current_time-button1_press_time < max_control_time && rainbow_mode == true) {
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 255, 0, 0);
    }
    strip.show();
    
    delay(100);
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 128, 128, 0);
    }
    strip.show();
    
    delay(100);
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 0, 255, 0);
    }
    strip.show();
    
    delay(100);
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 0, 128, 128);
    }
    strip.show();
    
    delay(100);
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 0, 0, 255);
    }
    strip.show();
    
    delay(100);
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 128, 0, 128);
    }
    strip.show();
    
    delay(100);
    
  }
  
  if(current_time-button1_press_time > max_control_time && rainbow_mode == true) {
    rainbow_mode = false;
  }
  
  
  
  /*
  if(xbee.available() > 0) {
    Serial << (char)xbee.read();
  }
  */


  if(current_time-last_servo_update >= 10000 && headband_speed_control == false && ears_wiggle_mode == false) {
    
    servo_L.attach(SERVO_L_PIN);
    servo_R.attach(SERVO_R_PIN);
    
    int r = (int)random(0, 2);
    
    if(r == 0) {
    
      for(int i=0; i<30; i++) {
        servo_L.write(SERVO_L_HOME+i);
        servo_R.write(SERVO_R_HOME+i);
        delay(10);
      }
      
      for(int i=30; i>-30; i--) {
        servo_L.write(SERVO_L_HOME+i);
        servo_R.write(SERVO_R_HOME+i);      
        delay(10);
      }
      
      for(int i=-30; i<0; i++) {
        servo_L.write(SERVO_L_HOME+i);
        servo_R.write(SERVO_R_HOME+i);      
        delay(10);
      }
    
    } else if(r == 1) {
      
      for(int i=0; i<30; i++) {
        servo_L.write(SERVO_L_HOME-i);
        servo_R.write(SERVO_R_HOME+i);
        delay(10);
      }
      
      for(int i=30; i>-30; i--) {
        servo_L.write(SERVO_L_HOME-i);
        servo_R.write(SERVO_R_HOME+i);      
        delay(10);
      }
      
      for(int i=-30; i<0; i++) {
        servo_L.write(SERVO_L_HOME-i);
        servo_R.write(SERVO_R_HOME+i);      
        delay(10);
      }
      
    }
    
    servo_L.detach();
    servo_R.detach();


      /*
      delay(100);
      
      if(pos_dir) {
        pos+=20; 
      } else {
        pos-=20;
      }
      
      if(pos <= -20 || pos >= 20) {
        pos_dir = !pos_dir; 
      }
      */
      
    last_servo_update = current_time;  
      
  }
  
  
  
  delay(20);

}


