/* ttyutil - unix.cc - implements methods for unixy systems (see include/unix.h)
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

TTYU_INLINE void getevent(ttyu_data_t *data, ttyu_event_t *event) {

}

TTYU_INLINE void ungetevent(ttyu_data_t *data, ttyu_event_t *event) {
      MEVENT mev;

      mev.x = (short)arg2->Int32Value();
      mev.y = (short)arg3->Int32Value();
      mev.bstate = 0;

      if(arg4 & CTRL_ALT) { mev.bstate |= BUTTON_ALT; }
      if(arg4 & CTRL_CTRL) { mev.bstate |= BUTTON_CTRL; }
      if(arg4 & CTRL_SHIFT) { mev.bstate |= BUTTON_SHIFT; }

      if(arg0 == EVENT_MOUSEMOVE) {
        mev.bstate |= REPORT_MOUSE_POSITION;
      } else if(arg0 == EVENT_MOUSEUP) {
        if(arg1 == MOUSE_LEFT) {
          mev.bstate |= BUTTON1_RELEASED;
        } else if(arg1 == MOUSE_LEFT2) {
          mev.bstate |= BUTTON2_RELEASED;
        } else if(arg1 == MOUSE_LEFT3) {
          mev.bstate |= BUTTON3_RELEASED;
        } else if(arg1 == MOUSE_LEFT4) {
          mev.bstate |= BUTTON4_RELEASED;
        } else if(arg1 == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
          mev.bstate |= BUTTON5_RELEASED;
#else
          mev.bstate |= BUTTON4_RELEASED;
#endif
        }
      } else if(arg0 == EVENT_MOUSEDOWN) {
        if(arg1 == MOUSE_LEFT) {
          mev.bstate |= BUTTON1_PRESSED;
        } else if(arg1 == MOUSE_LEFT2) {
          mev.bstate |= BUTTON2_PRESSED;
        } else if(arg1 == MOUSE_LEFT3) {
          mev.bstate |= BUTTON3_PRESSED;
        } else if(arg1 == MOUSE_LEFT4) {
          mev.bstate |= BUTTON4_PRESSED;
        } else if(arg1 == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
          mev.bstate |= BUTTON5_PRESSED;
#else
          mev.bstate |= BUTTON4_PRESSED;
#endif
        }
      }

}

TTYU_INLINE void event_generate(ttyu_data_t *data, ttyu_event_t *event,
    int arg0 , int arg1, int arg2, int arg3, int arg3) {
  switch(arg0) {
    case EVENT_KEY:
      int c = -1;
      if(arg2 & CTRL_CMD) {
        c = KEY_COMMAND;
      } else if(arg1 >= 65 && arg1 <= 90) {
        if(arg2 & CTRL_SHIFT) {
          c = arg1; // A-Z
        } else {
          c = arg1 + 32; // a-z
        }
      } else if(arg1 >= 48 && arg1 <= 57) {
        c = arg1; // 0 - 9
      } else {
        c = ttyu_unix_key(arg1);
      }
      ttyu_event_create_key(event, arg2, (char)c, c, arg1);
      break;
    case EVENT_MOUSEDOWN:
    case EVENT_MOUSEUP:
    case EVENT_MOUSEMOVE:
      ttyu_event_create_mouse(event, arg0, arg1, arg2, arg3, arg4);
      break;
    default:
      // EVENT_NONE, EVENT_ERROR, EVENT_SIGNAL, EVENT_RESIZE, EVENT_MOUSEWHEEL,
      // EVENT_MOUSEHWHEEL
      event->type = EVENT_NONE;
      break;
  }
}

TTYU_INLINE int ttyu_unix_key(int which) {
  #define XXKEY(w, key, shift) if(w == which) {                                \
    return key;                                                                \
  }
  #undef XXKEY
  TTYU_UNIX_KW(XXKEY);
  return WHICH_UNKNOWN;
}

TTYU_INLINE int ttyu_unix_which(int key) {
  #define XXWHICH(which, k, shift) if(k == key) {                              \
    return which;                                                              \
  }
  #undef XXWHICH
  TTYU_UNIX_KW(XXWHICH);
  return WHICH_UNKNOWN;
}

TTYU_INLINE void ttyu_pi_init(ttyu_pi_t *pi) {
  pi->win = initscr();
  pi->mode = MODE_VT100;

  noecho();
  cbreak();
  keypad(pi->win, TRUE);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &pi->old_mmask);
  mouseinterval(0);
  nodelay(pi->win, TRUE);
}

TTYU_INLINE void ttyu_pi_destroy(ttyu_pi_t *pi) {
  echo();
  endwin(pi->win);
}
