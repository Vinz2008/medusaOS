#include <stdlib.h>

div_t div(int num, int denom) {
  div_t val;
  val.quot = num / denom;
  val.rem = num - denom * val.quot;
  if (val.quot < 0 && 0 < val.rem) {
    val.quot += 1;
    val.rem -= denom;
  }
  return val;
}