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

void ttyu_data_init(ttyu_data_t *data) {
  data->win = initscr();
  data->closing = FALSE;
  data->mode = MODE_VT100;

//  data->ungetch_stack = new std::queue< int, std::list<int> >;
//  data->ungetmouse_stack = new std::queue< MEVENT, std::list<MEVENT> >;

  noecho();
  cbreak();
  keypad(data->win, TRUE);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &(data->old_mouse_mask));
  mouseinterval(0);
  nodelay(data->win, TRUE);
}

void ttyu_data_destroy(ttyu_data_t *data) {
  data->closing = TRUE;
  echo();
  while(!data->ungetch_stack.empty()) data->ungetch_stack.pop();
  while(!data->ungetmouse_stack.empty()) data->ungetmouse_stack.pop();
  data->ungetch_stack.push(TTYU_EXIT);
  endwin();
}

bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    ttyu_data_t *data) {
  int c = getch();

  if(c == TTYU_EXIT) { return FALSE; }
  if(data->closing) { return FALSE; }

  if(c == ERR) {
    if(data->ungetch_stack.empty()) {
      if(!(data->ungetmouse_stack.empty())) {
        // work on mouse stack
        //ungetmouse(&(data->ungetmouse_stack->pop()));
      }
    } else {
      // work on the char stack
      c = *data->ungetch_stack.pop();
      ungetch(c == -1 ? TTYU_UNKNOWN : c);
    }
    return TRUE;
  }

  MEVENT mev;
  ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));

  if(c == KEY_RESIZE) {
    ttyu_event_create_resize(event);
    progress.send(const_cast<const ttyu_event_t *>(event));
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
        ttyu_event_create_error(event, ERROR_UNIX_MOUSEUNCAUGHT);
      } else {
        // its VT102
        data->mode = MODE_VT102;
      }
    } else {
      // bad mouse event
      ttyu_event_create_error(event, ERROR_UNIX_MOUSEBAD);
    }
    progress.send(const_cast<const ttyu_event_t *>(event));
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
    progress.send(const_cast<const ttyu_event_t *>(event));
  }
  return TRUE;
}

void ttyu_unix_clrscr(ttyu_data_t *data, int x, int y, int width, int height) {
  if(x == 0 && y == 0 && width == COLS && height == LINES) {
    wclear(data->win);
  } else if(width != 0 || height != 0 || x < COLS || y < LINES) {
    int ox = data->win->_curx;
    int oy = data->win->_cury;
    wmove(data->win, x, y);

    for(int j = 0; j < height; ++j) {
      for(int i = 0; i < width; ++i) {
        std::cout << " ";
      }
      std::cout << "\r\n";
    }
    wmove(data->win, ox, oy);
  }
}

int ttyu_unix_key(int which) {
  #define XXKEY(w, key, shift) if(w == which) {                                \
    return key;                                                                \
  }
  TTYU_UNIX_KW(XXKEY);
  return WHICH_UNKNOWN;
}

int ttyu_unix_which(int key) {
  #define XXWHICH(which, k, shift) if(k == key) {                              \
    return which;                                                              \
  }
  TTYU_UNIX_KW(XXWHICH);
  return WHICH_UNKNOWN;
}

