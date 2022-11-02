enum
{
  _ISupper = 01,	/* UPPERCASE.  */
  _ISlower = 02,	/* lowercase.  */
  _ISdigit = 04,	/* Numeric.  */
  _ISspace = 010,	/* Whitespace.  */
  _ISpunct = 020,	/* Punctuation.  */
  _IScntrl = 040,	/* Control character.  */
  _ISblank = 0100,	/* Blank (usually SPC and TAB).  */
  _ISxdigit = 0200,	/* Hexadecimal numeric.  */
};

extern unsigned char _ctype[];
 
#define CT_UP	0x01	/* upper case */
#define CT_LOW	0x02	/* lower case */
#define CT_DIG	0x04	/* digit */
#define CT_CTL	0x08	/* control */
#define CT_PUN	0x10	/* punctuation */
#define CT_WHT	0x20	/* white space (space/cr/lf/tab) */
#define CT_HEX	0x40	/* hex digit */
#define CT_SP	0x80	/* hard space (0x20) */
 
#define isalnum(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_UP | CT_LOW | CT_DIG))
#define isalpha(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_UP | CT_LOW))
#define iscntrl(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_CTL))
#define isdigit(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_DIG))
#define isgraph(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG))
#define islower(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_LOW))
#define isprint(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_PUN | CT_UP | CT_LOW | CT_DIG | CT_SP))
#define ispunct(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_PUN))
#define isspace(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_WHT))
#define isupper(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_UP))
#define isxdigit(c)	((_ctype + 1)[(unsigned char)(c)] & (CT_DIG | CT_HEX))
#define isascii(c)	((unsigned)(c) <= 0x7F)
#define tolower(c)	(isupper(c) ? c + 'a' - 'A' : c)
#define toupper(c)	(islower(c) ? c + 'A' - 'a' : c)