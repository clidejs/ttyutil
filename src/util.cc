#include <ttyu.h>

int util_rgbi2term(float r, float g, float b) {
  return r / 255 * 5 * 36 + g / 255 * 5 * 6 + b / 255 * 5 + 16;
}

const char *util_render(const char *ch, int fg, int bg) {
  char *out = (char *)malloc(sizeof(char) * (strlen(ch) + 30));
  if(fg != -1 && bg != -1) {
    sprintf(out, "\x1b[38;5;%dm\x1b[48;5;%dm%s\x1b[49m\x1b[39m", fg, bg, ch);
  } else if(fg != -1) {
    sprintf(out, "\x1b[38;5;%dm%s\x1b[39m", fg, ch);
  } else if(bg != -1) {
    sprintf(out, "\x1b[48;5;%dm%s\x1b[49m", bg, ch);
  } else {
    delete out;
    return ch;
  }
  return const_cast<const char *>(out);
}

int util_parse_hex(char h) {
  if(h >= '0' && h <= '9') {
    return h - '0';
  } else if(h >= 'A' && h <= 'F') {
    return h - 'A' + 10;
  } else if(h >= 'a' && h <= 'f') {
    return h - 'a' + 10;
  }
  return 0;
}

int util_rgb2term(const char *rgb) {
  int r = 0;
  int g = 0;
  int b = 0;

  // TODO

  return util_rgbi2term(r, g, b);
}

int util_hex2term(const char *hex) {
  int r = 0;
  int g = 0;
  int b = 0;
  if(strlen(hex) == 6) {
    r = util_parse_hex(hex[0]) * 16 + util_parse_hex(hex[1]);
    g = util_parse_hex(hex[2]) * 16 + util_parse_hex(hex[3]);
    b = util_parse_hex(hex[4]) * 16 + util_parse_hex(hex[5]);
  } else if(strlen(hex) == 3) {
    r = util_parse_hex(hex[0]);
    g = util_parse_hex(hex[1]);
    b = util_parse_hex(hex[2]);
    r += r*16;
    g += g*16;
    b += b*16;
  }
  return util_rgbi2term(r, g, b);
}

int util_color(const char *c) {
  if(c[0] == '#') {
    return util_hex2term(&c[1]);
  // TODO:
  //} else if(c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == '(') {
  //  return util_rgb2term(&c[4]);
  } else {
    return util_hex2term(c);
  }
}

bool util_max(int a, int b) {
  return (a>b)?a:b;
}

bool util_min(int a, int b) {
  return (a>b)?b:a;
}
