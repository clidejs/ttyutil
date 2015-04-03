/* ttyutil - win.h - additional header file for windows systems,
 * implementation in src/win.cc
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
#ifndef TTYU_WIN_H_
#define TTYU_WIN_H_

#include <windows.h>

#define WIN_BUFFER_SIZE 128

typedef struct ttyu_error_s {
  char *msg;
  bool kill;
} ttyu_error_t;

#define ERROR_WIN_INIT "could not get console handles - %d"
#define ERROR_WIN_GET "could not get console output buffer information - %d"
#define ERROR_WIN_SET "could not set console output buffer information - %d"
#define ERROR_WIN_FILL "could not fill console output buffer - %d"

// windows data structure
struct ttyu_data_s {
  HANDLE hin;
  HANDLE hout;
  DWORD old_mode;
  ttyu_error_t *err;
  int width;
  int height;
  int top;
  int curx;
  int cury;
  short base_color;
  bool closing;
};

int ttyu_win_which(DWORD code);
int ttyu_win_ctrl(DWORD state);
DWORD ttyu_win_state(int ctrl);
bool ttyu_win_scr_update(ttyu_data_t *data, bool initial = FALSE);
void ttyu_win_render(char *c, ttyu_data_t *data);
bool ttyu_win_clrscr(ttyu_data_t *data, int x, int y, int width, int height);

#endif // TTYU_WIN_H_
