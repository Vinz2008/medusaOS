#ifndef _LOCALE_H
#define _LOCALE_H 1

#define LC_ALL 0
#define LC_COLLATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5
#define _NCAT 6

struct lvconv {
  char* currency_symbol;
  char* int_curr_symbol;
  char* mon_decimal_symbol;
  char* mon_grouping;
  char* mon_thousands_sep;
  char* negative_sign;
  char* positive_sign;
  char frac_digits;
  char int_frac_digits;
  char n_cs_precedes;
  char n_sep_by_space;
  char n_sign_posn;
  char* decimal_point;
  char* grouping;
  char* thousands_sep;
};

struct lvconv* localeconv(void);
char* setlocale(int category, const char* locale); // not implementated yet
extern struct lvconv _Locale;
#define localeconv() (&_Locale)

#endif