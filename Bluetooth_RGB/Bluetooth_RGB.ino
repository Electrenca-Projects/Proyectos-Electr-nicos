const int pinR = 11;
const int pinG = 10;
const int pinB = 9;

int valR = 0;
int valG = 0;
int valB = 0;

bool bluetoothConnected = false;  // Nuevo: bandera para detectar conexión

void setup() {
  Serial.begin(9600);      // Monitor Serial USB
  Serial1.begin(9600);     // Bluetooth TX1/RX1
  
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);

  delay(1000);
  Serial.println("🔄 Esperando conexión Bluetooth...");
}

void loop() {
  if (Serial1.available()) {
    if (!bluetoothConnected) {
      bluetoothConnected = true;
      Serial.println("✅ Conexión Bluetooth establecida");
    }

    String input = Serial1.readStringUntil('\n');
    input.trim();
    Serial.print("📨 Datos recibidos: ");
    Serial.println(input);
    parseAndSetRGB(input);
  }
}

void parseAndSetRGB(String data) {
  int firstComma = data.indexOf(',');
  int secondComma = data.indexOf(',', firstComma + 1);

  if (firstComma > 0 && secondComma > firstComma) {
    valR = data.substring(0, firstComma).toInt();
    valG = data.substring(firstComma + 1, secondComma).toInt();
    valB = data.substring(secondComma + 1).toInt();

    Serial.print("🎨 R: "); Serial.print(valR);
    Serial.print(" G: "); Serial.print(valG);
    Serial.print(" B: "); Serial.println(valB);

    analogWrite(pinR, valR);
    analogWrite(pinG, valG);
    analogWrite(pinB, valB);
  } else {
    Serial.println("⚠️ Formato inválido. Usa: R,G,B");
  }
}
