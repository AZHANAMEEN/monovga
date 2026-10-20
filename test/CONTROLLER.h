#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

extern bool u;
extern bool d;
extern bool l;
extern bool r;
extern bool x;
extern bool y;
extern bool a;
extern bool b;

void setupcont(void);
void loopcont(void);

#endif