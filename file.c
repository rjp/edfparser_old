#include <stdio.h>
#include <stdlib.h>
extern FILE *edf_pin;

#include "edf.h"
#include "y.tab.h"

struct EDFElement *edftree;
FILE *infile;

FILE *fh;
char buffer[16384];

int
edf_pwrap (void) 
{ 
		return feof(fh);
}

int main(void)
{
	int res;

	edftree=newEDFElement("_top", "", EDF_BYTE);

	fh = fopen("test.edf", "r");
	if (!fh) { perror("open"); exit(1); }

	while (res) {
		res = edf_pparse();
	}

	fclose(fh);
#if defined(TEST_VERSION)
	treeToEDF(edftree); 
#else
	{
		char *m = getChild(edftree->children[0], "banner");
		if (m) {
			puts(m);
			puts("Login: ");
		}
	}
	{ /* look for my user entry */
		edf *m = (edf*)getMatchingChild(edftree->children[0], "name", "phaedrus");
		if (m) treeToEDF(m->parent);
	}
#endif

	return 0;
}

