#include "edf.h"
#include "ruby.h"

struct EDFElement *edftree;
int edf_pwrap(void) { return 1; }

static VALUE t_parse(VALUE self, VALUE edf)
{
	edf_parse("<edf=\"on\"/>");
}

static VALUE t_init(VALUE self)
{
	VALUE arr;
	arr = rb_ary_new();
	rb_iv_set(self, "@arr", arr);
	return self;
}

static VALUE t_add(VALUE self, VALUE anObject)
{
	VALUE arr;
	arr = rb_iv_get(self, "@arr");
	rb_ary_push(arr, anObject);
	return arr;
}

VALUE cEDF;

void Init_EDF(void) {
	cEDF = rb_define_class("EDF", rb_cObject);
	rb_define_method(cEDF, "initialize", t_init, 0);
	rb_define_method(cEDF, "add", t_add, 1);
	rb_define_method(cEDF, "parse", t_parse, 1);
	edf_init();
	edftree = newEDFElement("_top","",EDF_BYTE);
}
