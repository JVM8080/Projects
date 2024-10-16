#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Pines del joystick
#define VRX_1 12 // Joystick 1 X
#define VRY_2 14 // Joystick 2 Y
#define SOLENOID 27 // Joystick push 2 


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
  pinMode(VRX_1, INPUT);
  pinMode(SOLENOID, INPUT);
  pinMode(VRY_2, INPUT);
  radio.begin();
  if (!radio.begin()) {
    Serial.println("No se pudo iniciar el módulo RF");
    while (1);
  }
  radio.openWritingPipe(direccion);
  //radio.setPALevel(RF24_PA_LOW);
}

void loop() {
  datosControl.ejeX_1 = analogRead(VRX_1);
  datosControl.sol_signal = digitalRead(SOLENOID);
  datosControl.ejeY_2 = analogRead(VRY_2);
  Serial.println(datosControl.ejeX_1);
  radio.write(&datosControl, sizeof(datosControl));
  delay(50);
}
