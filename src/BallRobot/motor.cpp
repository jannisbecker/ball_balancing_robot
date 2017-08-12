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

	Serial.print(" ");
	Serial.print(wheelSpeeds[0]);
	Serial.print(" ");
	Serial.print(wheelSpeeds[1]);
	Serial.print(" ");
	Serial.println(wheelSpeeds[2]);
	
}

void OmniMotorDriver::init() {
	for(int motor = 0; motor < 3; motor++) {
		for(int pinType = 0; pinType < 2; pinType++) {

			int pinNumber = motorPins[motor][pinType];
			pinMode(pinNumber, OUTPUT);
		}

		stop();
	}
	
} 

/* TODO: Turn the actual motor with the given parameters */
void OmniMotorDriver::driveSingle(int motor, double speed) {
	int pwmPin = motorPins[motor][0];
	int dirPin = motorPins[motor][1];

	int reverse = (speed < 0)? HIGH : LOW;
	int speedAbs = abs(speed);

	int pwm = max(255 - speedAbs,0);

	analogWrite(pwmPin, pwm);
	digitalWrite(dirPin, (reverse)? HIGH : LOW);
}

/* Stops all three motors (sets speeds to 0) */
void OmniMotorDriver::stop() { 
	for(int motor = 0; motor < 3; motor++) {
		driveSingle(motor, 0);
	}
}

/* Calculates the relative speed distribution on all three wheels */
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

/* Calculate the angle of a 2D origin vector (x,y) < */
float OmniMotorDriver::calcAngle(float x, float y) { 
	return fmod(toDegrees(atan2(y, x)) + 270, 360);
}

/* Calculate the length of a 2D vector */
float OmniMotorDriver::calcLength(float x, float y) { 
	return sqrt(x*x + y*y);
}

/* Turn radians into degrees */
float OmniMotorDriver::toDegrees(float radians) { 
	return radians / 2 / PI * 360;
}

/* Turn degrees into radians */
float OmniMotorDriver::toRadians(float degrees) { 
	return degrees / 360 * 2 * PI;
}