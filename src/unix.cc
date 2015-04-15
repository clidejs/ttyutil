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

void getevent(ttyu_js_c *data, ttyu_event_t *event) {
  event->type = EVENT_NONE;
  int c = getch();
  MEVENT mev;

  if(c == ERR) {
    return;
  } else if(c == KEY_RESIZE) {
    ttyu_event_create_resize(event);
  } else if(c == KEY_MOUSE) {
    if(getmouse(&mev) == OK) {
      ttyu_event_create_mouse(event, EVENT_ERROR, 0, mev.x, mev.y, 0);

      // add button control key sequences if possible
      if(mev.bstate & BUTTON_SHIFT) { event->mouse->ctrl |= CTRL_SHIFT; }
      if(mev.bstate & BUTTON_CTRL) { event->mouse->ctrl |= CTRL_CTRL; }
      if(mev.bstate & BUTTON_ALT) { event->mouse->ctrl |= CTRL_ALT; }

      // convert button codes and mev type
      if(mev.bstate & REPORT_MOUSE_POSITION) {
        event->type = EVENT_MOUSEMOVE;
      } else if(mev.bstate & BUTTON1_RELEASED) {
        event->mouse->button = MOUSE_LEFT;
        event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON1_PRESSED) {
        event->mouse->button = MOUSE_LEFT;
        event->type = EVENT_MOUSEDOWN;
      } else if(mev.bstate & BUTTON2_RELEASED) {
        event->mouse->button = MOUSE_LEFT2;
        event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON2_PRESSED) {
        event->mouse->button = MOUSE_LEFT2;
        event->type = EVENT_MOUSEDOWN;
      } else if(mev.bstate & BUTTON3_RELEASED) {
        event->mouse->button = MOUSE_LEFT3;
        event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON3_PRESSED) {
        event->mouse->button = MOUSE_LEFT3;
        event->type = EVENT_MOUSEDOWN;
      } else
#if NCURSES_MOUSE_VERSION > 1
      if(mev.bstate & BUTTON4_RELEASED) {
        event->mouse->button = MOUSE_LEFT4;
        event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON4_PRESSED) {
        event->mouse->button = MOUSE_LEFT4;
        event->type = EVENT_MOUSEDOWN;
      } else if(mev.bstate & BUTTON5_RELEASED) {
        event->mouse->button = MOUSE_RIGHT;
        event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON5_PRESSED) {
        event->mouse->button = MOUSE_RIGHT;
        event->type = EVENT_MOUSEDOWN;
      }
#else
      if(mev.bstate & BUTTON4_RELEASED) {
        event->mouse->button = MOUSE_RIGHT;
        event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON4_PRESSED) {
        event->mouse->button = MOUSE_RIGHT;
        event->type = EVENT_MOUSEDOWN;
      }
#endif
      if(event->type == EVENT_ERROR) {
        // uncaught mouse event
        ttyu_event_destroy(event);
      } else {
        // its VT102
        data->pi.mode = MODE_VT102;
      }
    } else {
      // bad mouse event
    }
  } else {
    char *ch = const_cast<char *>(keyname(c));
    int ctrl = CTRL_NULL;
    int which = WHICH_UNKNOWN;

    if(c >= 48 && c <= 57) {
      which = c; // WHICH_CHAR0 to WHICH_CHAR9
    } else if(c >= 65 && c <= 90) {
      ctrl |= CTRL_SHIFT;
      which = c; // WHICH_CHARA to WHICH_CHARZ
    } else if(c >= 97 && c <= 122) {
      which = c - 32; // WHICH_CHARA to WHICH_CHARZ
    } else if(c == KEY_COMMAND) {
      ctrl |= CTRL_CMD;
    } else if(c == KEY_SCOMMAND) {
      ctrl |= CTRL_CMD | CTRL_SHIFT;
    } else {
      which = ttyu_unix_which(c);
      //if(ch[0] == '^') {
      //  ctrl |= CTRL_CTRL;
      //}
    }
    ttyu_event_create_key(event, ctrl, ch, c, which);
  }
}

