

#include <stdio.h>

#ifdef NDEBUG
#define assert(ignore) ((void 0))

#else
void __assert_fail(const char* assertion, int expression, const char* file, int line);
#define assert(ignore) __assert_fail(#ignore, ignore, __FILE__, __LINE__)
//#define assert2(expression) ((expression) ? ((void 0)) : __assert_fail(#express))
#endif