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

unsigned long previousTime =0; 
float errorX=0; 
float errorY=0;
float lastError=0; 
int SetPoint =0; 
int setpointX = 555;
int setpointY = 555;
int screen_upper=750;
int screen_lower=250;
        
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
	float PID_total=0;
  float PID_totaly=0;
  float x_upper=305;
  float x_lower= -195;
  float y_upper= 305;
  float y_lower= -195; 

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

        SetPoint = setpointX;
        // Calculate the error in X direction
        errorX = PIDerror(x);

        Serial.print(" PID: ");
        Serial.print(errorX);

        x_lower= setpointX - screen_upper;
        x_upper= setpointX- screen_lower;

        // SET POINT RANGE VARIABILITY X
		PID_total = map(errorX, x_upper, x_lower, 5, 130); // 150 to 150 plus minus error 
		// 0 to 150 is motor range 
		myservo.write(PID_total);
        //delay(10);   // this was in example code so maybe it's not needed or we can do it differently
        // need to add code for the output from the PID being fed to the motors
        y_lower= setpointY - screen_upper;
        y_upper= setpointY- screen_lower;		
		

        SetPoint = setpointY;
        // Calculate the error in Y direction
        //errorY = PIDerror(y);
        //PID_totaly= map(errorY, y_upper, y_lower, 5, 130);
        //myservoy.write(PID_totaly); // 250 to -250 may or may not be the range for the set point 
        //delay(10);   // this was in example code so maybe it's not needed or we can do it differently
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
  int upperLim=800;
  int lowerLim= 20; // change these based on motor orientation and corresponding encoder value 
  
  elapsedTime = (currentTime - previousTime); 

  currentError = SetPoint - inp;   // current error
  cumError += currentError ;   //integral - sum all errors 
  rateError = (currentError - lastError)/elapsedTime;   //derivative

        Serial.print(" current error: ");
        Serial.print(currentError);
        Serial.print("  rateError: ");
         Serial.println(rateError);
  
  
  out = kp*currentError + ki*cumError + kd*rateError;   // PID Output
  
  
  //if(out < lowerLim){out = lowerLim;}
  //if(out > upperLim) {out = upperLim; }


  // make a max error 

  lastError = currentError;   //store error for next iteration
  previousTime = currentTime;   //store time for next iteration

  return out;   // function returns PID output
}
