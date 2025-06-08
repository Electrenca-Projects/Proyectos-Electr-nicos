#include <QTRSensors.h>
#include <Pushbutton.h>
#include <FastGPIO.h>

// Sensores QTR
QTRSensorsAnalog qtr((unsigned char[]){A0, A1, A2, A3, A4, A5, A10, A11}, 8);
int sensorMin[8], sensorMax[8], sensorValues[8];
bool lineDetected = false; 
int pos, last_pos = 0;

#define LEDON 13

// Pines motores TB6612FNG (organizados por cercanía física en A-Star)
#define PWMA 9
#define AIN1 8
#define AIN2 7
#define STBY 6
#define BIN1 5
#define BIN2 4
#define PWMB 3

// Botón
Pushbutton button(0); // pin 0 con INPUT_PULLUP interno

bool calibrado = false;
bool enMarcha = false;
bool motoresDetenidos = true;

void setup() {
  Serial.begin(115200);

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  FastGPIO::Pin<AIN1>::setOutputLow();
  FastGPIO::Pin<AIN2>::setOutputLow();
  FastGPIO::Pin<BIN1>::setOutputLow();
  FastGPIO::Pin<BIN2>::setOutputLow();
  FastGPIO::Pin<STBY>::setOutputLow(); // STBY bajo = desactivado

  FastGPIO::Pin<LEDON>::setOutputHigh();  // Encender LEDs infrarrojos del QTR conectando LEDON

  Serial.println("Presiona el botón para calibrar o iniciar...");
}

void loop() {
  if (!calibrado && button.isPressed()) {
    unsigned long start = millis();
    while (button.isPressed()) {
      if (millis() - start > 2000) {
        calibracion();
        calibrado = true;
        Serial.println("✅ Calibración terminada.");

        Serial.println("📊 Calibración de sensores (alineados verticalmente):");
        for (int i = 0; i < 8; i++) {
          Serial.print(" S");
          Serial.print(i);
          Serial.print("  ");
        }
        Serial.println();

        for (int i = 0; i < 8; i++) {
          int esp = 5 - String(sensorMin[i]).length();
          for (int j = 0; j < esp / 2; j++) Serial.print(" ");
          Serial.print(sensorMin[i]);
          for (int j = 0; j < (esp + 1) / 2; j++) Serial.print(" ");
        }
        Serial.println("← Min");

        for (int i = 0; i < 8; i++) {
          int esp = 5 - String(sensorMax[i]).length();
          for (int j = 0; j < esp / 2; j++) Serial.print(" ");
          Serial.print(sensorMax[i]);
          for (int j = 0; j < (esp + 1) / 2; j++) Serial.print(" ");
        }
        Serial.println("← Max");

        while (button.isPressed());
        delay(300);
        return;
      }
    }
  }

  if (calibrado && button.getSingleDebouncedPress()) {
    enMarcha = !enMarcha;  // Alternar estado
    if (enMarcha) {
      digitalWrite(STBY, HIGH);
      Serial.println("🚗 Iniciando seguidor de línea...");
      motoresDetenidos = false;
    } else {
      detener();
      Serial.println("⏹️ Motores detenidos");
      motoresDetenidos = true;
      digitalWrite(STBY, LOW);
    }
    delay(200); // Evitar rebotes
    return;
  }

  if (!enMarcha) {
    if (!motoresDetenidos) {
      detener();
      Serial.println("⏹️ Motores detenidos");
      motoresDetenidos = true;
      digitalWrite(STBY, LOW);
    }
    return;
  }

  // Control normal en marcha
  int line_position = GetPos();
  int pwm_value = 50;

  Serial.print(line_position);
  Serial.print(",");
  Serial.println(0);

  if (line_position > -10 && line_position < 10) {
    avanzar(pwm_value);
  } else if (line_position <= -10) {
    girarIzquierda(pwm_value);
  } else if (line_position >= 10) {
    girarDerecha(pwm_value);
  }

  delay(50);
}

// ------------- FUNCIONES ------------------------

void calibracion() {
  for (int i = 0; i < 8; i++) {
    sensorMin[i] = 1023;
    sensorMax[i] = 0;
  }

  Serial.println("🔧 Calibrando...");

  for (int j = 0; j < 200; j++) {
    qtr.read(sensorValues);
    for (int i = 0; i < 8; i++) {
      if (sensorValues[i] < sensorMin[i]) sensorMin[i] = sensorValues[i];
      if (sensorValues[i] > sensorMax[i]) sensorMax[i] = sensorValues[i];
    }
    delay(10);
  }
}

void readSensors() {
  qtr.read(sensorValues);
  int sum = 0;
  for (int i = 0; i < 8; i++) {
    sensorValues[i] = constrain(sensorValues[i], sensorMin[i], sensorMax[i]);
    sensorValues[i] = map(sensorValues[i], sensorMin[i], sensorMax[i], 100, 0);
    sum += sensorValues[i];
  }
  lineDetected = (sum > 100);
}

int GetPos() {
  readSensors();
  int prom = -3.5 * sensorValues[0] - 2.5 * sensorValues[1] - 1.5 * sensorValues[2] - 0.5 * sensorValues[3]
           + 0.5 * sensorValues[4] + 1.5 * sensorValues[5] + 2.5 * sensorValues[6] + 3.5 * sensorValues[7];

  int sum = 0;
  for (int i = 0; i < 8; i++) sum += sensorValues[i];

  if (lineDetected && sum != 0) {
    pos = int(100.0 * prom / sum);
  } else {
    pos = (last_pos < 0) ? -255 : 255;
  }
  last_pos = pos;
  return pos;
}

// ------------------ CONTROL MOTORES ---------------------

void avanzar(int velocidad) {
  FastGPIO::Pin<AIN1>::setOutputValue(1);
  FastGPIO::Pin<AIN2>::setOutputValue(0);
  FastGPIO::Pin<BIN1>::setOutputValue(1);
  FastGPIO::Pin<BIN2>::setOutputValue(0);
  analogWrite(PWMA, velocidad);
  analogWrite(PWMB, velocidad);
}

void girarIzquierda(int velocidad) {
  FastGPIO::Pin<AIN1>::setOutputValue(0);
  FastGPIO::Pin<AIN2>::setOutputValue(1);
  FastGPIO::Pin<BIN1>::setOutputValue(1);
  FastGPIO::Pin<BIN2>::setOutputValue(0);
  analogWrite(PWMA, velocidad);
  analogWrite(PWMB, velocidad);
}

void girarDerecha(int velocidad) {
  FastGPIO::Pin<AIN1>::setOutputValue(1);
  FastGPIO::Pin<AIN2>::setOutputValue(0);
  FastGPIO::Pin<BIN1>::setOutputValue(0);
  FastGPIO::Pin<BIN2>::setOutputValue(1);
  analogWrite(PWMA, velocidad);
  analogWrite(PWMB, velocidad);
}

void detener() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  FastGPIO::Pin<AIN1>::setOutputLow();
  FastGPIO::Pin<AIN2>::setOutputLow();
  FastGPIO::Pin<BIN1>::setOutputLow();
  FastGPIO::Pin<BIN2>::setOutputLow();
}

