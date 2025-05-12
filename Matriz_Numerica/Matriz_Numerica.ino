#include <PololuSH1106.h>
#include <Wire.h>
#include <Keypad.h>

// Configuración de la OLED (SPI Software)
PololuSH1106 display(13, 7, 6, 5, 4);  // SCK, MOSI, CS, DC, RST

// Configuración del Teclado Matricial 4x4
const byte FILAS = 4;
const byte COLUMNAS = 4;

char keys[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pinesFilas[FILAS] = {A0, A1, A2, A3};   // Pines para filas
byte pinesColumnas[COLUMNAS] = {3, 2, 1, 0}; // Pines para columnas

Keypad teclado = Keypad(makeKeymap(keys), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

// Variables para la contraseña
String password = "1234";   // Contraseña inicial
String input = "";          // Entrada del usuario
bool cambiarClave = false;  // Estado para cambiar la clave
String nuevaClave = "";
bool claveCorrecta = false; // Indica si se ingresó la clave correcta

void setup() {
  Serial.begin(9600);  // Inicia la comunicación serial
  display.init();      // Inicializa la pantalla OLED
  display.setLayout11x4();  // Resolución de la pantalla
  display.clear();          // Limpia la pantalla
  mostrarMensaje("Ingrese clave:");
}

void loop() {
  char tecla = teclado.getKey();
  if (tecla) {
    Serial.print("Tecla presionada: ");
    Serial.println(tecla);  // Muestra la tecla presionada en el monitor serial

    if (cambiarClave) {
      gestionarCambioClave(tecla);
    } else {
      gestionarIngresoClave(tecla);
    }
  }
}

// Función para manejar el ingreso de clave normal
void gestionarIngresoClave(char tecla) {
  if (tecla == '#') {  // Verificar contraseña
    if (input == password) {
      claveCorrecta = true;  // La clave ingresada es correcta
      mostrarMensaje("Acceso Permitido");
      delay(2000);
      input = "";
      mostrarMensaje("Ingrese clave:");
    } else {
      mostrarMensaje("Clave Incorrecta");
      delay(2000);
      input = "";
      mostrarMensaje("Ingrese clave:");
    }
  } else if (tecla == 'A' && input == password) {  // Cambiar a modo de cambio de clave si la clave es correcta
    input = "";
    nuevaClave = "";
    cambiarClave = true;
    mostrarMensaje("Nueva clave:");
  } else if (isDigit(tecla)) {  // Permitir solo dígitos
    if (input.length() < 4) {  // Evita que ingrese más de 4 dígitos
      input += tecla;
      mostrarMensaje("Clave: " + String(input));
    }
  } else if (tecla == 'D') {  // Borrar el último dígito
    if (input.length() > 0) {
      input.remove(input.length() - 1);  // Elimina el último carácter
      mostrarMensaje("Clave: " + String(input));
    }
  }
}

// Función para manejar el cambio de clave
void gestionarCambioClave(char tecla) {
  if (tecla == '#') {  // Confirmar cambio de clave
    if (nuevaClave.length() == 4) {  // Solo permite claves de 4 dígitos
      password = nuevaClave;
      mostrarMensaje("Clave Guardada!");
      delay(2000);
      cambiarClave = false;
      mostrarMensaje("Ingrese clave:");
    } else {
      mostrarMensaje("Clave Inválida!");
      delay(2000);
      mostrarMensaje("Nueva clave:");
    }
  } else if (tecla == '*') {  // Cancelar cambio
    cambiarClave = false;
    mostrarMensaje("Cambio Cancelado");
    delay(2000);
    mostrarMensaje("Ingrese clave:");
  } else if (isDigit(tecla)) {  // Permitir solo dígitos
    if (nuevaClave.length() < 4) {
      nuevaClave += tecla;
      mostrarMensaje("Nueva: " + String(nuevaClave));
    }
  } else if (tecla == 'D') {  // Borrar el último dígito en nueva clave
    if (nuevaClave.length() > 0) {
      nuevaClave.remove(nuevaClave.length() - 1);  // Elimina el último carácter
      mostrarMensaje("Nueva: " + String(nuevaClave));
    }
  }
}

// Función para actualizar la pantalla OLED
void mostrarMensaje(String mensaje) {
  display.clear();
  
  int anchoPantalla = 11;  // Máximo de caracteres por línea en el layout 11x4
  int espacio = mensaje.indexOf(' '); // Buscar el primer espacio
  
  String linea1, linea2;

  if (espacio != -1 && espacio < anchoPantalla) {  
    // Si hay un espacio y cabe en la primera línea, dividir mensaje
    linea1 = mensaje.substring(0, espacio);
    linea2 = mensaje.substring(espacio + 1);
  } else if (mensaje.length() > anchoPantalla) {
    // Si es demasiado largo, dividir a la fuerza
    linea1 = mensaje.substring(0, anchoPantalla);
    linea2 = mensaje.substring(anchoPantalla);
  } else {
    // Si es corto, imprimir en una sola línea
    linea1 = mensaje;
    linea2 = "";
  }

  // Centrar cada línea agregando espacios al inicio
  linea1 = centrarTexto(linea1, anchoPantalla);
  linea2 = centrarTexto(linea2, anchoPantalla);

  // Imprimir en pantalla
  display.gotoXY(0, 0);
  display.print(linea1);
  display.gotoXY(0, 1);
  display.print(linea2);

  display.display();
}

// Función para centrar un texto en una línea de ancho específico
String centrarTexto(String texto, int ancho) {
  int espacios = (ancho - texto.length()) / 2;
  String resultado = "";
  
  for (int i = 0; i < espacios; i++) {
    resultado += " ";
  }
  
  resultado += texto;
  return resultado;
}
