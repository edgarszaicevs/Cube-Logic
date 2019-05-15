#ifndef ACCEL_H
#define ACCEL_H

#include <MPU6050.h>

static float KaPc = 0.0, KaG = 0.0, KaP = 1.0, KaXp = 0.0, KaZp = 0.0, KaXe = 0.0;

float kalmanFilter(float val) {
  KaPc = KaP + varProcess;
  KaG = KaPc / (KaPc + varVolt);
  KaP = (1 - KaG) * KaPc;
  KaXp = KaXe;
  KaZp = KaXp;
  KaXe = KaG * (val - KaZp) + KaXp;
  return (KaXe);
}

float mapToFloat(float x, float a, float b, float c, float d) {
  float f = x/(b-a)*(d-c)+c;
  return f;
}

#endif
