package com.hshl.robot.calibrate;

import java.awt.BorderLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.io.BufferedReader;
import java.io.InputStreamReader;

import javax.swing.AbstractAction;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.Timer;

import gnu.io.NRSerialPort;

public class CalibrateMain extends JFrame {
	private static final long serialVersionUID = 1L;

	private SampleRing ring = new SampleRing(100);

	private NRSerialPort serial;

	public CalibrateMain() {
		super("Calibrate IMU");
		registerComponents();
	}

	private void registerComponents() {
		JPanel mainPanel = new JPanel(new BorderLayout());
		JPanel toolPanel = new JPanel();
		toolPanel.setLayout(new BoxLayout(toolPanel, BoxLayout.LINE_AXIS));
		toolPanel.add(new JButton(new ConnectAction()));
		toolPanel.add(new JButton(new DisconnectAction()));
		mainPanel.add(toolPanel, BorderLayout.NORTH);

		JPanel accPane = new JPanel();
		accPane.setLayout(new GridLayout(4, 4));
		// Zeile 1
		accPane.add(new GraphCanvas("Acc X", ring, sample -> sample.acc.x));
		accPane.add(new GraphCanvas("Gyro X", ring, sample -> sample.gyro.x));
		accPane.add(new GraphCanvas("Raw Roll", ring, sample -> sample.raw.roll));
		accPane.add(new GraphCanvas("Raw Pitch", ring, sample -> sample.raw.pitch));

		// Zeile 2
		accPane.add(new GraphCanvas("Acc Y", ring, sample -> sample.acc.y));
		accPane.add(new GraphCanvas("Gyro Y", ring, sample -> sample.gyro.y));
		accPane.add(new GraphCanvas("Kalman Roll", ring, sample -> sample.kalman.roll));
		accPane.add(new GraphCanvas("Kalman Pitch", ring, sample -> sample.kalman.pitch));

		// Zeile 3
		accPane.add(new GraphCanvas("Acc Z", ring, sample -> sample.acc.z));
		accPane.add(new GraphCanvas("Gyro Z", ring, sample -> sample.gyro.z));
		accPane.add(new GraphCanvas("Comp Roll", ring, sample -> sample.comp.roll));
		accPane.add(new GraphCanvas("Comp Pitch", ring, sample -> sample.comp.pitch));

		// Zeile 4
		accPane.add(new JPanel());
		accPane.add(new JPanel());
		accPane.add(new GraphCanvas("Gyro Roll", ring, sample -> sample.gyroFilter.roll));
		accPane.add(new GraphCanvas("Gyro Pitch", ring, sample -> sample.gyroFilter.pitch));

		mainPanel.add(accPane, BorderLayout.CENTER);
		getContentPane().add(mainPanel);

		new Timer(1000, event -> {
			for (int i = 0; i < accPane.getComponentCount(); i++) {
				accPane.getComponent(i).repaint();
			}

		}).start();
	}

	public static void main(String[] args) {
		CalibrateMain main = new CalibrateMain();
		main.setDefaultCloseOperation(EXIT_ON_CLOSE);
		main.setSize(500, 500);
		main.setVisible(true);
	}

	private class ConnectAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		public ConnectAction() {
			super("Connect");
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			connect();
		}

	}

	private class DisconnectAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		public DisconnectAction() {
			super("Disconnect");
		}

		@Override
		public void actionPerformed(ActionEvent e) {
			if (serial != null) {
				serial.removeEventListener();
				serial.disconnect();
				serial = null;
			}
		}
	}

	public void connect() {
		try {
			for (String s : NRSerialPort.getAvailableSerialPorts()) {
				System.out.println("Availible port: " + s);
			}
			String port = "/dev/ttyUSB0";
			int baudRate = 9600;
			this.serial = new NRSerialPort(port, baudRate);
			serial.connect();
			BufferedReader input = new BufferedReader(new InputStreamReader(serial.getInputStream()));

			serial.addEventListener(new SerialListener(input, ring));
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
