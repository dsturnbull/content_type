#include "ruby.h"
#include <magic.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdbool.h>

// see libmagic(3)
#define MAGIC_OPTIONS MAGIC_SYMLINK | MAGIC_MIME_TYPE | MAGIC_PRESERVE_ATIME

// presume file extensions are at max 16 chars. this is fine for now since
// only microsoft crap is overridden.
#define MAX_EXT_LEN 16

// ContentType prototypes
VALUE content_type = Qnil;
VALUE content_type_initialize(VALUE self, VALUE path);
VALUE content_type_content_type(VALUE self);

// File prototypes
VALUE file_content_type_wrap(VALUE self, VALUE path);
VALUE file_content_type(VALUE self);
VALUE file_singleton_content_type(VALUE self, VALUE path);

// String prototypes
VALUE string_content_type(VALUE self);

// internal prototypes
bool content_type_file_ext(VALUE self, char *ext);
void magic_fail(const char *error);

// http://www.webdeveloper.com/forum/showthread.php?t=162526
const char *content_type_ext_overrides[][2] = {
	{ "docm", "application/vnd.ms-word.document.macroEnabled.12" },
	{ "docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document" },
	{ "dotm", "application/vnd.ms-word.template.macroEnabled.12" },
	{ "dotx", "application/vnd.openxmlformats-officedocument.wordprocessingml.template" },
	{ "potm", "application/vnd.ms-powerpoint.template.macroEnabled.12" },
	{ "potx", "application/vnd.openxmlformats-officedocument.presentationml.template" },
	{ "ppam", "application/vnd.ms-powerpoint.addin.macroEnabled.12" },
	{ "ppsm", "application/vnd.ms-powerpoint.slideshow.macroEnabled.12" },
	{ "ppsx", "application/vnd.openxmlformats-officedocument.presentationml.slideshow" },
	{ "pptm", "application/vnd.ms-powerpoint.presentation.macroEnabled.12" },
	{ "pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation" },
	{ "xlam", "application/vnd.ms-excel.addin.macroEnabled.12" },
	{ "xlsb", "application/vnd.ms-excel.sheet.binary.macroEnabled.12" },
	{ "xlsm", "application/vnd.ms-excel.sheet.macroEnabled.12" },
	{ "xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" },
	{ "xltm", "application/vnd.ms-excel.template.macroEnabled.12" },
	{ "xltx", "application/vnd.openxmlformats-officedocument.spreadsheetml.template" },
};

// Init_content_type - initialize the extension
void
Init_content_type()
{
	// class definition
	content_type = rb_define_class("ContentType", rb_cObject);

	// instance methods
	rb_define_method(content_type, "initialize",   content_type_initialize, 1);
	rb_define_method(content_type, "content_type", content_type_content_type, 0);

	// instance attributes
	rb_define_attr(content_type, "filepath",	1, 0);
	rb_define_attr(content_type, "processed",   1, 0);

	// hax on File
	rb_define_method(rb_cFile, "content_type", file_content_type, 0);
	rb_define_singleton_method(rb_cFile,
			"content_type", file_singleton_content_type, 1);

	// hax on String
	rb_define_method(rb_cString, "content_type", string_content_type, 0);
}

// content_type_initialize - initialize the object and check file path exists
//
// iv @content_type, @filepath and @processed are available in all variations
// because each implementation creates a ContentType obj through here
// (except String#content_type)
VALUE
content_type_initialize(VALUE self, VALUE path)
{
	struct stat st;

	SafeStringValue(path);

	if ((stat(RSTRING_PTR(path), &st)) != 0)
		rb_raise(rb_const_get(rb_cObject, rb_intern("ArgumentError")),
				"invalid file");

	rb_iv_set(self, "@content_type",	rb_str_new("", 0));
	rb_iv_set(self, "@filepath",		path);
	rb_iv_set(self, "@processed",		Qfalse);

	return self;
}

