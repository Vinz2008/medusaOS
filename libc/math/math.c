#include <math.h>

int max(int a, int b) { return a > b ? a : b; }

int min(int a, int b) { return a < b ? a : b; }

double pow(double x, double y) {
  double result = 1.0;
  while (y != 0) {
    result *= x;
    --y;
  }
  return result;
}

double sqrt(double x) {
  union {
    int i;
    float x;
  } u;

  u.x = x;
  u.i = (1 << 29) + (u.i >> 1) - (1 << 22);
  return u.x;
}