/*
The sensor outputs provided by the library are the raw
16-bit values obtained by concatenating the 8-bit high and
low accelerometer and gyro data registers. They can be
converted to units of g and dps (degrees per second) using
the conversion factors specified in the datasheet for your
particular device and full scale setting (gain).

Example: An LSM6DS33 gives an accelerometer Z axis reading
of 16276 with its default full scale setting of +/- 2 g. The
LA_So specification in the LSM6DS33 datasheet (page 11)
states a conversion factor of 0.061 mg/LSB (least
significant bit) at this FS setting, so the raw reading of
16276 corresponds to 16276 * 0.061 = 992.8 mg = 0.9928 g.
*/

#include <Wire.h>
#include <LSM6.h>
#include <Kalman.h>
#include<stdlib.h>

LSM6 imu;

// Puffer für die Ausgabe an die serielle Verbindung
char report[160];
char doubleBuffer[16];

// letzter Zeitpunkt der Messung
unsigned long time;

// Winkel über einen Kalman-Filter berechnet
Kalman kalmanRoll;
Kalman kalmanPitch;

// Winkel über einen Komplementär-Filter berechnet
double compXangle;
double compYangle;

// Winkel über die Drehrate (Gyroskop) berechnet (als Prüf-Methode)
double gyroXangle;
double gyroYangle;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
  
  
  // Start-Winkel ermitteln
  imu.read();
  int accX = imu.a.x;
  int accY = imu.a.y;
  int accZ = imu.a.z;
  
  // Aus den Lagesensordaten die Winkel ausrechnen
  // Siehe http://www.nxp.com/docs/en/application-note/AN3461.pdf ab Seite 10
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
  
  // Kalman-Filter mit den Startwerten initialisieren
  kalmanRoll.setAngle(roll); 
  kalmanPitch.setAngle(pitch);
  
  // Startwerte des Komplementärfilters initialisieren
  compXangle = roll;
  compYangle = pitch;
  
  // Startwerte der Probewinkel initialisieren
  gyroXangle = roll;
  gyroYangle = pitch;
}

void loop()
{
  // vergangene Zeit holen und in Sekunden umrechnen
  double elapsedTime = (double)(micros() - time) / 1000000; // Calculate delta time  
  time = micros();
  
  // neue sensordaten holen
  imu.read();
  double accX = imu.a.x;
  double accY = imu.a.y;
  double accZ = imu.a.z;
  // Aus den Lagesensordaten die Winkel ausrechnen
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;

  
  // Ausgemessen nach Datasheet LSM6DS33 Kapitel 4.6.1
  // https://www.pololu.com/file/0J1087/LSM6DS33.pdf
  const double GYRO_SENSITIVITY = 110.0;
  // Rohe Sensorwerte in Grad/Sekunde umrechnen
  double gyroXrate = imu.g.x / GYRO_SENSITIVITY;
  double gyroYrate = imu.g.y / GYRO_SENSITIVITY;

  // Neue Winkel über den Kalman Filter ausrechnen
  double kalmanAngleRoll = kalmanRoll.getAngle(roll, gyroXrate, elapsedTime);
  double kalmanAnglePitch = kalmanPitch.getAngle(pitch, gyroYrate, elapsedTime);

  // Zur Probe: Winkel über die Dreh-Rate ausrechnen
  gyroXangle += gyroXrate * elapsedTime;
  gyroYangle += gyroYrate * elapsedTime;
  
  // Winkel über einen Komplementärfilter ausrechen
  compXangle = 0.93 * (compXangle + gyroXrate * elapsedTime) + 0.07 * roll;
  compYangle = 0.93 * (compYangle + gyroYrate * elapsedTime) + 0.07 * pitch;
  //  K: %6d %6d C: %6d %6d G: %6d %6d
  snprintf(report, sizeof(report), "T: %10ld A: %6d %6d %6d G: %6d %6d %6d W: ",
    time,
    imu.a.x, imu.a.y, imu.a.z,
    imu.g.x, imu.g.y, imu.g.z
    );
  Serial.print(report);
  printDouble(roll);
  printDouble(pitch);  
  
  Serial.print("K: ");
  printDouble(kalmanAngleRoll);
  printDouble(kalmanAnglePitch);  
  
  Serial.print("C: ");
  printDouble(compXangle);
  printDouble(compYangle);  

  Serial.print("G: ");
  printDouble(gyroXangle);
  printDouble(gyroYangle);
  
  Serial.println();

  delay(100);
}

void printDouble(double d) {
  dtostrf(d,8,2,doubleBuffer);
  Serial.print(doubleBuffer);
  Serial.print(" ");
  
}