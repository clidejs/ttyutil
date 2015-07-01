/* ttyutil - unix.h - additional header file for unixy systems,
 * impementation in src/unix/.
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
#ifndef INCLUDE_UNIX_H_
#define INCLUDE_UNIX_H_

#define NCURSES_OPAQUE FALSE
#include <sys/ioctl.h>
#include <curses.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <queue>

#define TTYU_UNIX_KW(XX)                                                       \
  XX(WHICH_DOWN, KEY_DOWN, FALSE);                                             \
  XX(WHICH_DOWN, KEY_SF, FALSE);                                               \
  XX(WHICH_UP, KEY_UP, FALSE);                                                 \
  XX(WHICH_UP, KEY_SR, FALSE);                                                 \
  XX(WHICH_LEFT, KEY_LEFT, FALSE);                                             \
  XX(WHICH_RIGHT, KEY_RIGHT, FALSE);                                           \
  XX(WHICH_HOME, KEY_HOME, FALSE);                                             \
  XX(WHICH_HOME, KEY_LL, FALSE);                                               \
  XX(WHICH_HOME, KEY_BEG, FALSE);                                              \
  XX(WHICH_BACKSPACE, KEY_BACKSPACE, FALSE);                                   \
  XX(WHICH_BACKSPACE, KEY_BTAB, FALSE);                                        \
  XX(WHICH_F1, KEY_F(1), FALSE);                                               \
  XX(WHICH_F2, KEY_F(2), FALSE);                                               \
  XX(WHICH_F3, KEY_F(3), FALSE);                                               \
  XX(WHICH_F4, KEY_F(4), FALSE);                                               \
  XX(WHICH_F5, KEY_F(5), FALSE);                                               \
  XX(WHICH_F6, KEY_F(6), FALSE);                                               \
  XX(WHICH_F7, KEY_F(7), FALSE);                                               \
  XX(WHICH_F8, KEY_F(8), FALSE);                                               \
  XX(WHICH_F9, KEY_F(9), FALSE);                                               \
  XX(WHICH_F10, KEY_F(10), FALSE);                                             \
  XX(WHICH_F11, KEY_F(11), FALSE);                                             \
  XX(WHICH_F12, KEY_F(12), FALSE);                                             \
  XX(WHICH_F13, KEY_F(13), FALSE);                                             \
  XX(WHICH_F14, KEY_F(14), FALSE);                                             \
  XX(WHICH_F15, KEY_F(15), FALSE);                                             \
  XX(WHICH_F16, KEY_F(16), FALSE);                                             \
  XX(WHICH_F17, KEY_F(17), FALSE);                                             \
  XX(WHICH_F18, KEY_F(18), FALSE);                                             \
  XX(WHICH_F19, KEY_F(19), FALSE);                                             \
  XX(WHICH_F20, KEY_F(20), FALSE);                                             \
  XX(WHICH_F21, KEY_F(21), FALSE);                                             \
  XX(WHICH_F22, KEY_F(22), FALSE);                                             \
  XX(WHICH_F23, KEY_F(23), FALSE);                                             \
  XX(WHICH_F24, KEY_F(24), FALSE);                                             \
  XX(WHICH_DELETE, KEY_DC, FALSE);                                             \
  XX(WHICH_INSERT, KEY_IC, FALSE);                                             \
  XX(WHICH_INSERT, KEY_EIC, FALSE);                                            \
  XX(WHICH_CLEAR, KEY_CLEAR, FALSE);                                           \
  XX(WHICH_CLEAR, KEY_EOS, FALSE);                                             \
  XX(WHICH_CLEAR, KEY_EOL, FALSE);                                             \
  XX(WHICH_NEXT, KEY_NPAGE, FALSE);                                            \
  XX(WHICH_PRIOR, KEY_PPAGE, FALSE);                                           \
  XX(WHICH_SPACE, 32, FALSE);                                                  \
  XX(WHICH_TAB, KEY_STAB, FALSE);                                              \
  XX(WHICH_ENTER, KEY_ENTER, FALSE);                                           \
  XX(WHICH_PRINT, KEY_PRINT, FALSE);                                           \
  XX(WHICH_END, KEY_END, FALSE);                                               \
  XX(WHICH_HELP, KEY_HELP, FALSE);                                             \
  XX(WHICH_BROWSER_REFRESH, KEY_REFRESH, FALSE);                               \
                                                                               \
  XX(WHICH_HOME, KEY_SBEG, TRUE);                                              \
  XX(WHICH_DELETE, KEY_SDC, TRUE);                                             \
  XX(WHICH_END, KEY_SEND, TRUE);                                               \
  XX(WHICH_CLEAR, KEY_SEOL, TRUE);                                             \
  XX(WHICH_HELP, KEY_SHELP, TRUE);                                             \
  XX(WHICH_HOME, KEY_SHOME, TRUE);                                             \
  XX(WHICH_INSERT, KEY_SIC, TRUE);                                             \
  XX(WHICH_LEFT, KEY_SLEFT, TRUE);                                             \
  XX(WHICH_NEXT, KEY_SNEXT, TRUE);                                             \
  XX(WHICH_PRIOR, KEY_SPREVIOUS, TRUE);                                        \
  XX(WHICH_PRINT, KEY_SPRINT, TRUE);                                           \
  XX(WHICH_RIGHT, KEY_SRIGHT, TRUE);                                           \
                                                                               \
  XX(WHICH_SHIFT, 0, TRUE)

#define REFRESH_POSITION(obj) wmove(obj->win, obj->x, obj->y);                 \
  wrefresh(obj->win)

int ttyu_unix_which(int c);
int ttyu_unix_key(int which);
TTYU_INLINE int ttyu_get_colors();

class ttyu_worker_c : public NanAsyncWorker {
 public:
  explicit ttyu_worker_c(ttyu_js_c *obj) : NanAsyncWorker(NULL), obj(obj) { }
  ~ttyu_worker_c() { }

  void Execute();
  void HandleOKCallback();

 private:
  ttyu_js_c *obj;
  std::vector<ttyu_event_t> emit_stack;
};

#define PLATFORM_DEPENDENT_FIELDS                                              \
  void check_queue();                                                          \
  static void curses_thread_func(void *that);                                  \
  static int curses_threaded_func_thread(WINDOW *win, void *that);             \
  static int curses_threaded_func(WINDOW *win, ttyu_js_c *obj);                \
                                                                               \
  uv_thread_t curses_thread;                                                   \
  WINDOW *win;                                                                 \
  uv_barrier_t barrier;                                                        \
  uv_mutex_t emitstacklock;                                                    \
  uv_mutex_t ungetlock;                                                        \
  uv_cond_t condition;                                                         \
  int mode;                                                                    \
  int x;                                                                       \
  int y;                                                                       \
  int colors;                                                                  \
  bool worker_run;                                                             \
  std::queue<ttyu_event_t> unget_stack;                                        \
  std::vector<ttyu_event_t> emit_stack

#endif  // INCLUDE_UNIX_H_#
