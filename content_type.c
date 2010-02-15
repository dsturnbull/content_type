#include "ruby.h"
#include <magic.h>
#include <stdio.h>
#include <sys/stat.h>

#define MAGIC_OPTIONS MAGIC_SYMLINK | MAGIC_MIME_TYPE | MAGIC_PRESERVE_ATIME

VALUE content_type = Qnil;
VALUE content_type_initialize(VALUE self, VALUE path);
VALUE content_type_content_type(VALUE self);

void magic_fail(const char *error);

void
Init_content_type()
{
    // class definition
	content_type = rb_define_class("ContentType", rb_cObject);

    // instance methods
	rb_define_method(content_type, "initialize",   content_type_initialize, 1);
	rb_define_method(content_type, "content_type", content_type_content_type, 0);

    // instance attributes
    rb_define_attr(content_type, "filepath", 1, 0);
}

VALUE
content_type_initialize(VALUE self, VALUE path)
{
    struct stat st;

    SafeStringValue(path);

    if ((stat(RSTRING_PTR(path), &st)) != 0)
        rb_raise(rb_const_get(rb_cObject, rb_intern("ArgumentError")),
                "invalid file");

    rb_iv_set(self, "@filepath", path);

    return self;
}

VALUE
content_type_content_type(VALUE self)
{
    VALUE            str;
    struct magic_set *mh;
    const char       *mime;

    if (!(mh = magic_open(MAGIC_OPTIONS)))
        magic_fail("open");

    if ((magic_load(mh, NULL)) != 0)
        magic_fail("load");

    if (!(mime = magic_file(mh, RSTRING_PTR(rb_iv_get(self, "@filepath")))))
        magic_fail("file");

    str = rb_str_new(mime, strlen(mime));

    magic_close(mh);

    return str;
}

void
magic_fail(const char *error)
{
    const char  *format = "magic_%s() error";
    char        *error_message;

    error_message = malloc(sizeof(char *) * (strlen(format) + strlen(error)));
    sprintf((char *)error_message, format, error);
    rb_raise(rb_const_get(rb_cObject, rb_intern("RuntimeError")), error_message);
}
