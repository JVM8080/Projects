#include <Arduino.h>
#include <time.h>

void setup() {
  Serial.begin(9600);

  delay(random(0,20));
  
  randomSeed(millis());
}


void loop() {
  int a, b;
  a = random(10,700);
  b = random(10,700);

  String s = String(a) + "," + String(b);

  Serial.println(s);
  delay(100);
}