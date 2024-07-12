//practica 1 del tercer bimestre//
/*
   Fundacion Kinal
   Centro educativo tecnico laboral Kinal
   Quinto perito
   Quinto electronica
   Codigo Tecnico: EB5AM / EB5AV 
   Curso: Taller de electronica digital y reparacion de computadoras I
   Proyecto: MOTORES ELECTRICOS, SENSOR ULTRASONICO, NEOPIXEL, ADC Y 
   SENSORES DIGITALES 
   Dev: Damasco Oxcal Vasquez
   Fecha: 12 DE JULIO 2024
*/

#include <LiquidCrystal_I2C.h>

// Configuración del LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C del LCD

// Definición de pines del motor stepper
#define IN1  9
#define IN2  10
#define IN3  11
#define IN4  12

// Secuencia de pasos para el motor stepper
int paso[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// Configuración del sensor de efecto Hall y del sensor de posición
const int hallPin = 2; // Pin del sensor de efecto Hall
const int obstaclePin = 3; // Pin del sensor de posición
const int ledPin = 4; // Pin del LED

volatile int revolutionCount = 0; // Contador de revoluciones
unsigned long lastUpdateTime = 0; // Tiempo del último cálculo de RPM
const unsigned long updateInterval = 60000; // Intervalo de actualización (1 minuto en milisegundos)
float rpm = 0; // Revoluciones por minuto

// Pin del potenciómetro
const int potPin = A0;

// Declaración de funciones
void setupMotor();
void stepMotor(int delayTime);
int readPotentiometer();
void measureRevolutions();
void displayRevolutions();
void countRevolutions();

void setup() {
  // Configuración de los pines del motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configuración de los sensores
  pinMode(hallPin, INPUT);
  pinMode(obstaclePin, INPUT);
  pinMode(ledPin, OUTPUT); // Configurar el pin del LED como salida
  attachInterrupt(digitalPinToInterrupt(hallPin), countRevolutions, RISING);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Contador de Rev.");

  // Inicializar el motor
  setupMotor();
}

void loop() {
  if (digitalRead(obstaclePin) == LOW) { // Verificar si el sensor de posición detecta algo
    int delayTime = readPotentiometer(); // Leer el valor del potenciómetro y ajustar el delay
    stepMotor(delayTime); // Mover el motor según el delay calculado

    // Calcular y mostrar RPM
    measureRevolutions();
    displayRevolutions();
  } else {
    lcd.setCursor(0, 1);
    lcd.print("No. De Rev: 0   "); // Limpiar la segunda línea cuando el sensor de posición no detecte
  }
}

void setupMotor() {
  // Inicializar los pines del motor stepper
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void stepMotor(int delayTime) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(IN1, paso[i][0]);
    digitalWrite(IN2, paso[i][1]);
    digitalWrite(IN3, paso[i][2]);
    digitalWrite(IN4, paso[i][3]);
    delay(delayTime); // Controla la velocidad del motor con el delay
  }
}

int readPotentiometer() {
  int potValue = analogRead(potPin); // Leer el valor del potenciómetro
  return map(potValue, 0, 1023, 0, 10); // Mapear el valor del potenciómetro a un rango de delay (10-200 ms)
}

void measureRevolutions() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= updateInterval) { // Cada minuto
    rpm = (revolutionCount * (60000.0 / updateInterval)); // Convertir a RPM
    revolutionCount = 0; // Resetear el contador de revoluciones
    lastUpdateTime = currentTime; // Actualizar el tiempo de la última actualización
  }
}

void displayRevolutions() {
  lcd.setCursor(0, 1);
  lcd.print("No. De Rev: ");
  lcd.print(rpm, 2); // Mostrar RPM con dos decimales
  lcd.print("   "); // Limpiar el resto de la línea
}

void countRevolutions() {
  revolutionCount++;
  digitalWrite(ledPin, HIGH); // Encender el LED
  delay(10); // Mantener el LED encendido por un corto tiempo (10 ms)
  digitalWrite(ledPin, LOW); // Apagar el LED
}