// content_type_file_ext - return the file extension of the file
bool
content_type_file_ext(VALUE self, char *ext)
{
	char	*filepath;
	char	t;
	long	i;
	int		j, k;

	filepath = RSTRING_PTR(rb_iv_get(self, "@filepath"));

	// go back until the first . from the end of the string
	// then reverse the array
	j = 0;
	for (i = RSTRING_LEN(rb_iv_get(self, "@filepath")) - 1;
			i > 0 && j < MAX_EXT_LEN; i--) {
		if (filepath[i] == '.') {
			for (k = 0; k < j/2 ; k++) {
				t = ext[j - 1 - k];
				ext[j - 1 - k] = ext[k];
				ext[k] = t;
			}
			return true;
		}
		ext[j] = filepath[i];
		j++;
	}

	return false;
}

// content_type_content_type - check for hardcoded extensions or use libmagic
VALUE
content_type_content_type(VALUE self)
{
	VALUE				ct;
	struct magic_set	*mh;
	const char			*mime;
	char				*o_ext, *o_mime;
	char				ext[MAX_EXT_LEN];	// TODO dynamicly sized
	int					i;

	// check for hardcoded extension overrides first
	if (content_type_file_ext(self, ext))
		for (i = sizeof(content_type_ext_overrides) / sizeof(char *) / 2 - 1;
				i >= 0; i--) {

			o_ext	= (char *)content_type_ext_overrides[i][0];
			o_mime	= (char *)content_type_ext_overrides[i][1];

			if ((memcmp(ext, o_ext, strlen(o_ext))) == 0) {
				ct = rb_str_new2(o_mime);
				// memoize
				rb_iv_set(self, "@content_type", ct);
				rb_iv_set(self, "@processed", Qtrue);
				return ct;
			}
		}

	// otherwise use libmagic to get the mime type
	if (rb_iv_get(self, "@processed"))
		return rb_iv_get(self, "@content_type");

	if (!(mh = magic_open(MAGIC_OPTIONS)))
		magic_fail("open");

	if ((magic_load(mh, NULL)) != 0)
		magic_fail("load");

	if (!(mime = magic_file(mh, RSTRING_PTR(rb_iv_get(self, "@filepath")))))
		magic_fail("file");

	ct = rb_str_new(mime, strlen(mime));

	// memoize
	rb_iv_set(self, "@content_type", ct);
	rb_iv_set(self, "@processed", Qtrue);
	magic_close(mh);

	return ct;
}

// file_content_type_wrap - pass file path into ContentType#content_type
VALUE
file_content_type_wrap(VALUE self, VALUE path)
{
	VALUE ct, args[1];

	(void)self;
	SafeStringValue(path);
	args[0] = path;
	ct = rb_class_new_instance(1, args, content_type);

	return rb_funcall(ct, rb_intern("content_type"), 0);
}

// file_content_type - provide File#content_type
VALUE
file_content_type(VALUE self)
{
	return file_content_type_wrap(self, rb_funcall(self, rb_intern("path"), 0));
}

// file_singleton_content_type - provide File.content_type
VALUE
file_singleton_content_type(VALUE self, VALUE path)
{
	return file_content_type_wrap(self, path);
}

// string_content_type - pass the string as a buffer to libmagic
VALUE
string_content_type(VALUE self)
{
	struct magic_set	*mh;
	const char			*mime;
	VALUE				str;
	VALUE				ct;

	str = rb_funcall(self, rb_intern("to_s"), 0);

	if (!(mh = magic_open(MAGIC_OPTIONS)))
		magic_fail("open");

	if ((magic_load(mh, NULL)) != 0)
		magic_fail("load");

	if (!(mime = magic_buffer(mh, RSTRING_PTR(str), RSTRING_LEN(str))))
		magic_fail("buffer");

	ct = rb_str_new(mime, strlen(mime));
	magic_close(mh);

	return ct;
}

// magic_fail - raise exceptions
void
magic_fail(const char *error)
{
	const char  *format = "magic_%s() error";
	char		*error_message;

	error_message = malloc(sizeof(char *) * (strlen(format) + strlen(error)));
	sprintf((char *)error_message, format, error);
	rb_raise(rb_const_get(rb_cObject, rb_intern("RuntimeError")), "%s", error_message);
}

