#include <Arduino.h>

#include "motor.h"

#define M1_PWM 1
#define M1_DIR 2

#define M2_PWM 3
#define M2_DIR 4

#define M3_PWM 5
#define M3_DIR 6


/* Drive all three motors, given the info of where (x,y) to go
 * This method calculates the angle difference between the
 * destination vector and the two angles (forwards and backwards direction) 
 * of each wheel and if the difference is less than 90°, the wheel is supposed
 * to rotate in that particular direction, the speed of which being interpolated
 * on a sine curve. Note: In this system, 0° is UP (along the y-axis) and the
 * angle increases counterclockwise. The Forward direction of the motors means
 * that it turns clockwise (right round) if you look along the motor body.
 * Params:
 * 	inputX:	The x-value of the 2D force vector that the robot is supposed to move to.
 * 	inputY: The y-value of the 2D force vector that the robot is supposed to move to.
 */
void OmniMotorDriver::drive(double inputX, double inputY) { 
	float forceAngle = calcAngle(inputX, inputY);
	
	
	//Serial.print("FORCE ANGLE: ");
	//Serial.print(" ");
	//Serial.println(forceAngle);
	
	
	

	/* Calculates the relative speed distribution on all three wheels */
	calcDistribution(forceAngle);

	/* Calculates the force (length) of the direction vector */
	float force = calcLength(inputX, inputY);

	/* Multiply relative speed by force for each motor and 
	 * call the driveSingle method to actually turn them.
	 */
	for(int motor = 0; motor < 3; motor++) {
		wheelSpeeds[motor] = wheelRelSpeeds[motor] * force * motorGain;
		driveSingle(motor, wheelSpeeds[motor]);
	}

	/* Print some values for debugging purposes. Viewable in the Arduino IDE Serial plotter */
	Serial.print(" ");
	Serial.print(wheelSpeeds[0]);
	Serial.print(" ");
	Serial.print(wheelSpeeds[1]);
	Serial.print(" ");
	Serial.println(wheelSpeeds[2]);
	
}

/* Motor driver init method
 * Loops through the 2D array containing the direction and pwm pins of each motor
 * and initializes them as output. Also sets all the motor speeds to 0 by calling the stop() method.
 */  
void OmniMotorDriver::init() {
	for(int motor = 0; motor < 3; motor++) {
		for(int pinType = 0; pinType < 2; pinType++) {

			int pinNumber = motorPins[motor][pinType];
			pinMode(pinNumber, OUTPUT);
		}

		stop();
	}
	
} 

/* Method for driving a single motor 
 * Uses the given parameters (motor number and speed) to drive a single motor.
 * This method also contains some special calculations and number range transformations
 * that are needed to send the correct values to the motor.
 * Params:
 * 	motor:	[0-2] The number of the motor. Looking from the top back, the motors are numbered
 *		clockwise from the top left one starting with 0. Top right is 1 and back motor is 2.
 * 	speed:	[-255 - 255] Combined value for motor speed and direction (thus can be negative).
 */
void OmniMotorDriver::driveSingle(int motor, double speed) {
	/* Retrieve the required pin numbers */
	int pwmPin = motorPins[motor][0];
	int dirPin = motorPins[motor][1];

	/* Determine the reverse state and make the speed value positive */
	int reverse = (speed < 0)? HIGH : LOW;
	int speedAbs = abs(speed);

	/* Invert the speed value (needed for FIT0441 motors) and cap it to [0-255] */
	int pwm = max(255 - speedAbs,0);

	/* Write the speed and reverse data to the actual pins */
	analogWrite(pwmPin, pwm);
	digitalWrite(dirPin, reverse);
}

/* Stops all three motors (sets speeds to 0) */
void OmniMotorDriver::stop() { 
	for(int motor = 0; motor < 3; motor++) {
		driveSingle(motor, 0);
	}
}

/* Calculates the relative speed distribution on all three wheels 
 * that has to happen for the robot to move in the given direction. 
 * Params:
 * 	degAngle:	Angle in degrees in the agreed upon coordinate system (Read above).
 *			The angle indicates the final moving direction.
 */	
void OmniMotorDriver::calcDistribution(float degAngle) {
	/* Loop through each wheel and each of the two directions of them */
	for(int wheel = 0; wheel < 3; wheel++) {
		for(int dir = 0; dir < 2; dir++) {

			/* Get the angle at which this direction of the wheel is "pointing" */
			float curWheelAngle = wheelAngles[wheel][dir];

			/* Calculate the difference between the direction vector and the wheel direction */
			float phi = fmod(abs(curWheelAngle - degAngle),360);
			float angleDiff = phi > 180 ? 360 - phi : phi;

			/* If its < 90°, the wheel has to turn in that direction.
			 * On exactly 90° the wheel wouldn't turn at all and
			 * on more than 90°, the wheel's other direction will be < 90° anyway 
			 */
			if(angleDiff < 90) {
				wheelRelSpeeds[wheel] = sin(toRadians(90 - abs(angleDiff)));
				if(dir == 1)
					wheelRelSpeeds[wheel] = -wheelRelSpeeds[wheel];
			}  
		}
	}
}

/* Calculate the angle of a 2D origin vector (x,y) in the agreed upon coordinate system (Read above)
 * Params:
 * 	x:	x-value of the direction vector of which to calculate the angle of
 * 	y:	y-value of the direction vector of which to calculate the angle of
 * Returns:
 *	Float value storing an angle (in degrees)
 */	
float OmniMotorDriver::calcAngle(float x, float y) { 
	return fmod(toDegrees(atan2(y, x)) + 270, 360);
}

/* Calculate the length of a 2D origin vector (x,y)
 * Params:
 * 	x:	x-value of the direction vector of which to calculate the length of
 * 	y:	y-value of the direction vector of which to calculate the length of
 * Returns:
 *	Float value storing a length of the given vector
 */
float OmniMotorDriver::calcLength(float x, float y) { 
	return sqrt(x*x + y*y);
}

/* Convert angles given in radians into a degree format
 * Params:
 * 	radians: An angle in a radians number format
 * Returns:
 *	Float value storing an angle (in degrees).
 */
float OmniMotorDriver::toDegrees(float radians) { 
	return radians / 2 / PI * 360;
}

/* Convert angles given in degrees into a radians format
 * Params:
 * 	degrees: An angle in a degrees number format
 * Returns:
 *	Float value storing an angle (in radians).
 */
float OmniMotorDriver::toRadians(float degrees) { 
	return degrees / 360 * 2 * PI;
}