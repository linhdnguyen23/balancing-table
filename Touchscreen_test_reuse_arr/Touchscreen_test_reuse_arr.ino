#define MAX 11
class Deque {
  int  front = -1;
  int  rear = 0;
  int  size = 11;
  int arr[11];
  public:
  Deque(int size) {
    front = -1;
    rear = 0;
    this->size = size;
  }

    // Operations on Deque:
  void  insertFront(int key) {
    // check whether Deque if  full or not
    if (isFull()) {
      return;
    }

    // If queue is initially empty
    if (front == -1) {
      front = 0;
      rear = 0;
    }

    // front is at first position of queue
    else if (front == 0)
      front = size - 1 ;

    else // decrement front end by '1'
      front = front-1;

    // insert current element into Deque
    arr[front] = key ;
  }

  void  insertRear(int key) {
    if (isFull()) {
      return;
    }

    // If queue is initially empty
    if (front == -1) {
      front = 0;
      rear = 0;
    }

    // rear is at last position of queue
    else if (rear == size-1)
      rear = 0;

    // increment rear end by '1'
    else
      rear = rear+1;

    // insert current element into Deque
    arr[rear] = key ;
  }
  void  deleteFront() {
    // check whether Deque is empty or not
    if (isEmpty()) {
      return ;
    }

    // Deque has only one element
    if (front == rear) {
      front = -1;
      rear = -1;
    }
    else {
        // back to initial position
      if (front == size -1) {
        front = 0;
      }
      else { // increment front by '1' to remove current
            // front value from Deque
        front = front+1;
      }
    }
  }

  void  deleteRear() {
    if (isEmpty()) {
      return ;
    }

    // Deque has only one element
    if (front == rear) {
      front = -1;
      rear = -1;
    }
    else if (rear == 0)
      rear = size-1;
    else
      rear = rear-1;
  }
  int* getArr() {
    return arr;
  }
  bool  isFull() {
    return ((front == 0 && rear == size-1) || front == rear+1);
  }
  bool  isEmpty() {
    return (front == -1);
  }
  double  getFront() {
    // check whether Deque is empty or not
    if (isEmpty()) {
        return -1 ;
    }
    return arr[front];
  }
  double  getRear() {
    // check whether Deque is empty or not
    if(isEmpty() || rear < 0) {
      return -1 ;
    }
    return arr[rear];
  }
};


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
Deque dq(filterSamples);
double sum;

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

void swap(int* a, int* b) {
  int t = *a;
  *a = *b;
  *b = t;
}

/* This function takes last element as pivot, places
the pivot element at its correct position in sorted
  array, and places all smaller (smaller than pivot)
to left of pivot and all greater elements to right
of pivot */
int partition (int arr[], int low, int high) {
  int pivot = arr[high]; // pivot
  int i = (low - 1); // Index of smaller element

  for (int j = low; j <= high- 1; j++) {
    // If current element is smaller than or
    // equal to pivot
    if (arr[j] <= pivot) {
      i++; // increment index of smaller element
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return (i + 1);
}

/* The main function that implements QuickSort
arr[] --> Array to be sorted,
low --> Starting index,
high --> Ending index */
void quickSort(int arr[], int low, int high) {
  if (low < high) {
    /* pi is partitioning index, arr[p] is now
    at right place */
    int pi = partition(arr, low, high);

    // Separately sort elements before
    // partition and after partition
    quickSort(arr, low, pi - 1);
    quickSort(arr, pi + 1, high);
  }
}

int filterOutlier(void (*func)()) {
  func();
  delay(timeBetweenReads);
  double oldData;
  double newData;
  bool isInit = false;

  if (dq.isFull()) {
    oldData = dq.getFront();
    dq.deleteFront();
    dq.insertRear(analogRead(sensePin));
  } else {
    // first time running, need to fill up the deque
    while (!dq.isFull()) {
      newData = analogRead(sensePin);
      dq.insertRear(analogRead(newData));
      isInit = true;
    }
  }

  int* arr = dq.getArr();


  // get interquartile metrics
  quickSort(arr, 0, filterSamples - 1);
  iqr = arr[q3Idx] - arr[q1Idx];


  // set upper and lower fence to filter data
  lFence = arr[q1Idx] - 1.5 * iqr;
  uFence = arr[q3Idx] + 1.5 * iqr;

  // filter data
  // put -1 for outlier values
  // edge case 0  5 5 5 5 5 1
  int goodSampleNum = 0;
  if (isInit) {
    for (int i = 0; i < filterSamples; i++) {
        if (arr[i] > lFence && arr[i] < uFence) {
          sum += arr[i];
          goodSampleNum++;
          ///
          /// NON FUNCTIONAL
          /// FOR PRINTING RESULTS ONLY
          //

        } else {
          arr[i] = -1;
        }
      }
  } else {
    sum -= oldData;
    sum += newData;
  }

  return sum/goodSampleNum;
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
  int resX = filterOutlier(&setXMode);
  int resY = filterOutlier(&setYMode);
  
  timenow = millis();
  Serial.print("x: " );
  Serial.print(resX);
  Serial.print(" y: " );
  Serial.print(resY);
  Serial.print(" t: ");
  Serial.println(timenow);
}
