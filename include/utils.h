/* ttyutil - util.h - header file to define utility methods,
 * implementation in src/util.cc
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
#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#include <string.h>

#define EXPORT_METHOD(tpl, name, cb) do {                                      \
  v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(cb);        \
  tpl->InstanceTemplate()->Set(NanNew<v8::String>(name),                       \
      t->GetFunction(), v8::ReadOnly);                                         \
} while (0)

#define EXPORT_GET(tpl, name, fn) do {                                         \
  v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(fn);        \
  tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name),               \
      t->GetFunction());                                                       \
} while (0)

#define EXPORT_GETSET(tpl, name, get, set)                                     \
  tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (get), (set))

#if defined(__GNUC__) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE __forceinline
#else
# define TTYU_INLINE inline
#endif

#define WIN_COLORS 16

TTYU_INLINE int16_t util_rgbi2term(int16_t r, int16_t g, int16_t b);
TTYU_INLINE int16_t util_rgbi2win(int16_t r, int16_t g, int16_t b);
TTYU_INLINE char *util_render(const char *ch, int16_t fg, int16_t bg);
TTYU_INLINE int16_t util_parse_dec(char d);
TTYU_INLINE int16_t util_parse_hex(char h);
TTYU_INLINE uint32_t util_term2argb(int16_t t);
TTYU_INLINE int16_t util_rgb2term(const char *rgb);
TTYU_INLINE int16_t util_hex2term(const char *hex);
TTYU_INLINE int16_t util_color(const char *c);
TTYU_INLINE int util_max(int a, int b);
TTYU_INLINE int util_min(int a, int b);
TTYU_INLINE int util_abs(int a);
TTYU_INLINE char *util_error(char *name, int id);

#undef ERROR
#define ERROR(name, id) util_error(name, id)
#ifdef DEBUG
# include <iostream>
# define DBG(msg) std::cout << msg << "\r\n"
#else
# define DBG(msg)  // empty
#endif

#endif  // INCLUDE_UTILS_H_
