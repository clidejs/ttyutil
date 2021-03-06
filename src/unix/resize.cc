/* ttyutil - unix/resize.cc
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

JSFUNCTION(ttyu_js_c::js_getwidth, {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  NanReturnValue(NanNew<v8::Number>(w.ws_col));
})

JSFUNCTION(ttyu_js_c::js_getheight, {
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  NanReturnValue(NanNew<v8::Number>(w.ws_row));
})

JSFUNCTION(ttyu_js_c, js_setwidth, {
  THROW_IF_STOPPED(that);
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  resizeterm(args[0]->Int32Value(), w.ws_row);
  wrefresh(that->win);
})

JSFUNCTION(ttyu_js_c, js_setheight, {
  THROW_IF_STOPPED(that);
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  resizeterm(w.ws_col, args[0]->Int32Value());
  wrefresh(that->win);
})

JSFUNCTION(ttyu_js_c, js_resize, {
  THROW_IF_STOPPED(that);
  resizeterm(args[0]->Int32Value(), args[1]->Int32Value());
  wrefresh(that->win);
})
