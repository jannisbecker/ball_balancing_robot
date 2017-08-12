#include <Arduino.h>

#include <Wire.h>
#include <LSM6.h>
#include <PID_v1.h>
#include <stdlib.h>

#include "gyro.h"

/* Gyro Object constructor
 * 	dt: Delta Time, describes how frequent the IMU retrieves data,
 * 		and is also needed for the complementary filter
 */
Gyro::Gyro(double dt) {
	deltaTime = dt;
}

/* Additional init method
 * Initializes the Gyro unit via a library call, thus changing registers
 * in the IMU to set the neccessary settings.
 */
bool Gyro::init() {
	Wire.begin();

	if (!imu.init(imu.device_DS33, imu.sa0_auto))
	{
		return false;
	}
	imu.enableDefault();
	return true;
}

/* Read new values, transform them and save them in *xAngle and *yAngle 
 * Also subtract the calibration values to zero out these new values 
 */
void Gyro::update(double *xAngle, double *yAngle) { 
	read();
	complementaryFilter();

	/* Make adjustments so that xAngle is the leftward rotation around the X axis
	 * and yAngle is the rightward rotation around the Y-axis. Both positive rotations
	 * should point towards the first quadrant in the 2D coordinate space
	 */
	*xAngle = pitch - zeroPitch;
	*yAngle = -roll + zeroRoll;

}


/* Retrieve new, raw data from the gyro */
void Gyro::read() {
	imu.read();

	/* Get raw acc data */
	accData[0] = imu.a.x;
	accData[1] = imu.a.y;
	accData[2] = imu.a.z;

	/* Get raw gyro data */
	gyroData[0] = imu.g.x;
	gyroData[1] = imu.g.y;
	gyroData[2] = imu.g.z;
}

/* Apply a complementary filter to merge data into accurate pitch/roll readings and to prevent drift
 * Read http://www.pieter-jan.com/node/11 for more info
 */
void Gyro::complementaryFilter() {
	 double pitchAcc, rollAcc;               
 
    /* Integration of gyro data (degrees per second) over time (seconds) = angle (degrees) 
     * Pitch is a rotation around the x-axis, roll rotates around the y-axis.
     */
    pitch += (gyroData[0] / GYRO_SENSITIVITY) * (0.001 * deltaTime);
    roll -= (gyroData[1] / GYRO_SENSITIVITY) * (0.001 * deltaTime);
 
    /* Apply drift compensation when the sum of accel forces is between 0.5G (8192) and 2G (32768) */
    int force = abs(accData[0]) + abs(accData[1]) + abs(accData[2]);
    if (force > 8192 && force < 32768) {
		
		/* Complementary filter on pitch (x-axis rotation) */
        pitchAcc = atan2(accData[1], accData[2]) * 180 / PI;
        pitch = pitch * 0.98 + pitchAcc * 0.02;
 
		/* Complementary filter on roll (y-axis rotation) */
        rollAcc = atan2(accData[0], accData[2]) * 180 / PI;
        roll = roll * 0.98 + rollAcc * 0.02;
    }
}

/* A calibration process to be used at the beginning (in the setup() call for example).
 * It calculates an average from a high number of readings, that can later be subtracted from
 * any new readings to zero them out.
 */
void Gyro::calibrate() {

	/* Buffers for sum calculation */
	long pitchSum = 0, rollSum = 0;
	/* Number of readings (equals 2 seconds / dt) */
	int iterations = (int)(2000/deltaTime);
	/* Number of burn in readings */
	int burninIterations = 300;


	/* Take readings, burn in the filter for a while and then start to sum them up */
	for(int i = 0; i < iterations + burninIterations; i++) {

		long lastMillis = millis();

		read();
		complementaryFilter();

		if(i >= burninIterations) {
			/* Sum up pitch and roll values */
			pitchSum += pitch;
			rollSum += roll;
		}

		delay(deltaTime - (millis()-lastMillis));
	}

	/* Divide the sums by number of iterations to get an average */
	zeroPitch = (pitchSum / iterations);
	zeroRoll = (rollSum / iterations);

	calibrated = true;
}
