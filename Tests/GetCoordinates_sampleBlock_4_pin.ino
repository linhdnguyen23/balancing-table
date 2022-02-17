/**
 Touch Screen Testing Code WIRE 
 Purpose: to test various methods of data receiving from the touch panel 
 */

// CONSTANTS
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

  //FIRST XY COORDINATE RECEIVER 
  getCoordinates();


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
// getCoordinates takes in the empty average x and y values (empty) then returns them with the current average x and y 
// does not use rolling window technique 
float getCoordinates(){
  int delayValue = 1;
  int delayXY= 1;  
  int sumX=0;
  int sumY=0;  
  int sampleSize =11;

  pinMode(Y1,INPUT);
  pinMode(Y2,INPUT);  
  digitalWrite(Y2,LOW);
  pinMode(X1,OUTPUT);
  digitalWrite(X1,HIGH);
  pinMode(X2,OUTPUT);
  digitalWrite(X2,LOW);
  
  delay(delayValue);
  for(int i = 0; i < sampleSize; ++i){
    x[i] = analogRead(Y1);
    sumX= sumX+ x[i];
  }

// calculate average samples 
  xVal= sumX/sampleSize; 
  delay(delayXY); 
  pinMode(X1,INPUT);
  pinMode(X2,INPUT);
  digitalWrite(X2,LOW);
  pinMode(Y1,OUTPUT);
  digitalWrite(Y1,HIGH);
  pinMode(Y2,OUTPUT);
  digitalWrite(Y2,LOW);
  delay(delayValue);
  for(int i = 0; i < sampleSize; ++i){
      // Set the bottom corner pins LOW and the top corner pins HIGH
  // Read the sensor value in the y gradient
    y[i] = analogRead(X1);
    sumY=sumY + y[i];
  }
  // calculate average of samples 
  yVal= sumY/sampleSize; 
  

 // return averageY, averageX; 

    
}
