/* ttyutil - cdebug.h - header file defining debugging macros
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
#ifndef INCLUDE_CDEBUG_H_
#define INCLUDE_CDEBUG_H_
#include <cstdio>
#include <string>

#ifdef CDEBUG
# ifdef CDEBUG_FILE
// TODO(@bbuecherl) write debugging log
# else  // ifndef CDEBUG_FILE
#define DBG(msg) printf("%s\r\n", msg)
#define SDBG(f, ...) do {                                                      \
  std::string str = f;                                                         \
  str.append("\r\n");                                                          \
  printf(str.c_str(), __VA_ARGS__);                                            \
} while (0)

# endif  // CDEBUG_FILE
#else  // ifndef CDEBUG
#define DBG(msg)
#define SDBG(f, ...)

#endif  // CDEBUG

#endif  // INCLUDE_CDEBUG_H_
