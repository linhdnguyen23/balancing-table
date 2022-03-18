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
POINT *calibPts = new POINT[3];

int setCalibrationMatrix( POINT * displayPtr,
                          POINT * screenPtr,
                          MATRIX * matrixPtr) {

    int  retValue = OK ;



    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                         ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

    if ( matrixPtr->Divider == 0 )
    {
        retValue = NOT_OK ;
    }
    else
    {
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                        ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

        matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                        ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                        ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;

        matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                        ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;

        matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                        (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                        (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
    }

    return( retValue ) ;

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
int getDisplayPoint( POINT * displayPtr,
                     POINT * screenPtr,
                     MATRIX * matrixPtr ) {
    int  retValue = OK ;


    if ( matrixPtr->Divider != 0 )
    {

        /* Operation order is important since we are doing integer */
        /*  math. Make sure you add all terms together before      */
        /*  dividing, so that the remainder is not rounded off     */
        /*  prematurely.                                           */

        displayPtr->x = ( (matrixPtr->An * screenPtr->x) +
                          (matrixPtr->Bn * screenPtr->y) +
                          matrixPtr->Cn
                        ) / matrixPtr->Divider ;

        displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) +
                          (matrixPtr->En * screenPtr->y) +
                          matrixPtr->Fn
                        ) / matrixPtr->Divider ;
    }
    else
    {
        retValue = NOT_OK ;
    }

    return( retValue ) ;

} /* end of getDisplayPoint() */

void waitForPoint(POINT* pts, int idx) {
  
}

void setup() {
	// Configure corner pins as outputs
	for(int i=0; i<4; i++){
		pinMode(cornerPins[i], OUTPUT);
	}
	// Start serial connection to monitor output
	Serial.begin(9600);

	Serial.print("Enter 1st point:");
	waitForPoint(calibPts, 0);
	Serial.print("Enter 2nd point:");
	waitForPoint(calibPts, 1);
	Serial.print("Enter 3rd point:");
	waitForPoint(calibPts, 2);
  
}



void loop() {
	// Set the left-hand corner pins LOW and the right-hand corner pins HIGH
	digitalWrite(cornerPins[0], HIGH);
	digitalWrite(cornerPins[1], HIGH);
	digitalWrite(cornerPins[2], LOW);
	digitalWrite(cornerPins[3], LOW);
	// Read the sensor value in the x gradient
	delay(1);
	x = analogRead(sensePin);
	// resolution? 

	// Set the bottom corner pins LOW and the top corner pins HIGH
	digitalWrite(cornerPins[0], HIGH);
	digitalWrite(cornerPins[1], LOW);
	digitalWrite(cornerPins[2], HIGH);
	digitalWrite(cornerPins[3], LOW);
	// Read the sensor value in the y gradient
	delay(1);
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
