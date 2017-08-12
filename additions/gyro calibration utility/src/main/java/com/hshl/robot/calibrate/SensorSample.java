package com.hshl.robot.calibrate;

public class SensorSample {
	int x;
	int y;
	int z;

	public SensorSample(int x, int y, int z) {
		this.x = x;
		this.y = y;
		this.z = z;
	}

	@Override
	public String toString() {
		return "{" + x + ", " + y + ", " + z + "}";
	}
}
