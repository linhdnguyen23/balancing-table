/**
 Touch Screen Testing Code WIRE 
 Purpose: to test various methods of data receiving from the touch panel 
 */

// CONSTANTS
// UR, LR, UL, LL corner pins
#include <stdio.h>
#define A0 X1
#define A1 X2
#define A2 Y1
#define A3 Y2

// GLOBALS
// Last sensed values
int x[11]={};// should initialize at 0 
int y[11]={};// should initialize at 0 
float xVal, yVal; 

void setup() {
  // Start serial connection to monitor output
  Serial.begin(9600);
}


void loop() {
  long unsigned int now =millis(); 
  // SECOND XY COORDINATE RECEIVER- ROLLING WINDOW WITH 11 X POINTS 11 Y POINTS 
  rollingWindow();
  

  Serial.print("x: ");
  Serial.print(xVal);
  Serial.print(", y : ");
  Serial.print(yVal);
  Serial.print(" time: ");
  Serial.print( now);
  Serial.print("\n");

  Serial.print("x array: ");

  for (int i = 0; i < 10; ++i){
    Serial.print(x[i]);
    Serial.print(" , ");
  }
  Serial.print("\n");

  Serial.print("y array: ");

  for (int i = 0; i < 10; ++i){
    Serial.print(y[i]);
    Serial.print(" , ");
  }
  Serial.print("\n");
  
}

// rolling window array (sliding window) with x,y,x,y,x,y.... 
float rollingWindow(){
  int xTemp, yTemp; 
  int sizeX, sizeY =0 ; 
  int delayValue = 1;
  int delayXY= 1;  
  int sumX=0;
  int sumY=0;  
  int sampleSize =11; 
  int arrayLocY=0; 
  int arrayLocX=0; 

  pinMode(Y1,INPUT);
  pinMode(Y2,INPUT);  
  digitalWrite(Y2,LOW);
  pinMode(X1,OUTPUT);
  digitalWrite(X1,HIGH);
  pinMode(X2,OUTPUT);
  digitalWrite(X2,LOW);
  // Read the sensor value in the x gradient
  delay(delayValue);
  
  if (sizeX < 10) {
    for (int i = 0; i < sampleSize; ++i){
      x[i] = analogRead(Y1);
      sumX= sumX + x[i]; 
    }
    sizeX=11; 
  }
  else {
    xTemp = analogRead(Y1);
    sumX= sumX- x[arrayLocX] + xTemp;
    x[arrayLocX]= xTemp;
    if (arrayLocX<10) {
      arrayLocX=arrayLocX+1;
    }
    else{
      arrayLocX=0; 
    }
  }
  xVal= sumX/sampleSize;  

  pinMode(X1,INPUT);
  pinMode(X2,INPUT);
  digitalWrite(X2,LOW);
  pinMode(Y1,OUTPUT);
  digitalWrite(Y1,HIGH);
  pinMode(Y2,OUTPUT);
  digitalWrite(Y2,LOW);
  // Read the sensor value in the x gradient
  delay(delayValue);

  if (sizeY < 10) {
    for (int i = 0; i < sampleSize; ++i){
      y[i] = analogRead(X1);
      sumY= sumY + y[i]; 
    }
    sizeY=11; 
  }
  else {
    yTemp = analogRead(X1);
    sumY= sumY- y[arrayLocY] + yTemp;
    y[arrayLocY]= yTemp;
    if (arrayLocY<10) {
      arrayLocY=arrayLocY+1;
    }
    else{
      arrayLocY=0; 
    }
  }
  yVal= sumY/sampleSize;                                                                                                                                                                                                                                                                                                                                                                                                                      
// for sliding window type array, I think there needs to be two arrays, initial array and final array with the added point from the sensor 
// so there will be 11 new points every "batch" of readings... and then a gap for readings. This means an inconsistent sampling rate- im unsure if thats a problem 
}


