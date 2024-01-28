#include <stdlib.h>
#include <types.h>

uint32_t _Randseed = 1;

int rand() {
  _Randseed = _Randseed * 1103515245 + 12345;
  return (unsigned int)(_Randseed >> 16) & RAND_MAX;
}