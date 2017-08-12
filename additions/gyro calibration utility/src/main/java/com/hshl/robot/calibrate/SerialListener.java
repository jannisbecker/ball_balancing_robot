package com.hshl.robot.calibrate;

import java.io.BufferedReader;

import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;

public class SerialListener implements SerialPortEventListener {

	private BufferedReader reader;
	private SampleRing ring;

	public SerialListener(BufferedReader reader, SampleRing ring) {
		this.reader = reader;
		this.ring = ring;
	}

	@Override
	public void serialEvent(SerialPortEvent ev) {
		try {
			switch (ev.getEventType()) {
				case SerialPortEvent.DATA_AVAILABLE:
					String line = reader.readLine();
					if (line == null) {
						return;
					}
					int pos = 3;
					Sample sample = new Sample();
					sample.time = readLong(line, pos);
					pos += 10;
					pos += 1 + 3;
					sample.acc = readSensorSample(line, pos);
					pos += 3 * 6 + 2;
					pos += 1 + 3;
					sample.gyro = readSensorSample(line, pos);
					pos += 3 * 6 + 2;
					pos += 1 + 3;
					sample.raw = readFilterSample(line, pos);
					pos += 2 * 8 + 1;
					pos += 1 + 3;
					sample.kalman = readFilterSample(line, pos);
					pos += 2 * 8 + 1;
					pos += 1 + 3;
					sample.comp = readFilterSample(line, pos);
					pos += 2 * 8 + 1;
					pos += 1 + 3;
					sample.gyroFilter = readFilterSample(line, pos);

					ring.addSample(sample);
					System.out.println(sample);
					break;

				default:
					break;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private SensorSample readSensorSample(String line, int pos) {
		int x = readInteger(line, pos);
		pos += 6 + 1;
		int y = readInteger(line, pos);
		pos += 6 + 1;
		int z = readInteger(line, pos);
		return new SensorSample(x, y, z);
	}

	private FilterSample readFilterSample(String line, int pos) {
		double roll = readDouble(line, pos);
		pos += 8 + 1;
		double pitch = readDouble(line, pos);
		return new FilterSample(roll, pitch);
	}

	private double readDouble(String line, int start) {
		return Double.parseDouble(line.substring(start, start + 8).trim());
	}

	private int readInteger(String line, int start) {
		return Integer.parseInt(line.substring(start, start + 6).trim());
	}

	private long readLong(String line, int start) {
		return Long.parseLong(line.substring(start, start + 10).trim());
	}

}
