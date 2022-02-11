#include <StandardCplusplus.h>
#include <algorithm>
using namespace std;
//
// Edit these
const int filterSamples = 11;
const int medIdx = 5;
const int q1Idx = 2;
const int q3Idx = 8;
int timeBetweenReads = 10;
const int screenXRange = 307.11;
const int screenYRange = 231.09;
const byte cornerPins[] = {2, 3, 4, 5};
const byte sensePin = A0;
int x, y, iqr;
double uFence, lFence;
unsigned long timenow;
void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < 4; i++) {
    pinMode(cornerPins[i], OUTPUT);
  }
  Serial.begin(9600);
}
double xTransform(int n) {
  return n/screenXRange;
}
double yTransform(int n) {
  return n/screenYRange;
}
int* filterOutlier(void (*func)(), char dimType) {
  func();
  int arr[filterSamples];
  // record the samples
  for (int i = 0; i < filterSamples; i++) {
    arr[i] = analogRead(sensePin);
  }
  // get interquartile metrics
  sort(arr, arr + filterSamples);
  iqr = arr[q3Idx] - arr[q1Idx];
  // set upper and lower fence to filter data
  lFence = arr[q1Idx] - 1.5 * iqr;
  uFence = arr[q3Idx] + 1.5 * iqr;
  // filter data
  // put -1 for outlier values
  // edge case 0  5 5 5 5 5 1
  for (int i = 0; i < filterSamples; i++) {
    if (arr[i] > lFence && arr[i] < uFence) {
      arr[i] = -1;
      ///
      /// NON FUNCTIONAL
      /// FOR PRINTING RESULTS ONLY
      //
    } else {
      timenow = millis();
      Serial.print(dimType);
      Serial.print(": " );
      Serial.print(arr[i]);
      Serial.print(" t: ");
      Serial.println(timenow);
    }
  }
  return arr;
}
void setYMode() {
  digitalWrite(cornerPins[0], HIGH);
  digitalWrite(cornerPins[1], LOW);
  digitalWrite(cornerPins[2], HIGH);
  digitalWrite(cornerPins[3], LOW);
}
void setXMode() {
  digitalWrite(cornerPins[0], HIGH);
  digitalWrite(cornerPins[1], HIGH);
  digitalWrite(cornerPins[2], LOW);
  digitalWrite(cornerPins[3], LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
  // Set the left-hand corner pins LOW and the right-hand corner pins HIGH  
  // Set the bottom corner pins LOW and the top corner pins HIGH
  delay(timeBetweenReads);
  int* resX = filterOutlier(&setXMode, 'x');
  delay(timeBetweenReads);
  int* resY = filterOutlier(&setYMode, 'y');
  delay(timeBetweenReads);
  }
