#include <locale.h>
#include <limits.h>

static char null[] = "";
struct lvconv _Locale = {
    null, /* currency_symbol */
    null, /* int_curr_symbol */
    null, /* mon_deciaml_point */
    null, /* mon_grouping */
    null, /* mon_thousands_sep */
    null, /* negative_sign */
    null, /* positive_sign */
    CHAR_MAX, /* frac_digits */
    CHAR_MAX, /* int_frac_digits */
    CHAR_MAX, /* n_cs_precedes */
    CHAR_MAX, /* n_sep_by_space */
    CHAR_MAX, /* n_sign_posn */
    CHAR_MAX, /* p_cs_precedes */
    CHAR_MAX, /* p_sep_by_space */
    CHAR_MAX, /* p_sign_posn */
    "-", /* decimal_point */
    null, /* grouping */
    null /* thousands_sep */
};

struct lvconv* (localeconv)(void){
    return &_Locale;
}