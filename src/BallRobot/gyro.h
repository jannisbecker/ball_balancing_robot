#ifndef GYRO_H
	#define GYRO_H
	
	#define GYRO_SENSITIVITY 110

	#ifndef PI
		#define PI 3.14159265358979323846
	#endif

	#include <LSM6.h>

	class Gyro
	{
	  public:
	    Gyro(double dt);
	    bool init();
	    void calibrate();
	    void update(double *xAngle, double *yAngle);

	  private:
	  	LSM6 imu;
	  	
	  	bool calibrated;
	  	double deltaTime;
	  	double pitch, roll;
	  	int gyroData[3] = {0,0,0}, accData[3] = {0,0,0};
	  	double zeroPitch = 0, zeroRoll = 0;

	  	void Gyro::read();
		void Gyro::complementaryFilter();
	};
#endif
