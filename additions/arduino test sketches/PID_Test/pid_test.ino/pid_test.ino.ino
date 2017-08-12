

#include <PID_v1.h>

//Define Variables we'll be connecting to
double  InputX, SetpointX, OutputX,
        InputY, SetpointY, OutputY,
        Kp, Ki, Kd;

//Specify the links and initial tuning parameters
PID pidX(&InputX, &OutputX, &SetpointX, 2, 5, 1, P_ON_M, DIRECT),
    pidY(&InputY, &OutputY, &SetpointY, 2, 5, 1, P_ON_M, DIRECT);  

/* PID TEST CASES */                       
double testData[][8] = { 
               /* InputX  InputY  SetX  SetY  Kp  Ki  Kd  Testr uns */
/* Test 1: */   { -10.0,   -10.0, 0.0,  0.0,  2,  5,  1,  10 }, /* Bidirectional, positive direction, medium range */
/* Test 2: */   { 10.0,    10.0,  0.0,  0.0,  2,  5,  1,  10 }, /* Bidirectional, negative direction, medium range */
/* Test 3: */   { -2.0,   -2.0,   0.0,  0.0,  2,  5,  1,  10 }, /* Bidirectional, positive direction, short range */
/* Test 4: */   { -20.0, -20.0,   0.0,  0.0,  2,  5,  1,  10 }, /* Bidirectional, positive direction, long range */
/* Test 5: */   { -20.0, -20.0,   0.0,  0.0,  30, 5,  1,  10 }, /* Bidirectional, positive direction, long range, high PID vars */
/* Test 6: */   { -10.0,  -10.0,  0.0,  0.0,  30, 5,  1,  10 } /* Bidirectional, positive direction, medium range, high PID vars */
};

int test = 0;
int testrun = 1;
boolean testing = false;
long lastmicros = 0;


void setup() {
  Serial.begin(9600);
}

void loop() {
  if(testing) {

    /* First run, set vars */
    if(testrun == 1) {
      InputX = testData[test][0];
      InputY = testData[test][1];

      SetpointX = testData[test][2];
      SetpointY = testData[test][3];

      Kp = testData[test][4];
      Ki = testData[test][5];
      Kd = testData[test][6];

      pidX.SetMode(AUTOMATIC);
      pidY.SetMode(AUTOMATIC);
    }

    while(testrun <= testData[test][7]) {
      
      /* Get last microseconds stamp */ 
      lastmicros = micros();

      /* Compute PID Data */
      pid_compute();

      /* Print current progress */
      pid_show_results(test, testrun);

      /* Increase Testrun counter */
      testrun++;

      /* Calculate new input vars for next run */
      pid_calc_input();

      /* Wait for time to fill up 100ms */
      delayMicroseconds(100000 - (micros()-lastmicros));
    }

    /* End testing phase */
    Serial.print(testrun); Serial.print(" testruns of test "); Serial.print(test+1); Serial.print(" finished!");
    Serial.println();Serial.println(); 
    testrun = 1;
    testing = false;
    pidX.SetMode(MANUAL);
    pidY.SetMode(MANUAL);
    
  } else {
    Serial.println("Which test to run?");
  
    while(Serial.available() == 0) {
      delay(500);
    }
  
    test = Serial.parseInt()-1;
    testing = true;
  }
}

void pid_compute() {
  pidX.Compute();
  pidY.Compute();
}

void pid_calc_input() {
  if(InputX > SetpointX) {
    InputX -= (OutputX/25);
  } else {
    InputX += (OutputX/25);
  }

  if(InputY > SetpointY) {
    InputY -= (OutputY/25);
  } else {
    InputY += (OutputY/25);
  }
}

void pid_show_results(int test, int testrun) {
  Serial.print("====== Test Results for test "); Serial.print(test+1); Serial.print(", run "); Serial.print(testrun); Serial.println(" ======");
  Serial.println("====== Input Vars ==========================");
  Serial.print("Input: "); Serial.print(InputX); Serial.print(" "); Serial.println(InputY);
  Serial.print("Setpoint: "); Serial.print(SetpointX); Serial.print(" "); Serial.println(SetpointY);
  Serial.print("Kp/Ki/Kd: "); Serial.print(Kp); Serial.print(" / "); Serial.print(Ki); Serial.print(" / "); Serial.println(Kd);
  Serial.println("====== Output Vars =========================");
  Serial.print("Output: "); Serial.print(OutputX); Serial.print(" "); Serial.println(OutputY);
  Serial.println("");
}

