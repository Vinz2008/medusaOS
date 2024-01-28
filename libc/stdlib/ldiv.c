#include <stdlib.h>

ldiv_t ldiv(long num, long denom) {
  ldiv_t val;
  val.quot = num / denom;
  val.rem = num - denom * val.quot;
  if (val.quot < 0 && 0 < val.rem) {
    val.quot += 1;
    val.rem -= denom;
  }
  return val;
}