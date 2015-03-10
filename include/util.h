#ifndef TTYU_UTIL_H_
#define TTYU_UTIL_H_

#include <string.h>
#include <iostream>

#define EXPORT_PROTOTYPE_METHOD NODE_SET_PROTOTYPE_METHOD

#define EXPORT_PROTOTYPE_GET(tpl, name, fn)                                    \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (fn))

#define EXPORT_PROTOTYPE_GETSET(tpl, name, get, set)                           \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (get), (set))

int util_rgbi2term(float r, float g, float b);
char *util_render(const char *ch, int fg, int bg);
int util_parse_dec(char d);
int util_parse_hex(char h);
unsigned long util_term2argb(int t);
int util_rgb2term(const char *rgb);
int util_hex2term(const char *hex);
int util_color(const char *c);
bool util_max(int a, int b);
bool util_min(int a, int b);
char *util_error(char *name, int id);

#undef ERROR
#define ERROR(name, id) util_error(name, id)

#ifdef DEBUG
# define DBG(a) std::cout << a << std::endl
#else
# define DBG(a)
#endif

#endif // TTYUTIL_UTIL_H_
