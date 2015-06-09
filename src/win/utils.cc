/* ttyutil - win/utils.cc
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

bool ttyu_win_scr_update(ttyu_js_c *obj, bool initial) {
  CONSOLE_SCREEN_BUFFER_INFO con_info;

  if (!GetConsoleScreenBufferInfo(obj->hout, &con_info)) {
    /*data->err->msg = ERRMSG(0x02);
    return !(data->err->kill = TRUE);*/
    return FALSE;
  }

  obj->top = con_info.srWindow.Top;
  obj->width = con_info.dwSize.X;
  obj->height = con_info.dwSize.Y - obj->top;

  obj->curx = con_info.dwCursorPosition.X;
  obj->cury = con_info.dwCursorPosition.Y - obj->top;

  /*if(initial) {
    data->base_color = (short)con_info.wAttributes;
  } */
  return TRUE;
}

int ttyu_win_ctrl(DWORD state) {
  int ctrl = CTRL_NULL;
  if (state & RIGHT_ALT_PRESSED || state & LEFT_ALT_PRESSED) {
    ctrl |= CTRL_ALT;
  }
  if (state & RIGHT_CTRL_PRESSED || state & LEFT_CTRL_PRESSED) {
    ctrl |= CTRL_CTRL;
  }
  if (state & SHIFT_PRESSED) {
    ctrl |= CTRL_SHIFT;
  }
  if (state & ENHANCED_KEY) {
    ctrl |= CTRL_ENHANCED;
  }
  if (state & NUMLOCK_ON) {
    ctrl |= CTRL_NUMLOCK;
  }
  if (state & SCROLLLOCK_ON) {
    ctrl |= CTRL_SCROLLLOCK;
  }
  if (state & CAPSLOCK_ON) {
    ctrl |= CTRL_CAPSLOCK;
  }
  return ctrl;
}

int ttyu_win_which(DWORD code) {
  if (code > 0) {
    return static_cast<int>(code);
  }
  return WHICH_UNKNOWN;
}

DWORD ttyu_win_state(int ctrl) {
  DWORD state = 0;
  if (ctrl & CTRL_ALT) {
    state |= RIGHT_ALT_PRESSED & LEFT_ALT_PRESSED;
  }
  if (ctrl & CTRL_CTRL) {
    state |= RIGHT_CTRL_PRESSED & LEFT_CTRL_PRESSED;
  }
  if (ctrl & CTRL_SHIFT) {
    state |= SHIFT_PRESSED;
  }
  if (ctrl & CTRL_ENHANCED) {
    state |= ENHANCED_KEY;
  }
  if (ctrl & CTRL_NUMLOCK) {
    state |= NUMLOCK_ON;
  }
  if (ctrl & CTRL_SCROLLLOCK) {
    state |= SCROLLLOCK_ON;
  }
  if (ctrl & CTRL_CAPSLOCK) {
    state |= CAPSLOCK_ON;
  }
  return state;
}
