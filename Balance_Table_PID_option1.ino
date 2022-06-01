#include <stdio.h>
    // Variables
const byte cornerPins[] = {2, 3, 4, 5};
// Central sensor pin
const byte sensePin = A0;

float kp = 1;
float ki = 1;// I think this value should be in between 0.1 and 0.3 
float kd = 1;

unsigned long previousTime =0; 
float errorX=0; 
float errorY=0;
float lastError; 
int SetPoint =0; 
int setpointX = 555;
int setpointY = 580;
        
int readDelay = 1;

float x;
float y;
float output;

void setup() {
  for(int i=0; i<4; i++){
    pinMode(cornerPins[i], OUTPUT);
  }
  // Start serial connection to monitor output
  Serial.begin(9600);
}

void loop() {
        // Set-up touch screen and get X coordinate
        digitalWrite(cornerPins[0], HIGH);
        digitalWrite(cornerPins[1], HIGH);
        digitalWrite(cornerPins[2], LOW);
        digitalWrite(cornerPins[3], LOW); 
        delay(readDelay);

        x = analogRead(sensePin);

        // Set-up touch screen and get Y coordinate
        digitalWrite(cornerPins[0], HIGH);
        digitalWrite(cornerPins[1], LOW);
        digitalWrite(cornerPins[2], HIGH);  
        digitalWrite(cornerPins[3], LOW);
        delay(readDelay);

        y = analogRead(sensePin);

        SetPoint = setpointX;
        // Calculate the error in X direction
        errorX = PIDerror(x);
        delay(100);   // this was in example code so maybe it's not needed or we can do it differently
        // need to add code for the output from the PID being fed to the motors

        SetPoint = setpointY;
        // Calculate the error in Y direction
        errorY = PIDerror(y);
        delay(100);   // this was in example code so maybe it's not needed or we can do it differently
        // need to add code for the output from the PID being fed to the motors 
}


float PIDerror(float inp){
  
  // Local function variables 
  unsigned long currentTime = millis();
  int elapsedTime;
  float currentError;
  float cumError;
  float rateError;
  float out; 
  
  elapsedTime = (currentTime - previousTime); 

  currentError = SetPoint - inp;   // current error
  cumError = currentError * elapsedTime;   //integral 
  rateError = (currentError - lastError)/elapsedTime;   //derivative

  out = kp*currentError + ki*cumError + kd*rateError;   // PID Output

  lastError = currentError;   //store error for next iteration
  previousTime = currentTime;   //store time for next iteration

  return out;   // function returns PID output
}
