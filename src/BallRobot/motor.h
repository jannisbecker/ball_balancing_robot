#ifndef MOTOR_H
	#define MOTOR_H

  #ifndef PI
		#define PI 3.14159265358979323846
	#endif

	class OmniMotorDriver
	{
	  public:
	  	void init();

	    void drive(double inputX, double inputY);
	    void driveSingle(int motor, double speed);

	    void stop();

	  private:
	  	double motorGain = 20;

	  	int motorPins[3][2] = {
	  		{2,3},
	  		{4,5},
	  		{6,7}
	  	};

	  	float lastInX, lastInY;
	  	float wheelAngles[3][2] = {
	  		{330, 150},
			{210, 30}, 
			{90, 270}
		};
		float wheelRelSpeeds[3] = {0,0,0};
		float wheelSpeeds[3] = {0,0,0};

	  	void calcDistribution(float degAngle);
	  	float calcAngle(float x, float y);
	  	float calcLength(float x, float y);
	  	float toDegrees(float radians);
	  	float toRadians(float degrees);
	  	
	};
#endif
