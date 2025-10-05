#include <Wire.h>
#include <vl53l8cx.h>

#define SerialPort Serial

TwoWire DEV_I2C(i2c0, 0, 1); // Crea un bus I2C con SDA=GP0, SCL=GP1
VL53L8CX sensor(&DEV_I2C, -1);

uint8_t status;
uint8_t res = VL53L8CX_RESOLUTION_8X8; // Resolución 8x8
char report[256];

void setup() {
  SerialPort.begin(115200);
  while(!SerialPort);
  SerialPort.println("Iniciando VL53L8CX...");

  DEV_I2C.begin();
  
  sensor.begin();
  delay(150);
  status = sensor.init();
  delay(150);

  if (status) {
    SerialPort.println("! Error al inicializar el sensor");
    while(1);
  }

  // Configura resolución 8x8
  sensor.set_resolution(res); 
  delay(50);
  
  status = sensor.start_ranging();

  if (status) {
    SerialPort.println("! Error al iniciar mediciones");
    while(1);
  }

  SerialPort.println("VL53L8CX Inicializado 8x8 ✅");
}

// Función para imprimir los resultados de la matriz 8x8
void printResults(VL53L8CX_ResultsData *results) {
  for (uint8_t row = 0; row < 8; row++) {
    for (uint8_t col = 0; col < 8; col++) {
      uint8_t idx = row * 8 + col;
      SerialPort.print(results->distance_mm[idx]);
      if (col < 7) SerialPort.print(" "); // separador por espacio
    }
    SerialPort.println(); // nueva línea al final de cada fila
  }
  
  SerialPort.println("END"); // marca de fin del bloque
}

void loop() {
  VL53L8CX_ResultsData results;
  uint8_t newDataReady = 0;
  
  // Espera a que haya nuevos datos disponibles
  do {
    status = sensor.check_data_ready(&newDataReady);
  } while (!newDataReady);
  
  // Si no hay error y hay datos nuevos, obtén y muestra la información
  if (status == 0 && newDataReady) {
    status = sensor.get_ranging_data(&results);
    printResults(&results);
  }
  
  delay(100);
}

