#ifndef TTYU_UTIL_H_
#define TTYU_UTIL_H_

#include <string.h>
#include <iostream>

#define EXPORT_PROTOTYPE_METHOD NODE_SET_PROTOTYPE_METHOD
#define EXPORT_PROTOTYPE_METHOD_HIDDEN(tpl, name, cb) do {                     \
  v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(cb);        \
  tpl->InstanceTemplate()->Set(                                                \
      NanNew<v8::String>(name), t->GetFunction(), v8::ReadOnly);               \
} while(0)

#define EXPORT_PROTOTYPE_GET(tpl, name, fn)                                    \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (fn))

#define EXPORT_PROTOTYPE_GETSET(tpl, name, get, set)                           \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (get), (set))

#define WIN_COLORS 16

short util_rgbi2term(short r, short g, short b);
short util_rgbi2win(short r, short g, short b);
char *util_render(const char *ch, short fg, short bg);
short util_parse_dec(char d);
short util_parse_hex(char h);
unsigned long util_term2argb(short t);
short util_rgb2term(const char *rgb);
short util_hex2term(const char *hex);
short util_color(const char *c);
int util_max(int a, int b);
int util_min(int a, int b);
int util_abs(int a);
char *util_error(char *name, int id);

#undef ERROR
#define ERROR(name, id) util_error(name, id)
#define DBG(a) std::cout << (a) << std::endl

#endif // TTYUTIL_UTIL_H_
