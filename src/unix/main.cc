/* ttyutil - unix/main.cc
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

ttyu_js_c::ttyu_js_c() : running(FALSE), stop(TRUE), mode(MODE_VT100),
    x(0), y(0) {
  colors = ttyu_get_colors();
}

ttyu_js_c::~ttyu_js_c() {
  running = FALSE;
  stop = TRUE;
  delete emitter;
}

TTYU_INLINE int ttyu_get_colors() {
  FILE *pipe = popen("tput colors", "r");
  int out = 0;
  if(!pipe) return out;
  char buffer[64];
  std::string result = "";
  while(!feof(pipe)) {
    if(fgets(buffer, 64, pipe) != NULL)
      result += buffer;
  }
  int len = static_cast<int>(result.size());
  for(int i = len; i > 0; --i) {
    char d = result[i-1];
    if(d >= '0' && d <= '9') {
      out += (d - '0') * pow(10, len - i - 1);
    }
  }
  pclose(pipe);
  return out;
}
