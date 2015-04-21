/* ttyutil - util.cc - implements utility methods (see include/util.h)
 * https://github.com/clidejs/ttyutil
 *
 * Copyright Bernhard Bücherl <bernhard.buecherl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <ttyu.h>

TTYU_INLINE uint32_t util_colormatch(uint8_t i) {
  #define XX(index, i, c) if (index == i) { return c; }
  COLORS_ARRAY(XX, i);
  #undef XX
  return 0;
}

TTYU_INLINE int16_t util_rgbi2term(int16_t r, int16_t g, int16_t b) {
  return static_cast<int16_t>(r / 51 * 36 + g / 51 * 6 + b / 51 + 16);
}

TTYU_INLINE int16_t util_rgbi2win(int16_t r, int16_t g, int16_t b) {
  uint32_t match;
  int16_t match_i = 0;
  int match_diff = 256+256+256;
  int diff;
  unsigned char mr;
  unsigned char mg;
  unsigned char mb;

  for (uint8_t i = 0; i < 16; ++i) {
    match = util_colormatch(i);
    mb = ((match << 8) >> 24);
    mg = ((match << 16) >> 24);
    mr = ((match << 24) >> 24);

    diff = util_abs(mr - r) + util_abs(mg - g) + util_abs(mb - b);
    if (diff <= match_diff) {
      match_diff = diff;
      match_i = i;
    }
  }
  return match_i;
}

TTYU_INLINE char *util_render(const char *ch, int16_t fg, int16_t bg) {
  char *out =
      reinterpret_cast<char *>(std::malloc(sizeof(char) * (strlen(ch) + 30)));
  if (fg != -1 && bg != -1) {
    sprintf(out, "\x1b[38;5;%dm\x1b[48;5;%dm%s\x1b[49m\x1b[39m", fg, bg, ch);
  } else if (fg != -1) {
    sprintf(out, "\x1b[38;5;%dm%s\x1b[39m", fg, ch);
  } else if (bg != -1) {
    sprintf(out, "\x1b[48;5;%dm%s\x1b[49m", bg, ch);
  } else {
    sprintf(out, "%s", ch);
  }
  return out;
}

TTYU_INLINE int16_t util_parse_dec(char d) {
  if (d >= '0' && d <= '9') {
    return d - '0';
  }
  return 0;
}

TTYU_INLINE int16_t util_parse_hex(char h) {
  if (h >= 'A' && h <= 'F') {
    return h - 'A' + 10;
  } else if (h >= 'a' && h <= 'f') {
    return h - 'a' + 10;
  }
  return util_parse_dec(h);
}

TTYU_INLINE uint32_t util_term2argb(int16_t t) {
  uint32_t argb = 0;
  t -= 16;
  argb = ((t / 36) * 51) << 8;
  argb += ((t % 36)/ 6) * 51;
  argb = argb << 8;
  argb += ((t % 36) % 6) * 51;
  return argb;
}

TTYU_INLINE int16_t util_rgb2term(const char *rgb) {
  int16_t r = 0;
  int16_t g = 0;
  int16_t b = 0;
  int16_t cur = 0;
  int16_t pos = 0;

  for (uint8_t i = static_cast<int>(strlen(rgb)); i >= 0; --i) {
    if (rgb[i] == ',') {
      ++cur;
      pos = 0;
    } else if (rgb[i] != ')') {
      if (cur == 0) {
        b += util_parse_dec(rgb[i]) * (pos == 2 ? 100 : (pos == 1 ? 10 : 1));
      } else if (cur == 1) {
        g += util_parse_dec(rgb[i]) * (pos == 2 ? 100 : (pos == 1 ? 10 : 1));
      } else if (cur == 2) {
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

TTYU_INLINE int16_t util_hex2term(const char *hex) {
  int16_t r = 0;
  int16_t g = 0;
  int16_t b = 0;
  if (strlen(hex) == 6) {
    r = util_parse_hex(hex[0]) * 16 + util_parse_hex(hex[1]);
    g = util_parse_hex(hex[2]) * 16 + util_parse_hex(hex[3]);
    b = util_parse_hex(hex[4]) * 16 + util_parse_hex(hex[5]);
  } else if (strlen(hex) == 3) {
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

TTYU_INLINE int16_t util_color(const char *c) {
  if (c[0] == '#') {
    return util_hex2term(&c[1]);
  } else if (c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == '(') {
    return util_rgb2term(&c[4]);
  } else {
    return util_hex2term(c);
  }
}

TTYU_INLINE int util_max(int a, int b) {
  return (a > b) ? a : b;
}

TTYU_INLINE int util_min(int a, int b) {
  return (a > b) ? b : a;
}

TTYU_INLINE int util_abs(int a) {
  return (a < 0) ? a * -1 : a;
}
