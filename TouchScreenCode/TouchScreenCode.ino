#include <PID_v1.h>

/**
 * Resistive Touchscreen Example
 * (c) 2019 Playful Technology
 */
#include "calibrate.h"
#include<Servo.h>

// CONSTANTS
// UR, LR, UL, LL corner pins
const byte cornerPins[] = {2, 3, 4, 5};
// Central sensor pin
const byte sensePin = A0;
const byte servo1Pin = 6;
const byte servo2Pin = 7;

// GLOBALS
// PID values
double Setpoint, Input, Output; //for X
double Setpoint1, Input1, Output1; //for Y
Servo servo1; //X axis
Servo servo2; //Y axis

//PID const
float Kp = 0.3;                                                     
float Ki = 0.03;                                                      
float Kd = 0.13;

float Kp1 = 0.3;                                                       
float Ki1 = 0.08;                                                      
float Kd1 = 0.13;
long cas=0; 
//INIT PID
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
PID myPID1(&Input1, &Output1, &Setpoint1,Kp1,Ki1,Kd1, DIRECT);
int Ts = 50;
unsigned long Stable = 0; 
unsigned int noTouchCount = 0;

// Last sensed values
int x, y, count;
POINT* calibPts = (POINT*)malloc(sizeof(POINT) * 3);
// set desired screen coordinates corresponding to the 3 calibration points
// TODO: set these points later
POINT screenPts[] =     {
    [0] = { x = 0L, y = 0L },
    [1] = { x = 100L, y = 50L  },
    [2] = { x = -50L, y = 75L  }
    };
MATRIX *matrixPtr = new MATRIX();
//malloc(sizeof(MATRIX));

int setCalibrationMatrix(POINT * displayPtr, POINT * screenPtr, MATRIX * matrixPtr) {

  int  retValue = OK ;



  matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                       ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));

  if (matrixPtr->Divider == 0) {
      retValue = NOT_OK ;
  } else {
    matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));
    Serial.println("A: " + matrixPtr->An);
    matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                    ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x));

    matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y;

    matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y));

    matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                    ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x));

    matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                    (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                    (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y;
  }

  return retValue;

} /* end of setCalibrationMatrix() */



/**********************************************************************
 *
 *     Function: getDisplayPoint()
 *
 *  Description: Given a valid set of calibration factors and a point
 *                value reported by the touch screen, this function
 *                calculates and returns the true (or closest to true)
 *                display point below the spot where the touch screen
 *                was touched.
 *
 *
 *
 *  Argument(s): displayPtr (output) - Pointer to the calculated
 *                                      (true) display point.
 *               screenPtr (input) - Pointer to the reported touch
 *                                    screen point.
 *               matrixPtr (input) - Pointer to calibration factors
 *                                    matrix previously calculated
 *                                    from a call to
 *                                    setCalibrationMatrix()
 *
 *
 *  The function simply solves for Xd and Yd by implementing the
 *   computations required by the translation matrix.
 *
 *                                              /-     -\
 *              /-    -\     /-            -\   |       |
 *              |      |     |              |   |   Xs  |
 *              |  Xd  |     | A    B    C  |   |       |
 *              |      |  =  |              | * |   Ys  |
 *              |  Yd  |     | D    E    F  |   |       |
 *              |      |     |              |   |   1   |
 *              \-    -/     \-            -/   |       |
 *                                              \-     -/
 *
 *  It must be kept brief to avoid consuming CPU cycles.
 *
 *
 *       Return: OK - the display point was correctly calculated
 *                     and its value is in the output argument.
 *               NOT_OK - an error was detected and the function
 *                         failed to return a valid point.
 *
 *
 *
 *                 NOTE!    NOTE!    NOTE!
 *
 *  setCalibrationMatrix() and getDisplayPoint() will do fine
 *  for you as they are, provided that your digitizer
 *  resolution does not exceed 10 bits (1024 values).  Higher
 *  resolutions may cause the integer operations to overflow
 *  and return incorrect values.  If you wish to use these
 *  functions with digitizer resolutions of 12 bits (4096
 *  values) you will either have to a) use 64-bit signed
 *  integer variables and math, or b) judiciously modify the
 *  operations to scale results by a factor of 2 or even 4.
 *
 *
 */
int getDisplayPoint(POINT * displayPtr, POINT * screenPtr, MATRIX * matrixPtr) {
  int  retValue = OK ;


  if (matrixPtr->Divider != 0) {

    /* Operation order is important since we are doing integer */
    /*  math. Make sure you add all terms together before      */
    /*  dividing, so that the remainder is not rounded off     */
    /*  prematurely.                                           */

    displayPtr->x = ((matrixPtr->An * screenPtr->x) +
                      (matrixPtr->Bn * screenPtr->y) +
                      matrixPtr->Cn
                    ) / matrixPtr->Divider;

    displayPtr->y = ((matrixPtr->Dn * screenPtr->x) +
                      (matrixPtr->En * screenPtr->y) +
                      matrixPtr->Fn
                    ) / matrixPtr->Divider;
  } else {
    retValue = NOT_OK ;
  }

  return retValue;
} /* end of getDisplayPoint() */

