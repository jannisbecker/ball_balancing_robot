#include <Wire.h>
#include <LSM6.h>
#include <PID_v1.h>
#include <stdlib.h>

LSM6 imu;

// Puffer für die Ausgabe an die serielle Verbindung
char report[160];
char doubleBuffer[16];

// letzter Zeitpunkt der Messung
unsigned long time;

// Winkel über einen Komplementär-Filter berechnet
double compXangle;
double compYangle;


void setup() {
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
  
  // Startwerte des Komplementärfilters initialisieren
  compXangle = roll;
  compYangle = pitch;

}

void loop() {
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
  
  // Winkel über einen Komplementärfilter ausrechnen
  compXangle = 0.93 * (compXangle + gyroXrate * elapsedTime) + 0.07 * roll;
  compYangle = 0.93 * (compYangle + gyroYrate * elapsedTime) + 0.07 * pitch;
  
  Serial.print(compXangle);
  Serial.print("\t");
  Serial.println(compYangle);

  delay(100);

}
