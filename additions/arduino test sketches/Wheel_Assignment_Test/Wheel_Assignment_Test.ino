const float pi = 3.14159267;

float toDegrees(float radians) {
  return radians / 2 / pi * 360;
}

float toRadians(float degrees) {
  return degrees / 360 * 2 * pi;
}



void setup() {
  Serial.begin(115200);
  Serial.println("Please enter a valid force vector angle");
  Serial.println("==============================================");
}

float wheelAngles[3][2]  = {
  {330, 150},
  {210, 30}, 
  {90, 270}
};

void loop() {

  if(Serial.available() > 0) {   
    float forceAngle = Serial.parseFloat();

    Serial.println("Given force vector angle is: ");
    Serial.println(forceAngle);
    Serial.println("= = = = = = = = = = = = = = = = = = = = = = = ");
    
    float newWheelSpeeds[3] = {0,0,0};

    for(int wheel = 0; wheel < 3; wheel++) {
      for(int dir = 0; dir < 2; dir++) {
        float curWheelAngle = wheelAngles[wheel][dir];

        float phi = fmod(abs(curWheelAngle - forceAngle),360);
        float angleDiff = phi > 180 ? 360 - phi : phi;

        Serial.print("Wheel "); Serial.print(wheel); Serial.print((dir == 0)?"s forward":"s backward"); Serial.print("s angle difference is: "); Serial.println(angleDiff);
        if(angleDiff < 90) {
          newWheelSpeeds[wheel] = sin(toRadians(90 - abs(angleDiff)));
          if(dir == 1)
            newWheelSpeeds[wheel] = -newWheelSpeeds[wheel];
        }  
      }
    }

    Serial.println("= = = = = = = = = = = = = = = = = = = = = = = ");
    Serial.println("The wheels are now spinning with the following speeds: ");

    for(int i = 0; i < 3; i++) {
      Serial.print("Wheel "); Serial.print(i); Serial.print(": "); Serial.println(newWheelSpeeds[i]);
    }
    Serial.println("==============================================");
  }
}
