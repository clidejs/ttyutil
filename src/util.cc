#include <ttyu.h>

unsigned long util_colors_a[] = { 0x000000, 0x800000, 0x008000, 0x808000,
    0x000080, 0x800080, 0x008080, 0xc0c0c0, 0x808080, 0xff0000, 0x00ff00,
    0xffff00, 0x0000ff, 0xff00ff, 0x00ffff, 0xffffff };

short util_rgbi2term(short r, short g, short b) {
  return (short)(r / 51 * 36 + g / 51 * 6 + b / 51 + 16);
}

short util_rgbi2win(short r, short g, short b) {
  unsigned long match;
  short match_i = 0;
  int match_diff = 256*256*256;
  int diff = match_diff;
  short mr;
  short mg;
  short mb;

  for(short i = 0; i < WIN_COLORS; ++i) {
    match = util_colors_a[i];
    mr = match >> 16;
    mg = (match << 16) >> 24;
    mb = (match << 24) >> 32;
  }
  return match_i;
}

char *util_render(const char *ch, short fg, short bg) {
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

short util_parse_dec(char d) {
  if(d >= '0' && d <= '9') {
    return d - '0';
  }
  return 0;
}

short util_parse_hex(char h) {
  if(h >= 'A' && h <= 'F') {
    return h - 'A' + 10;
  } else if(h >= 'a' && h <= 'f') {
    return h - 'a' + 10;
  }
  return util_parse_dec(h);
}

unsigned long util_term2argb(short t) {
  unsigned long argb = 0;
  t -= 16;
  argb = ((t / 36) * 51) << 8;
  argb += ((t % 36)/ 6) * 51;
  argb = argb << 8;
  argb += ((t % 36) % 6) * 51;
  return argb;
}

short util_rgb2term(const char *rgb) {
  short r = 0;
  short g = 0;
  short b = 0;
  short cur = 0;
  short pos = 0;

  for(short i = strlen(rgb); i >= 0; --i) {
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

#ifdef PLATFORM_WINDOWS
  return util_rgbi2win(r, g, b);
#else
  return util_rgbi2term(r, g, b);
#endif
}

short util_hex2term(const char *hex) {
  short r = 0;
  short g = 0;
  short b = 0;
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
#ifdef PLATFORM_WINDOWS
  return util_rgbi2win(r, g, b);
#else
  return util_rgbi2term(r, g, b);
#endif
}

short util_color(const char *c) {
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
