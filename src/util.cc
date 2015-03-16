#include <ttyu.h>

int util_rgbi2term(float r, float g, float b) {
  return r / 51 * 36 + g / 51 * 6 + b / 51 + 16;
}

char *util_render(const char *ch, int fg, int bg) {
  char *out = (char *)malloc(sizeof(char) * (strlen(ch) + 30));
  if(fg != -1 && bg != -1) {
    sprintf(out, "\x1b[38;5;%dm\x1b[48;5;%dm%s\x1b[49m\x1b[39m", fg, bg, ch);
  } else if(fg != -1) {
    sprintf(out, "\x1b[38;5;%dm%s\x1b[39m", fg, ch);
  } else if(bg != -1) {
    sprintf(out, "\x1b[48;5;%dm%s\x1b[49m", bg, ch);
  } else {
    sprintf(out, "%s", ch);
  }
  return out;
}

int util_parse_dec(char d) {
  if(d >= '0' && d <= '9') {
    return d - '0';
  }
  return 0;
}

int util_parse_hex(char h) {
  if(h >= 'A' && h <= 'F') {
    return h - 'A' + 10;
  } else if(h >= 'a' && h <= 'f') {
    return h - 'a' + 10;
  }
  return util_parse_dec(h);
}

unsigned long util_term2argb(int t) {
  unsigned long argb = 0;
  t -= 16;
  argb = ((t / 36) * 51) << 8;
  argb += ((t % 36)/ 6) * 51;
  argb = argb << 8;
  argb += ((t % 36) % 6) * 51;
  return argb;
}

int util_rgb2term(const char *rgb) {
  int r = 0;
  int g = 0;
  int b = 0;
  int cur = 0;
  int pos = 0;

  for(int i = strlen(rgb); i >= 0; --i) {
    if(rgb[i] == ',') {
      ++cur;
      pos = 0;
    } else if(rgb[i] != ')') {
      if(cur == 0) {
        b += util_parse_dec(rgb[i]) * (pos == 2 ? 100 : (pos == 1 ? 10 : 1));
      } else if(cur == 1) {
        g += util_parse_dec(rgb[i]) * (pos == 2 ? 100 : (pos == 1 ? 10 : 1));
      } else if(cur == 2) {
        r += util_parse_dec(rgb[i]) * (pos == 2 ? 100 : (pos == 1 ? 10 : 1));
      }
      ++pos;
    }
  }

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
  } else if(c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == '(') {
    return util_rgb2term(&c[4]);
  } else {
    return util_hex2term(c);
  }
}

int util_max(int a, int b) {
  return (a>b)?a:b;
}

int util_min(int a, int b) {
  return (a>b)?b:a;
}

char *util_error(char *name, int id) {
  char *out = (char *)malloc(sizeof(char) * (strlen(name) + 5));
  sprintf(out, name, id);
  return out;
}
