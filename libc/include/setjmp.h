#ifndef _SETJMP_H
#define _SETJMP_H 1

#define _JBFP 1
#define _JBMOV 60
#define _JBOFF 4
#define _NSETJMP 17

#define setjmp(env) _Setjmp(env)
typedef int jmp_buf[_NSETJMP];
void longjmp(jmp_buf env, int value);

#endif