void populatePointArr(POINT* pts, int idx) {
  setReadXMode();
  pts[idx].x = analogRead(sensePin);
  setReadYMode();
  pts[idx].y = analogRead(sensePin);
}

void setup() {
  servo1.attach(5);
  servo2.attach(6);
  Output=95;
  Output1=95;
  servo1.write(Output);
  servo2.write(Output1);
  
  //Zapnutie PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(20, 160);
  myPID1.SetMode(AUTOMATIC);
  myPID1.SetOutputLimits(20, 160);
  // Configure corner pins as outputs
  for(int i=0; i<4; i++){
    pinMode(cornerPins[i], OUTPUT);
  }
  // Start serial connection to monitor output
  Serial.begin(9600);
  Serial.print("Begin calibration.");
  char buffer[50];
  for (int i = 0; i < 3; i++) {
//    setStandbyMode();
    sprintf(buffer, "Please press and hold the calibration point #",i + 1);
    Serial.println(buffer);
    while (Serial.available() == 0) {
      // wait for user input
    }
    String ans = Serial.readString();
    delay(1000);
    Serial.print(">");
    Serial.print(ans);
    delay(1000);
    if (ans.equals("y")) {
      populatePointArr(calibPts, i);
      Serial.println("");
      Serial.print(calibPts[i].x);
      Serial.print(", ");
      Serial.println(calibPts[i].y);
    } else {
      Serial.println("");
      Serial.println("Are you pressing on the screen? y/n");
    }  
  }
  setCalibrationMatrix(calibPts, screenPts, matrixPtr);
  Serial.print("matrix: ");
  sprintf(buffer, "%ld, ", matrixPtr->An);
  Serial.print(buffer);
  sprintf(buffer, "%ld, ", matrixPtr->Bn);
  Serial.print(buffer);
  sprintf(buffer, "%ld, ", matrixPtr->Cn);
  Serial.print(buffer);
  sprintf(buffer, "%ld, ", matrixPtr->Dn);
  Serial.print(buffer);
  sprintf(buffer, "%ld, ", matrixPtr->En);
  Serial.print(buffer);
  sprintf(buffer, "%ld, ", matrixPtr->Fn);
  Serial.print(buffer);
  sprintf(buffer, "%ld, ", matrixPtr->Divider);
  Serial.print(buffer);
}

void setStandbyMode() {
  digitalWrite(cornerPins[0], LOW);
  digitalWrite(cornerPins[1], HIGH);
  digitalWrite(cornerPins[2], HIGH);
  digitalWrite(cornerPins[3], HIGH);
  delay(1);
}

void setReadXMode() {
  // Set the left-hand corner pins LOW and the right-hand corner pins HIGH
  digitalWrite(cornerPins[0], HIGH);
  digitalWrite(cornerPins[1], HIGH);
  digitalWrite(cornerPins[2], LOW);
  digitalWrite(cornerPins[3], LOW);
  // Read the sensor value in the x gradient
  delay(1);
}

void setReadYMode() {
  // Set the bottom corner pins LOW and the top corner pins HIGH
  digitalWrite(cornerPins[0], HIGH);
  digitalWrite(cornerPins[1], LOW);
  digitalWrite(cornerPins[2], HIGH);
  digitalWrite(cornerPins[3], LOW);
  // Read the sensor value in the y gradient
  delay(1);
}
void loop() {
  
  // Display the co-ordinate value obtained
struct POINT *displayPtPtr = new POINT();
Serial.print("X:");
Serial.print(x);
Serial.print(",");
Serial.print("Y:");
Serial.println(y);
Serial.print("displayX:");
Serial.print(displayPtPtr->x);
Serial.print(",displayY:");
Serial.println(displayPtPtr->y);
Serial.print("Time");
Serial.print(count);
  
	setReadXMode();
	x = analogRead(sensePin);
	// resolution?

	setReadYMode();
	y = analogRead(sensePin);

	servo1.attach(servo1Pin); //connect servos
	servo2.attach(servo2Pin); 
	noTouchCount = 0;  
	POINT screenPt = {x, y};
	getDisplayPoint(displayPtPtr, &screenPt, matrixPtr);
	Input = displayPtPtr->x;  // read and convert X coordinate
	Input1 = displayPtPtr->y; // read and convert Y coordinate
  
	if((Input>Setpoint-2 && Input<Setpoint+2 && Input1>Setpoint1-2 && Input1<Setpoint1+2)) {//if ball is close to setpoint
		Stable=Stable+1; //increment STABLE
		digitalWrite(9,HIGH); // led would be on to indicate not stablized
	}
	else {
		digitalWrite(9,LOW);
	}
	myPID.Compute();  //action control X compute
	myPID1.Compute(); //   action control  Y compute   
	servo1.write(Output);//control
	servo2.write(Output1);//control 
	Serial.print(Setpoint);   Serial.print(",");  Serial.print(Setpoint1);  Serial.print(",");  Serial.print(Input);Serial.print(","); Serial.println(Input1); 

}

