#include <string.h>
#include <stdio.h>

void int_to_ascii_base(int n, char str[], int base){
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    if (base == 16){
        str[i++] = 'x';
        str[i++] = '0';
    }
    str[i] = '\0';

    reverse(str);
}

void int_to_ascii(int n, char str[]){
    int_to_ascii_base(n, str, 10);
}