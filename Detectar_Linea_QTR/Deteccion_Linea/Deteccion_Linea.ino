#include <QTRSensors.h>

// Define el objeto QTRSensorsAnalog para 8 sensores
QTRSensorsAnalog qtr((unsigned char[]){A0, A1, A2, A3, A4, A5, A6, A7}, 8);

int sensorMin[8];  // Valores mínimos calibrados
int sensorMax[8];  // Valores máximos calibrados
int sensorValues[8];
bool lineDetected = false; 
int pos;
int last_pos = 0;

void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH); // Enciende LED de sensores

  delay(1000);

  calibracion();

  Serial.println("Calibración terminada!");
  delay(2000);
}

void loop() {
  int line_position = GetPos();
  
  // Imprime la posición y una referencia "0" para ver en Plotter
  Serial.print(line_position);
  Serial.print("\t");    // Tab para separar en el Plotter
  Serial.println(0);     // Línea de referencia 0

  delay(50);
}

void calibracion() {
  for (int i = 0; i < 8; i++) {
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }

  Serial.println("Iniciando calibración...");

  for (int j = 0; j < 200; j++) {
    qtr.read(sensorValues);

    for (int i = 0; i < 8; i++) {
      if (sensorValues[i] < sensorMin[i]) sensorMin[i] = sensorValues[i];
      if (sensorValues[i] > sensorMax[i]) sensorMax[i] = sensorValues[i];
    }

    delay(10);
  }

  Serial.println("\nMínimos:");
  for (int i = 0; i < 8; i++) {
    Serial.print(sensorMin[i]);
    Serial.print("\t");
  }
  Serial.println();

  Serial.println("Máximos:");
  for (int i = 0; i < 8; i++) {
    Serial.print(sensorMax[i]);
    Serial.print("\t");
  }
  Serial.println();
}

void readSensors() {
  qtr.read(sensorValues);

  int mapped[8];

  for (int i = 0; i < 8; i++) {
    if (sensorValues[i] < sensorMin[i]) sensorValues[i] = sensorMin[i];
    if (sensorValues[i] > sensorMax[i]) sensorValues[i] = sensorMax[i];

    mapped[i] = map(sensorValues[i], sensorMin[i], sensorMax[i], 100, 0); // 100 blanco, 0 negro
  }

  int sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += mapped[i];
  }

  if (sum > 100) {
    lineDetected = true;
  } else {
    lineDetected = false;
    sum = 100;
  }
}

int GetPos() {
  readSensors();

  int prom = -3.5 * sensorValues[0]
             - 2.5 * sensorValues[1]
             - 1.5 * sensorValues[2]
             - 0.5 * sensorValues[3]
             + 0.5 * sensorValues[4]
             + 1.5 * sensorValues[5]
             + 2.5 * sensorValues[6]
             + 3.5 * sensorValues[7];

  int sum = sensorValues[0] + sensorValues[1] + sensorValues[2] + sensorValues[3]
            + sensorValues[4] + sensorValues[5] + sensorValues[6] + sensorValues[7];

  if (lineDetected) {
    pos = int(100.0 * prom / sum);
  } else {
    if (last_pos < 0) {
      pos = -255;
    } else {
      pos = 255;
    }
  }

  last_pos = pos;
  return pos;
}
