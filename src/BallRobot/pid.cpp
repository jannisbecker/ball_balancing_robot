#include <Arduino.h>
#include <PID_v1.h>

#include "pid.h"

/* Constructs a PIDControl object.
 * This class manages two independent PID controllers, one for
 * each x and y axis. The setpoint is always (0,0) and the tuning is the same for both units.
 * Params:
 * 	kP:		Initial kP tuning for both PID controllers
 * 	kI:		Initial kI tuning for both PID controllers
 * 	kD:		Initial kD tuning for both PID controllers
 * 	negLimit:	The minimum limit cap of the PID output values.
 * 	posLimit:	The maximum limit cap of the PID output values.
 * 	sampleTime:	Time value (in ms) indicating the interval in which the PID gets recalculated.
 *			This value must be equal to the main loop interval (dt)!
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

/* Computes new output values on both PID units and sets them to *outX and *outY 
 * Params:
 * 	inX:	Current x-Value to be used in the next calculation
 * 	inY:	Current y-Value to be used in the next calculation
 * 	outX:	Pointer that must be set to a variable which stores the x-value output of the PID controller
 *	outY:	Pointer that must be set to a variable which stores the y-value output of the PID controller
 */
void PIDControl::compute(double inX, double inY, double *outX, double *outY) {
	inputX = inX;
	inputY = inY;

	pidX.Compute();
	pidY.Compute();

	*outX = outputX;
	*outY = outputY;
}

/* Returns the last output and stores them in *outX and *outY
 * Params:
 * 	outX:	Pointer that must be set to a variable which stores the x-value output of the PID controller
 *	outY:	Pointer that must be set to a variable which stores the y-value output of the PID controller
 */
void PIDControl::getLast(double *outX, double *outY) {
	*outX = outputX;
	*outY = outputY;
}

/* Adjusts the setpoint (perhaps needed for autonomous control) 
 * Params:
 * 	setX:	New Setpoint for the PID controller calculating the X-rotation
 * 	setY:	New Setpoint for the PID controller calculating the Y-rotation
 */
void PIDControl::setSetpoint(double setX, double setY) {
	setpointX = setX;
	setpointY = setY;
}

/* Adjusts the PID tuning on both units
 * Params:
 * 	kP:		New kP tuning for both PID controllers
 * 	kI:		New kI tuning for both PID controllers
 * 	kD:		New kD tuning for both PID controllers
 */
void PIDControl::setPIDvalues(double kP, double kI, double kD) {
	pidX.SetTunings(kP, kI, kD);
	pidY.SetTunings(kP, kI, kD);   
}