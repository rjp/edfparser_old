%{
#if defined(FILE_TEST)
#include <stdio.h>
extern FILE *fh;
#define YY_INPUT(buf,result,max_size) \
  result = fread(buf,sizeof(char),16,fh); \
  buf[result]='\0'; puts(buf);
#endif
/* YY_BUFFER_STATE edf_p_scan_string(char *); */

#include <y.tab.h>
#include <stdio.h>
#include <stdlib.h>
extern char *edf_ptext;
int tags, indent=0;
%}

DIGIT [0-9]
WCHAR [-a-zA-Z]
OTAG [<]
CTAG [>]
SLASH [/]
EQUALS [=]
ESCAPED \\.
QUOTE ["]
NOTQUOTE [^"]

%%
{DIGIT}+	{ return NUMBER; }
{OTAG}		{ if (tags && indent==0) { yyterminate(); } return OPEN; }
{CTAG}		{ return CLOSE; }
{SLASH}		{ return SLASH; }
{EQUALS}	{ return EQUALS; }
{QUOTE}({NOTQUOTE}|{ESCAPED})+{QUOTE} { return STRING; }
{WCHAR}+	{ return WORD; }

%%
#include "edf.h"

/* int edf_p_wrap(void) { return 1; } */
int edf_perror(char *x) { return 0; } /* puts(x); exit(1); } */

void
edf_string(char *s)
{
	edf_p_scan_string(s);
	edf_pparse();
}
