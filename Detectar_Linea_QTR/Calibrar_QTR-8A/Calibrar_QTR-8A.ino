// Calibración del sensor QTR-8A
#include <QTRSensors.h>

// Declaramos el sensor
QTRSensorsAnalog qtr((unsigned char[]) {A0, A1, A2, A3, A4, A5, A6, A7}, 8);

void setup() {
  Serial.begin(9600);
  Serial.println("Calibrando sensores...");

  // Encender LEDs infrarrojos del QTR conectando LEDON
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  delay(500);  // Esperar estabilización

  // Calibración automática
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
    delay(5);
  }

  Serial.println("Calibración terminada.\n");

  // Imprimir valores mínimos y máximos de cada sensor
  Serial.println("Sensor\tMínimo\tMáximo");
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(qtr.calibratedMinimumOn[i]);
    Serial.print("\t");
    Serial.println(qtr.calibratedMaximumOn[i]);
  }
}

void loop() {
  // No hacemos nada en loop, solo calibramos y mostramos una vez
}
