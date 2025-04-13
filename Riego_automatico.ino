#include <Wire.h>
#include <PololuOLED.h>

// Configuración de los pines para la pantalla OLED (Pololu SH1106)
PololuSH1106 display(13, 7, 6, 5, 4);

// Definir los pines del sensor de humedad y la bomba
const int humedadPin = A0;   // Pin para leer el valor del sensor de humedad
const int bombaPin = 10;     // Pin para controlar la bomba de agua

// Variables de calibración para el sensor de humedad
int humedadMin = 1023;      // Valor máximo de humedad (suelo seco)
int humedadMax = 300;       // Valor mínimo de humedad (suelo saturado)
int humedadValor = 0;       // Variable para almacenar el valor leído del sensor

void setup() {

  // Configurar la pantalla OLED
  display.setLayout21x8();  // Establecer la resolución de la pantalla
  display.clear();          // Limpiar la pantalla

  // Animación de carga inicial
  display.clear();
  loadingAnimation();
  delay(2000);  // Esperar 2 segundos después de mostrar la animación de carga

  // Configuración del pin de la bomba como salida
  pinMode(bombaPin, OUTPUT);
  digitalWrite(bombaPin, LOW);  // Apagar la bomba al inicio

  delay(2000);  // Esperar 2 segundos antes de comenzar con la lectura del sensor
}

void loop() {

  // Leer el valor del sensor de humedad
  humedadValor = analogRead(humedadPin);

  // Calcular el porcentaje de humedad en el suelo
  int humedadPorcentaje = map(humedadValor, humedadMin, humedadMax, 0, 100);

  // Mostrar información en la pantalla OLED
  display.gotoXY(0, 0);
  display.print("Jacaranda");
  display.gotoXY(0, 2);
  display.print("Humedad: ");
  display.print(humedadPorcentaje);
  display.print("%");

  // Control de la bomba de acuerdo con el porcentaje de humedad
  if (humedadPorcentaje < 60) {
    // Si la humedad está por debajo del 60%, activar la bomba
    display.gotoXY(0, 3);
    display.print("Regar la planta!");
    digitalWrite(bombaPin, HIGH);  // Encender la bomba
  } else if (humedadPorcentaje > 80) {
    // Si la humedad está por encima del 80%, desactivar la bomba
    display.gotoXY(0, 3);
    display.print("Humedad adecuada");
    digitalWrite(bombaPin, LOW);  // Apagar la bomba
  } else {
    // Si la humedad está en un rango adecuado (60-80%)
    display.gotoXY(0, 3);
    display.print("Humedad suficiente");
    digitalWrite(bombaPin, LOW);  // Apagar la bomba
  }

  // Esperar 1 segundo antes de la siguiente lectura
  delay(1000);
}

// Función para mostrar una animación de carga inicial
void loadingAnimation() {

  // Mostrar el mensaje "Riego automatizado"
  display.gotoXY(0, 0);
  display.print("Riego automatizado");
  display.clear();

  // Mostrar el mensaje "Cargando..."
  display.gotoXY(0, 0);
  display.print("Cargando...");
  display.gotoXY(0, 2);

  // Barra de progreso con animación
  for (int i = 0; i <= 11; i++) {
    display.gotoXY(0, 3);
    
    // Dibujar bloques llenos en la barra de progreso
    for (int j = 0; j < i; j++) {
      display.print("#");
    }

    // Completar el resto de la barra con espacios vacíos
    for (int j = i; j < 11; j++) {
      display.print(" ");
    }

    display.display();  // Actualizar la pantalla
    delay(200);  // Esperar 200ms para el siguiente ciclo de animación
  }

  // Finalizar la animación con el mensaje "Completado!"
  display.clear();
  display.gotoXY(0, 3);
  display.print("Completado!");
  display.display();  // Actualizar la pantalla
  delay(1000);  // Esperar 1 segundo antes de continuar

  // Limpiar la pantalla y mostrar la información inicial
  display.clear();
  display.gotoXY(0, 0);
  display.print("Jacaranda");
  display.gotoXY(0, 1);
  display.print("Humedad: 60%-80%");
  display.gotoXY(0, 2);
}
