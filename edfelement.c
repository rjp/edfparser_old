#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include "edf.h"

char *edf_types[] = {
	"string", "integer", "flag", "tree", NULL
};

struct edf_counters {
	int nodes;
	int childnodes;
	int types[EDF_LAST];
} edf_stat_count, *counters=&edf_stat_count;

/* room for 64k nodes */
static struct EDFElement *_heap;
static int _heapcount;
static char *_pool, *_pool_ptr;

extern struct EDFElement *edftree;


void
edf_heap_init(int size)
{
	_heap = (struct EDFElement *)malloc(sizeof(struct EDFElement)*size);
	_heapcount = 0;
}

void
edf_pool_init(int size)
{
	_pool = (char*)malloc(sizeof(char) * size);
	_pool_ptr = _pool;
}

void
edf_init(void)
{
	int i;

	if (_heap == NULL) {
		edf_heap_init(EDF_HEAP_SIZE);
	}
	_heapcount = 0;
	if (_pool == NULL) {
		edf_pool_init(EDF_POOL_SIZE);
	}
	_pool_ptr = _pool;

	for (i=0; i<EDF_LAST; i++) {
		counters->types[i] = 0;
	}
	counters->nodes = 0;
	counters->childnodes = 0;
}

struct EDFElement *
edf_alloc(void)
{
	return &_heap[_heapcount++];
}

void
edf_statistics (void)
{
	int i;
	printf("%d cells allocated\n", _heapcount);
	printf("%d nodes created, %d child nodes\n",
			counters->nodes, counters->childnodes);
	for (i=0; i<EDF_LAST; i++) {
		printf("%-10s: %d\n", edf_types[i], counters->types[i]);
	}
}

void
edf_parse (char *s)
{
	edf_init();
	edf_p_scan_string(s);
	edf_pparse();
	printf("edftree has %d children, and %s=%s\n",
					edftree->numChildren, edftree->key, edftree->value.s);
/*	treeToEDF(edftree); */
}

/* utility routine to deconvert backslashed characters */
char *process_string(char *a)
{
	char *b=a, *c=a;

	/* trim the leading quote */
	if (*b=='"') { b++; }

	while (*b) {
		if (*b == '\\') {
			b++;
		}
		*c++=*b++;
	}

	*c='\0';
	c--;

	/* trim the trailing quote  */
	if (*c=='"') { 
		*c='\0'; 
	}
	return a;
}

void
addChild(struct EDFElement *parent, struct EDFElement *child)
{
	parent->children[parent->numChildren] = child;
	parent->numChildren++;	
	child->parent = parent;
	counters->childnodes++;
}

struct EDFElement *
newEDFElement (char *key, char *value, int type)
{
	struct EDFElement *tmp;
	tmp = edf_alloc();
/*	(struct EDFElement *)malloc(sizeof(struct EDFElement)); */
	tmp->key = strdup(key);
	tmp->type = type;
	tmp->numChildren=0;
	if (type == EDF_BYTE) {
		tmp->value.s = process_string(strdup(value));
	} else {
		tmp->value.i = atoi(value);
	}
	counters->nodes++;
	counters->types[type]++;
	return tmp;
}

struct EDFElement *
newEDFElement_flag (char *key)
{
	struct EDFElement *tmp;
	tmp = edf_alloc();
	tmp->key = strdup(key);
	tmp->type = EDF_FLAG;
	tmp->numChildren=0;
	counters->nodes++;
	counters->types[EDF_FLAG]++;
	return tmp;
}

struct EDFElement *
newEDFElement_int (char *key, int val)
{
	struct EDFElement *tmp;
	tmp = edf_alloc();
	tmp->key = strdup(key);
	tmp->type = EDF_INTEGER;
	tmp->numChildren=0;
	tmp->value.i = val;
	counters->nodes++;
	counters->types[EDF_INTEGER]++;
	return tmp;
}

void
treeToEDF(struct EDFElement *a)
{
	int i = a->numChildren;
	printf("<%s", a->key);
	switch (a->type) {
		case EDF_BYTE: printf("=\"%s\"", a->value.s);  break;
		case EDF_FLAG: break;
		case EDF_INTEGER: printf("=%d", a->value.i); break;
	}
	if (a->numChildren || a->null_tree) {
		printf(">");
	} else {
		printf("/>");
	}

	if (i > 0) {
		int j;
		for (j=0; j<i; j++) {
			treeToEDF(a->children[j]);
		}
	}
	if (a->numChildren || a->null_tree) {
		if (a->null_close) {
			printf("</>");
		} else {
			printf("</%s>", a->key);
		}
	}
}

char *
getChild(struct EDFElement *t, char *key)
{
	int i;
	
   	/* If we get sent a NULL tree, we don't have any children */	
	if (!t) {
		return NULL;
	}

	for (i=0; i<t->numChildren; i++) {
#if defined(DEBUG)
		printf("--checking [%s] [%s]\n", t->children[i]->key, key);
#endif
		if (!strcasecmp(t->children[i]->key, key))
			return t->children[i]->value.s;
	}
	return NULL;
}

struct EDFElement *
getMatchingChild(struct EDFElement *t, char *k, char *v)
{
	int i;

	if (!t) {
		return NULL;
	}

	if (!strcasecmp(t->key, k) && !strcasecmp(t->value.s, v))
		return t;

	for (i=0; i<t->numChildren; i++) {
		edf *ret;
#if defined(DEBUG)
		printf("--checking [%s] [%s]\n", t->children[i]->key, k);
#endif
		if ((ret = getMatchingChild(t->children[i], k, v))) {
			return ret;
		}
	}	
	return NULL;
}

#if defined(TEST)
int main (void)
{
	struct EDFElement *a, *b, *c;

	a=newEDFElement("reply", "user_login", EDF_BYTE);
	b=newEDFElement("username", "lara croft", EDF_BYTE);
	c=newEDFElement("real", "Lara", EDF_BYTE);
	addChild(a, b);
	addChild(b, c);
	treeToEDF(a);
}
#endif

char *
edf_pool_string(char *s)
{
	char *t = _pool_ptr;
	strcpy(_pool_ptr, s);
	_pool_ptr += strlen(s) + 1;
	debug("before %p, after %p, s=[%s]\n", t, _pool_ptr, s);
	return t;
}
