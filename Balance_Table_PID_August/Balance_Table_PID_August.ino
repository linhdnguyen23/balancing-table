#include <stdio.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
Servo myservoy; //servo object to control servo y 

    // Variables
const byte cornerPins[] = {2, 3, 4, 5};
// Central sensor pin
const byte sensePin = A0;



float kp = 0.5;
float ki = 0;// I think this value should be in between 0.1 and 0.3 
float kd = 1;

unsigned long previousTimeX = 0;
unsigned long previousTimeY = 0;
float errorX = 0; 
float errorY = 0;
float lastErrorX =0;
float lastErrorY =0;
float totalErrorX =0;
float totalErrorY =0; 
int SetPoint =0; 
int setpointX = 500;
int setpointY = 500;
int screenUpper = 750;
int screenLower = 250;
        
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
  myservo.attach(6);
  myservoy.attach(7);
}

void loop() {
  float pidTotalX=0;
  float pidTotalY=0;
  float xUpper=305;
  float xLower= -195;
  float yUpper= 305;
  float yLower= -195; 

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


  // Display the co-ordinate value obtained
  Serial.print(" x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.println(y);

  SetPoint = setpointX; // 
  // Calculate the error in X direction
  errorX = PIDerror(x, lastErrorX, totalErrorX, previousTimeX);

  Serial.print(" PID: ");
  Serial.print(errorX);
  // ?: how do we determine the range of errorX with these equations
  xLower = setpointX - screenUpper; // TODO: 500 - 750
  xUpper = setpointX - screenLower; // TODO: 500 - 250

  // SET POINT RANGE VARIABILITY X
  pidTotalX = map(errorX, xUpper, xLower, 5, 130); // 150 to 150 plus minus error 
  // 0 to 150 is motor range 
  myservo.write(pidTotalX);
  //delay(10);   // this was in example code so maybe it's not needed or we can do it differently
  // need to add code for the output from the PID being fed to the motors
  yLower= setpointY - screenUpper; // TODO: 500 - 750
  yUpper= setpointY- screenLower; // TODO: 500 - 250	


  SetPoint = setpointY;
  // Calculate the error in Y direction
  errorY = PIDerror(y, lastErrorY, totalErrorY, previousTimeY);
  pidTotalY= map(errorY, yUpper, yLower, 5, 130);
  myservoy.write(pidTotalY); // 250 to -250 may or may not be the range for the set point 
  //delay(10);   // this was in example code so maybe it's not needed or we can do it differently
  // need to add code for the output from the PID being fed to the motors 
}


float PIDerror(float inp, float& lastError, float& totalError, unsigned long& previousTime){
  // TODO: logic error cumError is a local variable, it should be global to be able to cumulate the values. We would need to have cumErrorX and cumErrorY
  
  // Local function variables 
  unsigned long currentTime = millis();
  int elapsedTime;
  float currentError;
  float rateError;
  float out; 
  int upperLim=800;
  int lowerLim= 20; // change these based on motor orientation and corresponding encoder value 
  
  elapsedTime = (currentTime - previousTime); 

  currentError = SetPoint - inp;   // current error
  totalError += currentError ;   // integral - sum all errors 
  rateError = (currentError - lastError) / elapsedTime;   // derivative

  Serial.print(" current error: ");
  Serial.print(currentError);
  Serial.print("  rateError: ");
  Serial.println(rateError);
  
  
  out = kp * currentError + ki * totalError + kd * rateError;   // PID Output
  
  
  //if(out < lowerLim){out = lowerLim;}
  //if(out > upperLim) {out = upperLim; }


  // make a max error 

  lastError = currentError;   //store error for next iteration
  previousTime = currentTime;   //store time for next iteration

  return out;   // function returns PID output
}
