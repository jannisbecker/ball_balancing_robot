package com.hshl.robot.calibrate;

import java.util.Deque;
import java.util.concurrent.ConcurrentLinkedDeque;

import javax.swing.SwingUtilities;

/** ein "sliding window" mit jeweils immer den letzten Sample-Daten. */
public class SampleRing {
	private int size;
	private Deque<Sample> samples = new ConcurrentLinkedDeque<Sample>();

	public SampleRing(int size) {
		this.size = size;
	}

	public Deque<Sample> getSamples() {
		return samples;
	}

	public void addSample(Sample sample) {
		SwingUtilities.invokeLater(new Runnable() {
			@Override
			public void run() {
				// wenn der ring schon voll ist, erst das erste element entfernen, dann
				// dieses Sample hinten anfügen
				if (samples.size() >= size) {
					samples.removeFirst();
				}
				samples.addLast(sample);
			}
		});
	}
}