bool ungetevent(ttyu_js_c *data, ttyu_event_t *event) {
  switch(event->type) {
    case EVENT_KEY:
      ungetch(event->key->code);
      return TRUE;
      break;
    case EVENT_MOUSEUP:
    case EVENT_MOUSEDOWN:
    case EVENT_MOUSEMOVE:
      MEVENT mev;
      mev.x = event->mouse->x;
      mev.y = event->mouse->y;
      mev.bstate = 0;

      if(event->mouse->ctrl & CTRL_ALT) { mev.bstate |= BUTTON_ALT; }
      if(event->mouse->ctrl & CTRL_CTRL) { mev.bstate |= BUTTON_CTRL; }
      if(event->mouse->ctrl & CTRL_SHIFT) { mev.bstate |= BUTTON_SHIFT; }

      if(event->type == EVENT_MOUSEMOVE) {
        mev.bstate |= REPORT_MOUSE_POSITION;
      } else if(event->type == EVENT_MOUSEUP) {
        if(event->mouse->button == MOUSE_LEFT) {
          mev.bstate |= BUTTON1_RELEASED;
        } else if(event->mouse->button == MOUSE_LEFT2) {
          mev.bstate |= BUTTON2_RELEASED;
        } else if(event->mouse->button == MOUSE_LEFT3) {
          mev.bstate |= BUTTON3_RELEASED;
        } else if(event->mouse->button == MOUSE_LEFT4) {
          mev.bstate |= BUTTON4_RELEASED;
        } else if(event->mouse->button == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
          mev.bstate |= BUTTON5_RELEASED;
#else
          mev.bstate |= BUTTON4_RELEASED;
#endif
        }
      } else if(event->type == EVENT_MOUSEDOWN) {
        if(event->mouse->button == MOUSE_LEFT) {
          mev.bstate |= BUTTON1_PRESSED;
        } else if(event->mouse->button == MOUSE_LEFT2) {
          mev.bstate |= BUTTON2_PRESSED;
        } else if(event->mouse->button == MOUSE_LEFT3) {
          mev.bstate |= BUTTON3_PRESSED;
        } else if(event->mouse->button == MOUSE_LEFT4) {
          mev.bstate |= BUTTON4_PRESSED;
        } else if(event->mouse->button == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
          mev.bstate |= BUTTON5_PRESSED;
#else
          mev.bstate |= BUTTON4_PRESSED;
#endif
        }
      }
      ungetmouse(&mev);
      return TRUE;
      break;
    default:
      // EVENT_NONE, EVENT_ERROR, EVENT_SIGNAL, EVENT_RESIZE, EVENT_MOUSEWHEEL,
      // EVENT_MOUSEHWHEEL
      return FALSE;
      break;
  }
}

void event_generate(ttyu_js_c *data, ttyu_event_t *event,
    int arg0 , int arg1, int arg2, int arg3, int arg4) {
  switch(arg0) {
    case EVENT_KEY: {
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
      }                         // TODO   v
      ttyu_event_create_key(event, arg2, " ", c, arg1);
      } break;
    case EVENT_MOUSEDOWN:
    case EVENT_MOUSEUP:
    case EVENT_MOUSEMOVE: {
      ttyu_event_create_mouse(event, arg0, arg1, arg2, arg3, arg4);
      } break;
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
  TTYU_UNIX_KW(XXKEY);
  return WHICH_UNKNOWN;
}

TTYU_INLINE int ttyu_unix_which(int key) {
  #define XXWHICH(which, k, shift) if(k == key) {                              \
    return which;                                                              \
  }
  TTYU_UNIX_KW(XXWHICH);
  return WHICH_UNKNOWN;
}

void ttyu_pi_init(ttyu_pi_t *pi) {
  pi->win = initscr();
  pi->mode = MODE_VT100;
  DBG("a");
  noecho();
  cbreak();
  keypad(pi->win, TRUE);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &pi->old_mmask);
  mouseinterval(0);
  nodelay(pi->win, TRUE);
}

void ttyu_pi_destroy(ttyu_pi_t *pi) {
  echo();
  endwin();
}

