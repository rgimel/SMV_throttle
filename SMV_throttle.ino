/*
 * Code that converts a 0.8~4.2V output from throttle
 * to PWM signal that is sent to powertrain 
 * (Presumably their ESC or motor controller)
 * 
 * We are using the built-in Servo library because 
 * both a Servo and a brushless ESC uses the same
 * 50hz PWM signal. 
 * 
 * © 2020 SMV Electronics - BruinRacing
 */


#include <Servo.h>      // Servo library to generate PWM signal

#define throttleIn 5   // Placeholders
#define ESCOutput 3
#define button 7

const int minPulseWidth = 1000; // microseconds
const int maxPulseWidth = 2000;
const int fullThrottle = 5;//placeholder, amount of time to get to full throttle
//have to define pin for button 

const bool isButton = false;

Servo ESC;

void setup() {
  // Initialisation of ESC
  ESC.attach(ESCOutput, minPulseWidth, maxPulseWidth);
}

const double throttleMinVoltage = 0.8;
const double throttleMaxVoltage = 4.2;

bool isPressed = false;
int pressStart;


int mappingFunction(int f, int s){
  //linear
  if(0){
    return min(s/fullThrottle, 1) * 1023;
  }
  //exponential
  if(1){
  }
  //logarithmic
  if(2){
  }
}

int readThrottle() {
  /*
   * Reads the throttle input from analog signal,
   * Returns 0~1023
   */
  if(!isButton){
    int throttleReading = analogRead(throttleIn);
    int throttleMin = throttleMinVoltage * 1023, throttleMax = throttleMaxVoltage * 1023;
    // Throttle cutoff threshold
    if (throttleReading < throttleMin) {
      return 0;
    } else if (throttleReading < throttleMax) {
      return map(throttleReading, throttleMin, throttleMax, 0, 1023);
    } else {
      return 1023;
    }
  }
  else{
    if(button == HIGH){
      if(!isPressed){
        pressStart = millis();
      }
      int timePressed = millis() - pressStart;
      //using a linear mapping function(for now)
      return mappingFunction(0, timePressed);
    }
    else{
      isPressed = false;
      return 0;
    }
  }
}


void writeToESC(int throttleRead) { 
  ESC.write(map(throttleRead, 0, 1023, 0, 180));
}

int lastMotorWrite = 0;

void throttleRamping(int throttleValue) {
  for (int i = lastMotorWrite; i <= throttleValue; i++) {
    writeToESC(i);
    // Serial.println(i);
    delay(map(i, 0, 1023, 30, 1));
  }
}


void loop() {
  // Standard Arduino has 10-bit output
  int throttleRead = readThrottle();

  if (throttleRead > lastMotorWrite) {
    throttleRamping(throttleRead);
  } else {
    writeToESC(throttleRead);
  }
  lastMotorWrite = throttleRead;
}
