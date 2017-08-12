package com.hshl.robot.calibrate;

public class FilterSample {
	double roll;
	double pitch;

	public FilterSample(double roll, double pitch) {
		this.roll = roll;
		this.pitch = pitch;
	}

	@Override
	public String toString() {
		return "{" + roll + ", " + pitch + "}";
	}
}
