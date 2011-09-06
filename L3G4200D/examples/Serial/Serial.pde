#include <Wire.h>
#include <L3G4200D.h>

L3G4200D gyro;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  gyro.enableDefault();
}

void loop() {
  gyro.read();

  Serial.print("G ");
  Serial.print("X: ");
  Serial.print(gyro.g.x);
  Serial.print(" Y: ");
  Serial.print(gyro.g.y);
  Serial.print(" Z: ");
  Serial.println(gyro.g.z);

  delay(100);
}