/* Inclusion of required files */
#include "gyro.h"
#include "pid.h"
#include "motor.h"

#define dt 10 // in ms

/* Object definitions and settings */
Gyro myGyro(dt);
PIDControl pid(1.0,0,0.005,-255,255,dt);
OmniMotorDriver omd;

/* Deadzone definition. With a rotation of less than this value,
 * the robot will stand still
 */
double deadzone = 3.0;

/* Buffers for timing, gyro data and PID outputs */
long lastMillis;
double xAngle, yAngle;
double xOut, yOut;

/* Main setup call 
 * Initializes Serial, the IMU and prepares the motor driver,
 * calbriates the IMU and enables the PID controller
 */
void setup() {
	pinMode(13, OUTPUT);
	Serial.begin(115200);
	if(!myGyro.init()) {

		while(1) {
			digitalWrite(13, HIGH);
			delay(1000);
			digitalWrite(13, LOW);
			delay(2000);
		}
	}

	
	myGyro.calibrate();
	omd.init();
	pid.enable();
}

/* Main loop 
 * The main loop connects all the libraries and classes together,
 * in that it describes the order of function calls that is necessary
 * for the function of the robot
 */
void loop() {
	/* Save the current point in time in lastMillis */
	lastMillis = millis();

	/* Get Data from Gyro */
	myGyro.update(&xAngle, &yAngle);

	/* Compute PID Output with the data */
	pid.compute(xAngle, yAngle, &xOut, &yOut);

	/* Plot Angle on Serial Plotter */
	Serial.print(xAngle);
	Serial.print(" ");
	Serial.print(yAngle);
	Serial.print(" ");
	
	/* Also plot PID outputs */  
	Serial.print(xOut);
	Serial.print(" ");
	Serial.print(yOut);

	/* If any calculated angle is larger than the deadzone */
	if(abs(xAngle) > deadzone || abs(yAngle) > deadzone) {

		/* Actually drive the motors */
		omd.drive(xOut, yOut);

	/* Otherwise, stop them */
	} else omd.stop();
	

	/* Wait until the next dt cycle
	 * if processing took less than dt, wait for the remaining time,
	 * otherwise wait for the next full cycle to start
	 * This is needed for gyro and PID accuracy
	 */
	int pTime = millis() - lastMillis;
	int resttime = (dt > pTime)? (dt - pTime) : (pTime % dt);
	delay(resttime);
}

