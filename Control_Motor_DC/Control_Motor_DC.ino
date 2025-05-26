// Pines del TB6612FNG
const int PWMA = 13;
const int AIN1 = 12;
const int AIN2 = 11;
const int STBY = 10;

// Entrada
const int potPin = A0;
const int buttonPin = A1;

// Variables botón y debounce
bool lastButtonState = HIGH;
bool buttonPressed = false;
unsigned long buttonPressTime = 0;
unsigned long buttonReleaseTime = 0;
const unsigned long debounceDelay = 50;
bool motorActivo = false;
bool direccion = true;

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  digitalWrite(STBY, LOW); // motor apagado al inicio

  Serial.begin(9600);
}

void loop() {
  bool reading = digitalRead(buttonPin);

  // Debounce simple
  static unsigned long lastDebounceTime = 0;
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Si cambia el estado del botón y está presionado
    if (reading == LOW && !buttonPressed) {
      buttonPressed = true;
      buttonPressTime = millis();
    }

    // Si el botón fue soltado después de estar presionado
    if (reading == HIGH && buttonPressed) {
      buttonPressed = false;
      buttonReleaseTime = millis();
      unsigned long pressDuration = buttonReleaseTime - buttonPressTime;

      if (pressDuration >= 1000) {
        // Presión larga: toggle motor ON/OFF
        motorActivo = !motorActivo;
        digitalWrite(STBY, motorActivo ? HIGH : LOW);
        Serial.println(motorActivo ? "Motor ENCENDIDO" : "Motor APAGADO");
      } else {
        // Presión corta: cambia dirección solo si motor activo
        if (motorActivo) {
          direccion = !direccion;
          Serial.print("Cambio de dirección: ");
          Serial.println(direccion ? "Adelante" : "Reversa");
        }
      }
    }
  }

  lastButtonState = reading;

  // Leer potenciómetro y mapear PWM
  int potValue = analogRead(potPin);
  int speedValue = map(potValue, 0, 1023, 0, 255);

  if (motorActivo) {
    digitalWrite(AIN1, direccion ? HIGH : LOW);
    digitalWrite(AIN2, direccion ? LOW : HIGH);
    analogWrite(PWMA, speedValue);
  } else {
    analogWrite(PWMA, 0);
  }

  // Monitor serial
  Serial.print("Motor: ");
  Serial.print(motorActivo ? "ON" : "OFF");
  Serial.print(" | Dirección: ");
  Serial.print(direccion ? "Adelante" : "Reversa");
  Serial.print(" | PWM: ");
  Serial.println(speedValue);

  delay(250);
}
