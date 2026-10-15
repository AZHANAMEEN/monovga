#include "CONTROLLER.h"

const int nesData  = PIN_PD2;
const int nesClock = PIN_PD1;
const int nesLatch = PIN_PD0;

bool u = true;
bool d = true;
bool l = true;
bool r = true;
bool a = true;
bool b = true;

byte readNesController() {
  byte tempData = 255;
  digitalWrite(nesLatch, HIGH);
  digitalWrite(nesLatch, LOW);
  for (int i = 0; i < 8; i++) {
    if (digitalRead(nesData) == LOW) {
      bitClear(tempData, i);
    }
    digitalWrite(nesClock, HIGH);
    digitalWrite(nesClock, LOW);
  }
  return tempData;
}

void setupcont(void) {
  pinMode(nesData, INPUT_PULLUP);
  pinMode(nesClock, OUTPUT);
  pinMode(nesLatch, OUTPUT);
  digitalWrite(nesClock, LOW);
  digitalWrite(nesLatch, LOW);
  u = true;
  d = true;
  l = true;
  r = true;
}

void loopcont(void) {
  byte nesRegister = readNesController();
  a = (bitRead(nesRegister, 0) == 1);
  b = (bitRead(nesRegister, 1) == 1);
  u = (bitRead(nesRegister, 4) == 1);
  d = (bitRead(nesRegister, 5) == 1);
  l = (bitRead(nesRegister, 6) == 1);
  r = (bitRead(nesRegister, 7) == 1);
}