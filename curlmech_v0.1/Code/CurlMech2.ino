#include <Streaming.h>
#include <SoftwareSerial.h>
#include <Servo.h>

struct CurlMech {
  int motor_a;
  int motor_b;
  int led;
  int ldr;
  int reel_in;
  int reel_out;
};

CurlMech yellow = { 8, 10, A1, A0, 200, 10 };
CurlMech green1 = { 11, 12, A3, A2, 200, 10 };
CurlMech purple = { 4, 5, 3, A4, 200, 10 };
CurlMech green2 = { 6, 7, 2, A5, 200, 10 };

int val1 = 0;
int val2 = 0;
int val3 = 0;
int val4 = 0;

long last_dim = 0;
long current_time = 0;
boolean leds_dim = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(yellow.motor_a, OUTPUT);
  pinMode(yellow.motor_b, OUTPUT);
  pinMode(yellow.led, OUTPUT);

  pinMode(green1.motor_a, OUTPUT);
  pinMode(green1.motor_b, OUTPUT);
  pinMode(green1.led, OUTPUT);
  
  pinMode(purple.motor_a, OUTPUT);
  pinMode(purple.motor_b, OUTPUT);
  pinMode(purple.led, OUTPUT);  
  
  pinMode(green2.motor_a, OUTPUT);
  pinMode(green2.motor_b, OUTPUT);
  pinMode(green2.led, OUTPUT);  

  pinMode(yellow.ldr, INPUT);
  pinMode(green1.ldr, INPUT);
  pinMode(purple.ldr, INPUT);
  pinMode(green2.ldr, INPUT);
  
  digitalWrite(yellow.led, HIGH);
  digitalWrite(green1.led, HIGH);
  digitalWrite(purple.led, HIGH);  
  digitalWrite(green2.led, HIGH);
}

void loop() {

  val1 = analogRead(yellow.ldr);
  
  if(val1 < 130) {
    // reel in
    digitalWrite(yellow.motor_a, LOW);
    analogWrite(yellow.motor_b, 128);
  } else {
    // reel out
    digitalWrite(yellow.motor_a, HIGH);
    analogWrite(yellow.motor_b, 128);
  }
  
  
  val2 = analogRead(green1.ldr);
  
  if(val2 < 280) {
    digitalWrite(green1.motor_a, LOW);
    analogWrite(green1.motor_b, 128);
  } else {
    digitalWrite(green1.motor_a, HIGH);
    analogWrite(green1.motor_b, 128);
  }
  
  
  val3 = analogRead(purple.ldr);
  
  if(val3 < 200) {
    digitalWrite(purple.motor_a, LOW);
    analogWrite(purple.motor_b, 128);
  } else {
    digitalWrite(purple.motor_a, HIGH);
    analogWrite(purple.motor_b, 128);
  }
  
  
  val4 = analogRead(green2.ldr);
  
  if(val4 < 20) {
    digitalWrite(green2.motor_a, HIGH);
    analogWrite(green2.motor_b, 128);  
  } else {
    digitalWrite(green2.motor_a, HIGH);
    analogWrite(green2.motor_b, 128);
  }
  
  
  current_time = millis();
  
  if(current_time-last_dim > 3000) {
    if(leds_dim) {
      digitalWrite(yellow.led, HIGH);
      digitalWrite(green1.led, HIGH);
      digitalWrite(purple.led, HIGH);  
      digitalWrite(green2.led, HIGH);
    } else {
      digitalWrite(yellow.led, LOW);
      digitalWrite(green1.led, LOW);
      digitalWrite(purple.led, LOW);  
      digitalWrite(green2.led, LOW);
    }
    leds_dim = !leds_dim;
    last_dim = current_time;
  }
  
  
  delay(500);
  
  digitalWrite(yellow.motor_a, LOW);
  digitalWrite(yellow.motor_b, LOW);  
  digitalWrite(green1.motor_a, LOW);
  digitalWrite(green1.motor_b, LOW);
  digitalWrite(purple.motor_a, LOW);
  digitalWrite(purple.motor_b, LOW);
  digitalWrite(green2.motor_a, LOW);
  digitalWrite(green2.motor_b, LOW);  
  
  Serial << "1: " << analogRead(yellow.ldr);
  delay(20);
  Serial << " 2: " << analogRead(green1.ldr);
  delay(20);
  Serial << " 3: " << analogRead(purple.ldr);
  delay(20);
  Serial << " 4: " << analogRead(green2.ldr) << endl;
  
  delay(100);
    
}



