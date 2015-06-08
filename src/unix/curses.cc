/* ttyutil - unix/curses.cc
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

int ttyu_js_c::curses_threaded_func(WINDOW *win, void *that) {
  ttyu_js_c *obj = static_cast<ttyu_js_c *>(that);

  int c = wgetch(win);
  MEVENT mev;
  ttyu_event_t event;
  event.type = EVENT_NONE;

  if (c == ERR) {
    uv_mutex_lock(&obj->ungetlock);
    if (!obj->unget_stack.empty()) {
      ttyu_event_t ev = obj->unget_stack.front();
      obj->unget_stack.pop();
      uv_mutex_unlock(&obj->ungetlock);
      switch (ev.type) {
        case EVENT_KEY:
          ungetch(ev.key->code);
          break;
        case EVENT_MOUSEUP:
        case EVENT_MOUSEDOWN:
        case EVENT_MOUSEMOVE:
          mev.x = ev.mouse->x;
          mev.y = ev.mouse->y;
          mev.bstate = 0;

          if (ev.mouse->ctrl & CTRL_ALT) { mev.bstate |= BUTTON_ALT; }
          if (ev.mouse->ctrl & CTRL_CTRL) { mev.bstate |= BUTTON_CTRL; }
          if (ev.mouse->ctrl & CTRL_SHIFT) { mev.bstate |= BUTTON_SHIFT; }

          if (ev.type == EVENT_MOUSEMOVE) {
            mev.bstate |= REPORT_MOUSE_POSITION;
          } else if (ev.type == EVENT_MOUSEUP) {
            if (ev.mouse->button == MOUSE_LEFT) {
              mev.bstate |= BUTTON1_RELEASED;
            } else if (ev.mouse->button == MOUSE_LEFT2) {
              mev.bstate |= BUTTON2_RELEASED;
            } else if (ev.mouse->button == MOUSE_LEFT3) {
              mev.bstate |= BUTTON3_RELEASED;
            } else if (ev.mouse->button == MOUSE_LEFT4) {
              mev.bstate |= BUTTON4_RELEASED;
            } else if (ev.mouse->button == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
              mev.bstate |= BUTTON5_RELEASED;
#else
              mev.bstate |= BUTTON4_RELEASED;
#endif
            }
          } else if (ev.type == EVENT_MOUSEDOWN) {
            if (ev.mouse->button == MOUSE_LEFT) {
              mev.bstate |= BUTTON1_PRESSED;
            } else if (ev.mouse->button == MOUSE_LEFT2) {
              mev.bstate |= BUTTON2_PRESSED;
            } else if (ev.mouse->button == MOUSE_LEFT3) {
              mev.bstate |= BUTTON3_PRESSED;
            } else if (ev.mouse->button == MOUSE_LEFT4) {
              mev.bstate |= BUTTON4_PRESSED;
            } else if (ev.mouse->button == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
              mev.bstate |= BUTTON5_PRESSED;
#else
              mev.bstate |= BUTTON4_PRESSED;
#endif
            }
          }
          ungetmouse(&mev);
          break;
        case EVENT_MOUSEWHEEL:
        case EVENT_MOUSEHWHEEL:
          // TODO(@bbuecherl)
          break;
        default:
          // EVENT_NONE, EVENT_ERROR, EVENT_SIGNAL, EVENT_RESIZE
          break;
      }
    } else {
      uv_mutex_unlock(&obj->ungetlock);
    }
    return 0;
  } else if (c == KEY_RESIZE) {
    ttyu_event_create_resize(&event);
  } else if (c == KEY_MOUSE) {
    if (getmouse(&mev) == OK) {
      ttyu_event_create_mouse(&event, EVENT_ERROR, 0, mev.x, mev.y, 0);

      // add button control key sequences if possible
      if (mev.bstate & BUTTON_SHIFT) { event.mouse->ctrl |= CTRL_SHIFT; }
      if (mev.bstate & BUTTON_CTRL) { event.mouse->ctrl |= CTRL_CTRL; }
      if (mev.bstate & BUTTON_ALT) { event.mouse->ctrl |= CTRL_ALT; }

      // convert button codes and mev type
      if (mev.bstate & REPORT_MOUSE_POSITION) {
        event.type = EVENT_MOUSEMOVE;
      } else if (mev.bstate & BUTTON1_RELEASED) {
        event.mouse->button = MOUSE_LEFT;
        event.type = EVENT_MOUSEUP;
      } else if (mev.bstate & BUTTON1_PRESSED) {
        event.mouse->button = MOUSE_LEFT;
        event.type = EVENT_MOUSEDOWN;
      } else if (mev.bstate & BUTTON2_RELEASED) {
        event.mouse->button = MOUSE_LEFT2;
        event.type = EVENT_MOUSEUP;
      } else if (mev.bstate & BUTTON2_PRESSED) {
        event.mouse->button = MOUSE_LEFT2;
        event.type = EVENT_MOUSEDOWN;
      } else if (mev.bstate & BUTTON3_RELEASED) {
        event.mouse->button = MOUSE_LEFT3;
        event.type = EVENT_MOUSEUP;
      } else if (mev.bstate & BUTTON3_PRESSED) {
        event.mouse->button = MOUSE_LEFT3;
        event.type = EVENT_MOUSEDOWN;
      } else {
#if NCURSES_MOUSE_VERSION > 1
        if (mev.bstate & BUTTON4_RELEASED) {
          event.mouse->button = MOUSE_LEFT4;
          event.type = EVENT_MOUSEUP;
        } else if (mev.bstate & BUTTON4_PRESSED) {
          event.mouse->button = MOUSE_LEFT4;
          event.type = EVENT_MOUSEDOWN;
        } else if (mev.bstate & BUTTON5_RELEASED) {
          event.mouse->button = MOUSE_RIGHT;
          event.type = EVENT_MOUSEUP;
        } else if (mev.bstate & BUTTON5_PRESSED) {
          event.mouse->button = MOUSE_RIGHT;
          event.type = EVENT_MOUSEDOWN;
        }
#else
        if (mev.bstate & BUTTON4_RELEASED) {
          event.mouse->button = MOUSE_RIGHT;
          event.type = EVENT_MOUSEUP;
        } else if (mev.bstate & BUTTON4_PRESSED) {
          event.mouse->button = MOUSE_RIGHT;
          event.type = EVENT_MOUSEDOWN;
        }
#endif
      }
      if (event.type == EVENT_ERROR) {
        // uncaught mouse event
        event.type = EVENT_NONE;
      } else {
        obj->mode = MODE_VT102;
      }
    } else {
      // bad mouse event
      event.type = EVENT_NONE;
    }
  } else {
    char *ch = const_cast<char *>(keyname(c));
    int ctrl = CTRL_NULL;
    int which = WHICH_UNKNOWN;

    if (c >= 48 && c <= 57) {
      which = c;  // WHICH_CHAR0 to WHICH_CHAR9
    } else if (c >= 65 && c <= 90) {
      ctrl |= CTRL_SHIFT;
      which = c;  // WHICH_CHARA to WHICH_CHARZ
    } else if (c >= 97 && c <= 122) {
      which = c - 32;  // WHICH_CHARA to WHICH_CHARZ
    } else if (c == KEY_COMMAND) {
      ctrl |= CTRL_CMD;
    } else if (c == KEY_SCOMMAND) {
      ctrl |= CTRL_CMD | CTRL_SHIFT;
    } else {
      which = ttyu_unix_which(c);
      if (sizeof(ch) > 4 && ch[0] == '^') {
        ctrl |= CTRL_CTRL;
      }
    }
    ttyu_event_create_key(&event, ctrl, ch, c, which);
  }

  if (event.type != EVENT_NONE) {
    uv_mutex_lock(&obj->emitstacklock);
    {
      obj->emit_stack.push_back(event);
      uv_cond_signal(&obj->condition);
    }
    uv_mutex_unlock(&obj->emitstacklock);
  }
  return 0;
}

void ttyu_js_c::curses_thread_func(void *that) {
  ttyu_js_c *obj = static_cast<ttyu_js_c *>(that);

  noecho();
  cbreak();
  keypad(obj->win, TRUE);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  nodelay(obj->win, TRUE);
  mouseinterval(FALSE);

  uv_barrier_wait(&obj->barrier);

  while (obj->running && !obj->stop) {
    use_window(obj->win, curses_threaded_func, that);
    usleep(100);
  }
}
