#ifndef TTYU_UTIL_H_
#define TTYU_UTIL_H_

int util_rgbi2term(float r, float g, float b);
const char *util_render(const char *ch, int fg, int bg);
int util_parse_hex(char h);
int util_rgb2term(const char *rgb);
int util_hex2term(const char *hex);
int util_color(const char *c);
bool util_max(int a, int b);
bool util_min(int a, int b);

#endif // TTYUTIL_UTIL_H_
