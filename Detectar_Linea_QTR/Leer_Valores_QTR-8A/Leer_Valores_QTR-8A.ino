#include <QTRSensors.h>

QTRSensorsAnalog qtr((unsigned char[]) {A0, A1, A2, A3, A4, A5, A6, A7}, 8);

uint16_t sensorValues[8];

void setup() {
  Serial.begin(9600);
  Serial.println("Sensor0\tSensor1\tSensor2\tSensor3\tSensor4\tSensor5\tSensor6\tSensor7");
}

void loop() {
  qtr.read(sensorValues);

  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(sensorValues[i]);
    
    if (i < 7) {
      Serial.print("\t");
    }
  }

  Serial.println();
  delay(50);
}
