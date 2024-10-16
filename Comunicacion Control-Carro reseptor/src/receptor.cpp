#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// PINes del puENte H
#define IN1 26  // Motor 1 adelante
#define IN2 27  // Motor 1 atrás
#define IN3 25  // Motor 2 adelante
#define IN4 33  // Motor 2 atrás
#define ENA 32  // PWM Motor 1
#define ENB 14  // PWM Motor 2
#define solenoidPIN 12 // pIN solENoide

// Configuración NRF24L01
RF24 radio(22, 21);  // CE, CSN
const byte direccion[6] = "00001";

struct DatosControl {
  int ejeX_1;
  int ejeY_2;
  bool sol_signal;
};

DatosControl datosControl;

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(solenoidPIN, OUTPUT);
  radio.begin();
  if (!radio.begin()) {
    Serial.println("No se pudo INiciar el módulo RF");
    while (1);
  }
  radio.openReadingPipe(0, direccion);
  //radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&datosControl, sizeof(datosControl));
    
    // Control Adelante Atras
    if (datosControl.ejeY_2 > 512) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, map(datosControl.ejeY_2, 512, 1023, 0, 255));
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, map(datosControl.ejeY_2, 512, 1023, 0, 255));
  } else if (datosControl.ejeY_2 < 512) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, map(datosControl.ejeY_2, 511, 0, 0, 255));
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, map(datosControl.ejeY_2, 511, 0, 0, 255));
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }


    // Control de velocidad PWM
    

    if (datosControl.ejeX_1 > 512) {
    analogWrite(ENA, 255);
    analogWrite(ENB, map(datosControl.ejeX_1, 512, 1023, 0, 255));
  } else if (datosControl.ejeX_1 < 512) {
    analogWrite(ENA, map(datosControl.ejeX_1, 511, 0, 0, 255));
    analogWrite(ENB, 255);
  } else {
    analogWrite(ENA, 255);
    analogWrite(ENB, 255);
  }




    Serial.println(datosControl.ejeX_1);

    // Control del solENoide
    if (datosControl.sol_signal == 1) {
      digitalWrite(solenoidPIN, HIGH);
    } else {
      digitalWrite(solenoidPIN, LOW);
    }

  }
}