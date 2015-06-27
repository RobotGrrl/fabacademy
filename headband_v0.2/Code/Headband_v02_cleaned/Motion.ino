void initMotion() {
  x_home = analogRead(X_PIN);
  y_home = analogRead(Y_PIN);
  z_home = analogRead(Z_PIN);  
  
  if (!gyro.begin(gyro.L3DS20_RANGE_250DPS)) {
    Serial.println("Oops ... unable to initialize the L3GD20. Check your wiring!");
    while (1);
  }
  
}


void updateGyro() {
  gyro.read();

  if(DEBUG) {  
    Serial.print("X: "); Serial.print((int)gyro.data.x);   Serial.print(" ");
    Serial.print("Y: "); Serial.print((int)gyro.data.y);   Serial.print(" ");
    Serial.print("Z: "); Serial.println((int)gyro.data.z); Serial.print(" ");
  } 
}


void checkGyroBounds() {
  if(gyro.data.z < -100) z_val = -100;
  if(gyro.data.z > 100) z_val = 100;
  
  if(gyro.data.x < -100) x_val = -100;
  if(gyro.data.x > 100) x_val = 100; 
}


void detectMotionZ() {
  if(gyro.data.z > 40) {
    if(DEBUG) Serial << "forwards!" << endl;
    
    xbee_promulgate.transmit_action('#', 'F', MOTOR_SPEED, 500, '!');
    
    moveServos(-90, 90, 250);
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 100, 255, 10);
    }
    strip.show();
    
    last_gyro_trig = current_time;
      
  } else if(gyro.data.z < -40) {
    if(DEBUG) Serial << "backwards!" << endl;
    
    xbee_promulgate.transmit_action('#', 'B', MOTOR_SPEED, 500, '!');
    
    moveServos(60, -60, 250);
    
    for(int i=0; i<N_LEDS; i++) {
      strip.setPixelColor(i, 10, 50, 200);
    }
    strip.show();
    
    last_gyro_trig = current_time;
      
  } 
}


void detectMotionX() {
 
  if(gyro.data.x > 40) {
    if(DEBUG) Serial << "left!" << endl;
    
    xbee_promulgate.transmit_action('#', 'L', MOTOR_SPEED, 500, '!');
    
    moveServos(60, 60, 250);
    
    for(int i=0; i<N_LEDS; i++) {
    strip.setPixelColor(i, 200, 200, 200);
    }
    strip.setPixelColor(1, 10, 255, 100);
    strip.setPixelColor(2, 10, 255, 100);
    strip.setPixelColor(3, 10, 255, 100);
    strip.show();
    
    last_gyro_trig = current_time;
      
  } else if(gyro.data.x < -40) {
    if(DEBUG) Serial << "right!" << endl;
    
    xbee_promulgate.transmit_action('#', 'R', MOTOR_SPEED, 500, '!');
    
    moveServos(-60, -60, 250);
    
    for(int i=0; i<N_LEDS; i++) {
    strip.setPixelColor(i, 200, 200, 200);
    }
    strip.setPixelColor(3, 10, 255, 100);
    strip.setPixelColor(4, 10, 255, 100);
    strip.setPixelColor(5, 10, 255, 100);
    strip.show();
    
    last_gyro_trig = current_time;
      
  }
  
}


void moveServos(int l_val, int r_val, int d) {
  servo_L.attach(SERVO_L_PIN);
  servo_R.attach(SERVO_R_PIN);
  
  servo_L.write(SERVO_L_HOME+l_val);
  servo_R.write(SERVO_R_HOME+r_val);
  delay(d);
  
  servo_L.detach();
  servo_R.detach();
}


