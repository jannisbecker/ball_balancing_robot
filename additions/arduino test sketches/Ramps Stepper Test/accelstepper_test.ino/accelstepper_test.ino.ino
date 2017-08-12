#include <AccelStepper.h>

#define E_STEP_PIN 26
#define E_DIR_PIN 28
#define E_ENABLE_PIN 24

AccelStepper stepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);



void setup()
{

pinMode(E_ENABLE_PIN,OUTPUT);
digitalWrite(E_ENABLE_PIN,LOW);

// Change these to suit your stepper if you want
stepper.setEnablePin(E_ENABLE_PIN);
stepper.setPinsInverted(false, false, true);
stepper.setSpeed(4600);
stepper.setAcceleration(32000);
stepper.moveTo(5000);

pinMode(E_ENABLE_PIN,OUTPUT);
digitalWrite(E_ENABLE_PIN,LOW);

}
void loop()
{
// If at the end of travel go to the other end
if (millis() % 1000 < 2) {
  stepper.stop();
}

if(stepper.distanceToGo() == 0 && millis() % 1000 < 2) {
  stepper.moveTo(-stepper.currentPosition());
}

stepper.run();
}
