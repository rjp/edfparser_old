#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *edf_pin;
FILE *fh;

#include "edf.h"
#include "y.tab.h"

int edf_pwrap(void) { return 1; } 

struct EDFElement *edftree;

int main(int argc, char *argv[])
{
	int res;
	int testing = 0; /* are we running tests? */

	if (argc>1 && !strncmp(argv[1], "--testing", 9)) {
		testing = 1;
	}

	edf_init();
	edftree=newEDFElement("_top", "", EDF_BYTE);
	edf_pin=stdin;
	res = edf_pparse();

	if (testing) {	
		treeToEDF(edftree); 
	} else {
		edf_statistics();
		{
			char *m;
			
			if (edftree->children[0]) {
				m = getChild(edftree->children[0], "banner");
				if (m) {
					puts(m);
					puts("Login: ");
				}
			}
		}
		{ /* look for my user entry */
			edf *m = (edf*)getMatchingChild(edftree->children[0], "name", "phaedrus");
			if (m) treeToEDF(m->parent);
		}
	}

	return 0;
}

