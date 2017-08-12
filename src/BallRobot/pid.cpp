#include <Arduino.h>
#include <PID_v1.h>

#include "pid.h"

/* Constructs a PIDControl object.
 * This class manages two independent PID controllers, one for
 * each x and y axis. The setpoint is always (0,0) and the tuning is the same for both units.
 */
PIDControl::PIDControl(double kP, double kI, double kD, int negLimit, int posLimit, int sampleTime) {
	pidX = PID(&inputX, &outputX, &setpointX, kP, kI, kD, P_ON_E, DIRECT);
	pidY = PID(&inputY, &outputY, &setpointY, kP, kI, kD, P_ON_E, DIRECT);

	setpointX = 0.0;
	setpointY = 0.0;

	pidX.SetOutputLimits(negLimit, posLimit);
	pidY.SetOutputLimits(negLimit, posLimit);

	pidX.SetSampleTime(sampleTime);
	pidY.SetSampleTime(sampleTime);
}

/* Enable both controllers */
void PIDControl::enable() {
	pidX.SetMode(AUTOMATIC);
	pidY.SetMode(AUTOMATIC);
}

/* Disable both controllers */
void PIDControl::disable() {
	pidX.SetMode(MANUAL);
	pidY.SetMode(MANUAL);
}

/* Computes new output values on both PID units and sets them to *outX and *outY */
void PIDControl::compute(double inX, double inY, double *outX, double *outY) {
	inputX = inX;
	inputY = inY;

	pidX.Compute();
	pidY.Compute();

	*outX = outputX;
	*outY = outputY;
}

/* Returns the last output */
void PIDControl::getLast(double *outX, double *outY) {
	*outX = outputX;
	*outY = outputY;
}

/* Adjusts the setpoint (perhaps needed for autonomous control) */
void PIDControl::setSetpoint(double setX, double setY) {
	setpointX = setX;
	setpointY = setY;
}

/* Adjusts the PID tuning on both units */
void PIDControl::setPIDvalues(double kP, double kI, double kD) {
	pidX.SetTunings(kP, kI, kD);
	pidY.SetTunings(kP, kI, kD);   
}