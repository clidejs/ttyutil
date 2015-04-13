/* ttyutil - unix.h - additional header file for unixy systems,
 * impementation in src/unix.cc
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
#ifndef TTYU_UNIX_H_
#define TTYU_UNIX_H_

#define NCURSES_OPAQUE FALSE
#include <curses.h>
#include <queue>

#define ERROR_UNIX_UNDEF "unknown error occured while reading input"
#define ERROR_UNIX_MOUSEBAD "skipping unreadable mouse event"
#define ERROR_UNIX_MOUSEUNCAUGHT "skipping unknown mouse event"

#define TTYU_EXIT 2
#define TTYU_UNKNOWN 1

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

// thread save queue
template<class T> struct ttyu_queue_data_s;
template<class T> class ttyu_queue_c {
public:
  ttyu_queue_c() : size_(0) {
    uv_mutex_init(&mutex_);
  }

  virtual ~ttyu_queue_c() {
    uv_mutex_destroy(&mutex_);
  }

  void push(T val) {
    DBG("push has started");
    uv_mutex_lock(&mutex_);
    queue_.push(&val);
    uv_mutex_unlock(&mutex_);
    DBG("push has finished");
  }

  T *pop() {
    DBG("pop acquiring rdlock");
    uv_mutex_lock(&mutex_);
    DBG("pop acquired rdlock");
    T *front = queue_.front();
    queue_.pop();
    uv_mutex_unlock(&mutex_);
    DBG("pop released rdlock");
    return front;
  }

  int size() {
    DBG("size has sarted");
    if(uv_mutex_trylock(&mutex_) == 0) {
    DBG("size acquired rdlock");
      size_ = queue_.size();
      uv_mutex_unlock(&mutex_);
    DBG("size released rdlock");
    }
    return size_;
  }

  bool empty() {
    DBG("empty has sarted");
    if(uv_mutex_trylock(&mutex_) == 0) {
    DBG("empty acquired rdlock");
      size_ = queue_.size();
      uv_mutex_unlock(&mutex_);
    DBG("empty released rdlock");
    }
    return (size_ == 0);
  }

private:
  std::queue<T *> queue_;
  uv_mutex_t mutex_;
  int size_;
};

// unixy data structure
struct ttyu_data_s {
  WINDOW *win;
  mmask_t old_mouse_mask;
  int mode;
  bool closing;

  ttyu_queue_c<int> ungetch_stack;
  ttyu_queue_c<MEVENT> ungetmouse_stack;
};

void ttyu_unix_clrscr(ttyu_data_t *data, int x, int y, int width, int height);
int ttyu_unix_key(int which);
int ttyu_unix_which(int key);

#endif // TTYU_UNIX_H_
