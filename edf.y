%token OPEN CLOSE SLASH WORD EQUALS STRING SPACE NUMBER
%{
#include "edf.h"
extern edf *edftree;
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

extern void debug(char *, ...);
extern char *edf_ptext;
extern int indent, tags;
extern void yyterminate(void);
extern char *strpool(char *);

%}

%%
// EDF contains a valid tree
edf
: trees {}
;
// we can have multiple trees, unfortunately
trees
: trees tree {}
| tree {}
;
// a close tag, either </> or </word>
closetag 
: OPEN SLASH CLOSE { 
	debug("null closing [%s] to [%s]", edftree->key, edftree->parent->key);
	edftree->null_close = 1;
	edftree = edftree->parent; 
}
| OPEN SLASH word CLOSE { 
	debug("closing [%s] to [%s]", edftree->key, edftree->parent->key);
	/* should check we're closing the same tag */
	edftree= edftree->parent; 
}
;
// simple tag that doesn't contain any others
node
: OPEN word SLASH CLOSE {
	debug("sftag <%s=1/>", $2); 
	$$ = (int)newEDFElement_flag((char*)$2);
}
| OPEN word EQUALS number SLASH CLOSE { 
	debug("sntag <%s=%d/>", $2, $4); 
	$$ = (int)newEDFElement_int((char*)$2, (int)$4);
}
| OPEN word EQUALS string SLASH CLOSE { 
	debug("sstag <\"%s=%s\"/>", $2, $4); 
	$$ = (int)newEDFElement((char*)$2, (char*)$4, EDF_BYTE);
}
;
opentag
: OPEN word CLOSE { 
	edf *tmp;
	debug("oftag: <%s=1>", $2); 
	tmp = (edf*)newEDFElement_flag((char*)$2);
	tmp->parent = edftree; edftree = tmp;	
	$$ = (int)tmp;
}
| OPEN word EQUALS number CLOSE { 
	edf *tmp;
	debug("ontag <%s=%d>", $2, $4); 
	tmp = (edf*)newEDFElement_int((char*)$2, (int)$4);
	tmp->parent = edftree; edftree = tmp;	
	$$ = (int)tmp;
}
| OPEN word EQUALS string CLOSE { 
	edf *tmp;
	debug("ostag <%s=\"%s\">, child of <%s>", $2, $4, edftree->key);
	tmp = newEDFElement((char*)$2, (char*)$4, EDF_BYTE);
	tmp->parent = edftree; edftree = tmp;	
	$$ = (int)tmp;
}	
;
// a tree opens, contains some tags, then closes
tree
: opentag trees closetag {
	debug("TREE p=%p, k=%s", $1, ((struct EDFElement *)$1)->key); 
	addChild(edftree, (edf*)$1);
}
| opentag closetag {
	debug("EMPTY TREE p=%p, k=%s", $1, ((struct EDFElement *)$1)->key);
	addChild(edftree, (edf*)$1);
	((struct EDFElement *)$1)->null_tree = 1;
}
| node { 
	debug("NODE p=%p, k=%s", $1, ((struct EDFElement *)$1)->key); 
	addChild(edftree, (edf*)$1);
}
;
number : NUMBER { $$ = atoi(edf_ptext); } ;
word : WORD { $$ = (int)strpool(edf_ptext); } ;
string : STRING { $$ = (int)strpool(edf_ptext); } ;

%%
#if 0
char *process_string(char *a)
{
	char *b=a, *c=a;
	b++; /* ignoring the leading quote */
	while (*b) {
		if (*b == '\\') {
			b++;
		}
		*c++=*b++;
	}
	*--c='\0'; /* and trim the trailing quote */
	return a;
}
#endif

/* insert a string into our pool and return the pointer */
char *
strpool (char *s)
{
#if defined(POOL)
 	return edf_pool_string(s);
#else
	return strdup(s); 
#endif
}

void
debug (char *format, ...)
{
#if defined(DEBUG)
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	puts("");
#endif 
}
