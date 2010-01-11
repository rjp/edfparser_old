#if !defined(__EDF_H)
#define __EDF_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { EDF_BYTE, EDF_INTEGER, EDF_FLAG, EDF_TREE, EDF_LAST };
extern char *edf_types[];


typedef union {
	int i;
	char *s;
} EDFValue;

struct EDFElement {
	char *key;
	EDFValue value;
	int type;
	int null_tree;
	int null_close;
	/* internal */
	struct EDFElement *children[1024];
	struct EDFElement *parent;
	int numChildren;
};
typedef struct EDFElement edf;

extern void _addChild(struct EDFElement *, struct EDFElement *);
extern struct EDFElement *newEDFElement(char *, char *, int);
extern struct EDFElement *newEDFElement_flag(char *);
extern struct EDFElement *newEDFElement_int(char *, int);
extern char *getChild(struct EDFElement *, char *);

#if 0
#define addChild printf("addChild %s:%d\n",__FILE__,__LINE__);_addChild
#endif

extern void treeToEDF(struct EDFElement *);
extern void edf_init(void);
extern void edf_statistics(void);
extern void edf_parse(char *);
extern struct EDFElement *getMatchingChild(struct EDFElement *,char *,char *);

extern int yyparse(void);
extern int edf_p_parse(void);
extern int edf_pparse(void);
extern int edf_perror(char *);
extern int edf_plex(void);
extern void addChild(struct EDFElement *, struct EDFElement *);
/* extern void *edf_p_scan_string(char *); */

#define EDF_HEAP_SIZE (8192)
#define EDF_POOL_SIZE (256*1024)

#endif /* __EDF_H */