NAN_METHOD(ttyu_js_c::emit) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  if(obj->running) {
    int ev = args[0]->Int32Value();

    switch(ev) {
      case EVENT_KEY: {
        int c = -1;
        int which = args[1]->Int32Value();
        int ctrl = args[2]->Int32Value();
        if(ctrl & CTRL_CMD) {
          c = KEY_COMMAND;
        } else if(which >= 65 && which <= 90) {
          if(ctrl & CTRL_SHIFT) {
            c = which; // A-Z
          } else {
            c = which + 32; // a-z
          }
        } else if(which >= 48 && which <= 57) {
          c = which; // 0 - 9
        } else {
          c = ttyu_unix_key(which);
        }
        obj->data->ungetch_stack.push(c);
        } break;
      case EVENT_MOUSEDOWN:
      case EVENT_MOUSEUP:
      case EVENT_MOUSEMOVE: {
        MEVENT mev;
        int b = args[1]->Int32Value();
        int ctrl = args[4]->Int32Value();

        mev.x = (short)args[2]->Int32Value();
        mev.y = (short)args[3]->Int32Value();
        mev.bstate = 0;

        if(ctrl & CTRL_ALT) { mev.bstate |= BUTTON_ALT; }
        if(ctrl & CTRL_CTRL) { mev.bstate |= BUTTON_CTRL; }
        if(ctrl & CTRL_SHIFT) { mev.bstate |= BUTTON_SHIFT; }

        if(ev == EVENT_MOUSEMOVE) {
          mev.bstate |= REPORT_MOUSE_POSITION;
        } else if(ev == EVENT_MOUSEUP) {
          if(b == MOUSE_LEFT) {
            mev.bstate |= BUTTON1_RELEASED;
          } else if(b == MOUSE_LEFT2) {
            mev.bstate |= BUTTON2_RELEASED;
          } else if(b == MOUSE_LEFT3) {
            mev.bstate |= BUTTON3_RELEASED;
          } else if(b == MOUSE_LEFT4) {
            mev.bstate |= BUTTON4_RELEASED;
          } else if(b == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
            mev.bstate |= BUTTON5_RELEASED;
#else
            mev.bstate |= BUTTON4_RELEASED;
#endif
          }
        } else if(ev == EVENT_MOUSEDOWN) {
          if(b == MOUSE_LEFT) {
            mev.bstate |= BUTTON1_PRESSED;
          } else if(b == MOUSE_LEFT2) {
            mev.bstate |= BUTTON2_PRESSED;
          } else if(b == MOUSE_LEFT3) {
            mev.bstate |= BUTTON3_PRESSED;
          } else if(b == MOUSE_LEFT4) {
            mev.bstate |= BUTTON4_PRESSED;
          } else if(b == MOUSE_RIGHT) {
#if NCURSES_MOUSE_VERSION > 1
            mev.bstate |= BUTTON5_PRESSED;
#else
            mev.bstate |= BUTTON4_PRESSED;
#endif
          }
        }

        obj->data->ungetmouse_stack.push(mev);
        } break;
      default: // EVENT_ERROR, EVENT_SIGNAL, EVENT_RESIZE, EVENT_MOUSEWHEEL,
               // EVENT_MOUSEHWHEEL
        // do nothing
        break;
    }
  }
  NanReturnUndefined();
}

NAN_GETTER(ttyu_js_c::get_width) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  NanReturnValue(NanNew<v8::Number>(COLS));
}

NAN_GETTER(ttyu_js_c::get_height) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  NanReturnValue(NanNew<v8::Number>(LINES));
}

NAN_GETTER(ttyu_js_c::get_mode) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  NanReturnValue(NanNew<v8::Number>(obj->data->mode));
}

NAN_GETTER(ttyu_js_c::get_colors) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  NanReturnValue(NanNew<v8::Number>(COLORS));
}

NAN_GETTER(ttyu_js_c::getx) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  NanReturnValue(NanNew<v8::Number>(obj->data->win->_curx));
}

NAN_SETTER(ttyu_js_c::setx) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING_VOID(obj);
  obj->data->win->_curx = args.Data()->Int32Value();
}

NAN_GETTER(ttyu_js_c::gety) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  NanReturnValue(NanNew<v8::Number>(obj->data->win->_cury));
}

NAN_SETTER(ttyu_js_c::sety) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING_VOID(obj);
  obj->data->win->_cury = args.Data()->Int32Value();
}

NAN_METHOD(ttyu_js_c::gotoxy) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  wmove(obj->data->win, args[1]->Int32Value(), args[0]->Int32Value());
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::write) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  int fg = args[1]->IsNumber() ? args[1]->Int32Value() : (
      args[1]->IsString() ? util_color(
      (new v8::String::Utf8Value(args[1]->ToString()))->operator*()) : - 1);
  int bg = args[2]->IsNumber() ? args[2]->Int32Value() : (
      args[2]->IsString() ? util_color(
      (new v8::String::Utf8Value(args[2]->ToString()))->operator*()) : - 1);
  printf("%s", util_render(ch->operator*(), fg, bg));
  refresh();
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::beep) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  ::beep();
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::clear) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);

  if(args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber() &&
      args[3]->IsNumber()) {
    ttyu_unix_clrscr(obj->data,
        util_max(args[0]->Int32Value(), 0),     // x
        util_max(args[1]->Int32Value(), 0),     // y
        util_min(args[2]->Int32Value(), COLS),  // width
        util_min(args[3]->Int32Value(), LINES));// height
  } else {
    ttyu_unix_clrscr(obj->data, 0, 0, COLS, LINES);
  }
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::prepare) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  int fg = args[1]->IsNumber() ? args[1]->Int32Value() : (
      args[1]->IsString() ? util_color(
      (new v8::String::Utf8Value(args[1]->ToString()))->operator*()) : - 1);
  int bg = args[2]->IsNumber() ? args[2]->Int32Value() : (
      args[2]->IsString() ? util_color(
      (new v8::String::Utf8Value(args[2]->ToString()))->operator*()) : - 1);
  NanReturnValue(NanNew<v8::String>(util_render(ch->operator*(), fg, bg)));
}

NAN_METHOD(ttyu_js_c::color) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  TTYU_THROW_IF_NOT_RUNNING(obj);
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  NanReturnValue(NanNew<v8::Integer>(util_color(ch->operator*())));
}
