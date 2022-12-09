#include <setjmp.h>
#include <string.h>

static void dummy_function(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j){

}

static int getfp(){
    int arg;
    return ((int)(&arg + _JBFP));
}

int setjmp(jmp_buf env){
    register int a = 0, b = 0, c = 0, d = 0, e = 0;
    register int f = 0, g = 0, h = 0, i = 0, j = 0;
    if (a){
        dummy_function(a, b, c, d, e, f, g, h, i, j); /* dummy function called to outsmart optimizers*/
    }
    env[1] = getfp();
    memcpy((char*)&env[2], (char*)env[1] + _JBOFF, _JBMOV);
    return 0;
}