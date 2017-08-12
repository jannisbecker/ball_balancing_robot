#ifndef PID_H
	#define PID_H

	#include <PID_v1.h>

	class PIDControl
	{
	  public:
	    PIDControl(double kP, double kI, double kD, int negLimit, int posLimit, int sampleTime);
	    void enable();
	    void disable();
	    void compute(double inX, double inY, double *outX, double *outY);
	    void getLast(double *outX, double *outY);
	    void setSetpoint(double setX, double setY);
	    void setPIDvalues(double kP, double kI, double kD);

	  private:
	  	double inputX, inputY, setpointX, setpointY, outputX, outputY;
	  	PID pidX{&inputX, &outputX, &setpointX, 0, 0, 0, P_ON_M, DIRECT},
			pidY{&inputY, &outputY, &setpointY, 0, 0, 0, P_ON_M, DIRECT};
	  	double kP, kI, kD;
	};
#endif