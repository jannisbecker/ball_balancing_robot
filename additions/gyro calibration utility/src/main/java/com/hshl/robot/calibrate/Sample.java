package com.hshl.robot.calibrate;

public class Sample {
	long time;
	SensorSample acc;
	SensorSample gyro;
	FilterSample raw;
	FilterSample kalman;
	FilterSample comp;
	FilterSample gyroFilter;

	@Override
	public String toString() {
		return "{time: " + time + ", "
				+ "acc: " + acc + ", "
				+ "gyro: " + gyro + ", "
				+ "raw: " + raw + ", "
				+ "kalman: " + kalman + ", "
				+ "comp: " + comp + ", "
				+ "gyro: " + gyroFilter
				+ "}";
	}
}
