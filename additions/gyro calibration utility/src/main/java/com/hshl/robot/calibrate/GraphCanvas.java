package com.hshl.robot.calibrate;

import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

import javax.swing.JPanel;

public class GraphCanvas extends JPanel {
	private static final long serialVersionUID = 1L;

	private final SampleRing ring;

	private final Function<Sample, Number> supplier;

	private final String name;

	public GraphCanvas(String name, SampleRing ring, Function<Sample, Number> supplier) {
		this.name = name;
		this.ring = ring;
		this.supplier = supplier;
	}

	@Override
	public void paint(Graphics g) {
		super.paint(g);

		Dimension bounds = getSize();
		bounds.height -= 20;
		bounds.width -= 20;

		List<Sample> samples = new ArrayList<Sample>(ring.getSamples());
		double min = Integer.MAX_VALUE;
		double max = Integer.MIN_VALUE;
		for (Sample sample : samples) {
			double sampleValue = supplier.apply(sample).doubleValue();
			min = Math.min(sampleValue, min);
			max = Math.max(sampleValue, max);
		}

		// höhe skalieren: alle Werte passen auf den Bildschirm
		double maxScale = Math.max(Math.abs(max), Math.abs(min));
		double heightScale = 1.0f * bounds.height / (maxScale * 2);
		// Platz zwischen Sample-Punkten bestimmen
		double widthSteps = 1.0f * bounds.width / samples.size();

		Graphics2D g2d = (Graphics2D) g;
		g2d.drawRect(0, 20, bounds.width, bounds.height - 1);
		String text = name + " min: " + min + " max: " + max;
		g2d.drawChars(text.toCharArray(), 0, text.length(), 0, 18);
		g2d.translate(0, 20 + bounds.height / 2);
		g2d.drawLine(0, 0, bounds.width, 0);
		int lastx = 0;
		int lasty = 0;
		for (int i = 0; i < samples.size(); i++) {
			double sampleValue = supplier.apply(samples.get(i)).doubleValue();
			int x = (int) (i * widthSteps);
			int y = (int) (sampleValue * heightScale);
			g2d.drawLine(x, y, lastx, lasty);
			lastx = x;
			lasty = y;
		}

	}

}
