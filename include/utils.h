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

#define TTYU_TOSTRING(handle)                                                  \
  (new v8::String::Utf8Value(handle->ToString()))->operator*()

#if defined(__GNUC__) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE __forceinline
#else
# define TTYU_INLINE inline
#endif

#define WIN_COLORS 16

#define COLORS_ARRAY(XX, i)                                                    \
  XX(0, i, 0x000000)                                                           \
  XX(1, i, 0x800000)                                                           \
  XX(2, i, 0x008000)                                                           \
  XX(3, i, 0x808000)                                                           \
  XX(4, i, 0x000080)                                                           \
  XX(5, i, 0x800080)                                                           \
  XX(6, i, 0x008080)                                                           \
  XX(7, i, 0xc0c0c0)                                                           \
  XX(8, i, 0x808080)                                                           \
  XX(9, i, 0xff0000)                                                           \
  XX(10, i, 0x00ff00)                                                          \
  XX(11, i, 0xffff00)                                                          \
  XX(12, i, 0x0000ff)                                                          \
  XX(13, i, 0xff00ff)                                                          \
  XX(14, i, 0x00ffff)                                                          \
  XX(15, i, 0xffffff)

TTYU_INLINE uint32_t util_colormatch(uint8_t i);
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

// DEBUGGING
#include <iostream>
#include <fstream>
#ifdef DEBUG
#define THREAD_NUM 4
#define COL_SIZE 25
TTYU_INLINE void _DBG(const std::string &text, int thread) {  // NOLINT ???
  std::ofstream log_file;
  log_file.open("debug.log", std::ios_base::app);
  log_file << "|";
  size_t x = COL_SIZE * thread;

  while (x-- > 0) {
    log_file << " ";
    if (x%COL_SIZE == 0) {
      log_file << "|";
    }
  }

  log_file << text;

  size_t tlen = text.length();
  x = COL_SIZE * (THREAD_NUM - thread) - tlen;
  while (x-- > 0) {
    log_file << " ";
    if (x%COL_SIZE == 0) {
      log_file << "|";
    }
  }
  log_file << std::endl;
  log_file.close();
}

TTYU_INLINE void _DBGHEAD() {
  std::ofstream log_file;
  log_file.open("debug.log", std::ios_base::app);
  log_file << "+-------------------------+-------------------------+----------"
      << "---------------+-------------------------+" << std::endl;
  log_file << "|                                            ttyutil debug.log "
      << "                                         |" << std::endl;
  log_file << "+-------------------------+-------------------------+----------"
      << "---------------+-------------------------+" << std::endl;
  log_file << "|       main thread       |      libuv thread       |      curs"
      << "es thread      |         emit loop       |" << std::endl;
  log_file << "+-------------------------+-------------------------+----------"
      << "---------------+-------------------------+" << std::endl;
  log_file.close();
}
#define DBG(msg, thread) _DBG(msg, thread);
#define DBGHEAD() _DBGHEAD();
#else  // DEBUG
#define DBG(msg, thread)
#define DBGHEAD()
#endif  // DEBUG

#endif  // INCLUDE_UTILS_H_
