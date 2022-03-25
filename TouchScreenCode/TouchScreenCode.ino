/**
 * Resistive Touchscreen Example
 * (c) 2019 Playful Technology
 */
#include "calibrate.h"

// CONSTANTS
// UR, LR, UL, LL corner pins
const byte cornerPins[] = {2, 3, 4, 5};
// Central sensor pin
const byte sensePin = A0;

// GLOBALS
// Last sensed values
int x, y, count;
POINT* calibPts = (POINT*)malloc(sizeof(POINT) * 3);
// set desired screen coordinates corresponding to the 3 calibration points
// TODO: set these points later
POINT screenPts[] =     {
    [0] = { x = 0, y = 0 },
    [1] = { x = 100, y = 50  },
    [2] = { x = -50, y = 75  }
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
    Serial.print(buffer);
    while (Serial.available() == 0) {
      // wait for user input
    }
    String ans = Serial.readString();
    delay(1000);
    Serial.print(ans);
    delay(1000);
    if (ans.equals("y\n")) {
      populatePointArr(calibPts, i);
      Serial.println("");
      Serial.print(calibPts[i].x);
      Serial.println(calibPts[i].y);
    } else {
      Serial.print("Are you pressing on the screen? y/n");
    }  
  }
  setCalibrationMatrix(calibPts, screenPts, matrixPtr);
  Serial.print("matrix: ");
  Serial.print(matrixPtr->An + ", ");
  Serial.print(matrixPtr->Bn + ", ");
  Serial.print(matrixPtr->Cn + ", ");
  Serial.print(matrixPtr->Dn + ", ");
  Serial.print(matrixPtr->En + ", ");
  Serial.print(matrixPtr->Fn + ", ");
  Serial.print(matrixPtr->Divider);
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
  setReadXMode();
	x = analogRead(sensePin);
	// resolution?

  setReadYMode();
  y = analogRead(sensePin);
  
  count=millis();
  
  // Display the co-ordinate value obtained
  Serial.print("X:");
  Serial.print(x);
  Serial.print(",");
  Serial.print("Y:");
  Serial.println(y);
  Serial.print("Time");
  Serial.print(count);
}

