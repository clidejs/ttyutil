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

ttyu_js_c::ttyu_js_c() : running(FALSE), stop(TRUE), mode(MODE_VT100) {
  ee_init(&emitter, ttyu_ee_cb_call, ttyu_ee_compare);
}

ttyu_js_c::~ttyu_js_c() {
  running = FALSE;
  stop = TRUE;
  ee_destroy(&emitter);
}

NAN_METHOD(ttyu_js_c::js_start) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->running = TRUE;
  obj->stop = FALSE;
  obj->worker_run = TRUE;
  obj->win = initscr();

  uv_barrier_init(&obj->barrier, 3);
  uv_mutex_init(&obj->emitlock);
  uv_mutex_init(&obj->emitstacklock);
  uv_mutex_init(&obj->ungetlock);
  uv_cond_init(&obj->condition);

  uv_thread_create(&obj->curses_thread, ttyu_js_c::curses_thread_func, obj);
  obj->check_queue();

  uv_barrier_wait(&obj->barrier);
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_stop) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->running = FALSE;
  obj->stop = TRUE;

  uv_thread_join(&obj->curses_thread);
  uv_mutex_destroy(&obj->emitlock);
  uv_mutex_destroy(&obj->emitstacklock);
  uv_mutex_destroy(&obj->ungetlock);
  uv_cond_destroy(&obj->condition);
  uv_barrier_destroy(&obj->barrier);
  endwin();
  delwin(obj->win);

  DBG("window deleted");
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_on) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  uv_mutex_lock(&obj->emitlock);
  {
    ee_on(&obj->emitter, args[0]->Int32Value(),
        new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  }
  uv_mutex_unlock(&obj->emitlock);
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_off) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  uv_mutex_lock(&obj->emitlock);
  {
    ee_off(&obj->emitter, args[0]->Int32Value(),
        new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  }
  uv_mutex_unlock(&obj->emitlock);
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_emit) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  int arg0 = args[0]->Int32Value();
  int arg1 = args[1]->Int32Value();
  int arg2 = args[2]->Int32Value();
  int arg3 = args[3]->Int32Value();
  int arg4 = args[4]->Int32Value();
  ttyu_event_t event;
  switch (arg0) {
    case EVENT_KEY: {
      int c = -1;
      if (arg2 & CTRL_CMD) {
        c = KEY_COMMAND;
      } else if (arg1 >= 65 && arg1 <= 90) {
        if (arg2 & CTRL_SHIFT) {
          c = arg1;  // A-Z
        } else {
          c = arg1 + 32;  // a-z
        }
      } else if (arg1 >= 48 && arg1 <= 57) {
        c = arg1;  // 0 - 9
      } else {
        c = ttyu_unix_key(arg1);
      }  // TODO(@bbuecherl)               v
      ttyu_event_create_key(&event, arg2, " ", c, arg1);
      } break;
    case EVENT_MOUSEDOWN:
    case EVENT_MOUSEUP:
    case EVENT_MOUSEMOVE: {
      ttyu_event_create_mouse(&event, arg0, arg1, arg2, arg3, arg4);
      } break;
    default:
      // EVENT_NONE, EVENT_ERROR, EVENT_SIGNAL, EVENT_RESIZE, EVENT_MOUSEWHEEL,
      // EVENT_MOUSEHWHEEL
      event.type = EVENT_NONE;
      break;
  }

  if (event.type != EVENT_NONE) {
    uv_mutex_lock(&obj->ungetlock);
    {
      obj->unget_stack.push(event);
    }
    uv_mutex_unlock(&obj->ungetlock);
  }
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_write) {
  NanScope();
  printf("%s", TTYU_TOSTRING(args[0]));
  refresh();
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_getwidth) {
  NanScope();
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  NanReturnValue(NanNew<v8::Number>(w.ws_col));
}

NAN_METHOD(ttyu_js_c::js_getheight) {
  NanScope();
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  NanReturnValue(NanNew<v8::Number>(w.ws_row));
}

NAN_METHOD(ttyu_js_c::js_setwidth) {
  NanScope();
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  resizeterm(args[0]->Int32Value(), w.ws_row);
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::js_setheight) {
  NanScope();
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  resizeterm(w.ws_col, args[0]->Int32Value());
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::js_setx) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::js_getx) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::js_sety) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::js_gety) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::js_mode) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Number>(obj->mode));
}

void ttyu_js_c::check_queue() {
  if (running && !stop) {
    NanAsyncQueueWorker(new ttyu_worker_c(this));
  }
}

void ttyu_worker_c::Execute() {
  uv_mutex_lock(&obj->emitstacklock);
  {
    if (obj->worker_run) {
      obj->worker_run = FALSE;
      uv_barrier_wait(&obj->barrier);
    }

    while (obj->emit_stack.size() == 0) {
      uv_cond_wait(&obj->condition, &obj->emitstacklock);
    }

    SDBG("::Execute %zu", obj->emit_stack.size());

    // copy stack into emit_worker and clear stack
    std::copy(obj->emit_stack.begin(), obj->emit_stack.end(),
        std::back_inserter(emit_stack));
    obj->emit_stack.clear();
  }
  uv_mutex_unlock(&obj->emitstacklock);
}

void ttyu_worker_c::HandleOKCallback() {
  NanScope();
  for (std::vector<ttyu_event_t>::size_type i = 0;
      i < emit_stack.size(); ++i) {
    ttyu_event_t event = emit_stack[i];
    SDBG("::HandleOKCallback %d %d", i, event.type);

    if (ee_count(&obj->emitter, event.type) == 0 ||
        event.type == EVENT_NONE) {
      continue;  // fast skip
    }

    v8::Local<v8::Object> jsobj = NanNew<v8::Object>();
    switch (event.type) {
      case EVENT_RESIZE:
        jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_RESIZE);
        break;
      case EVENT_KEY:
        jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_KEY);
        jsobj->Set(NanNew<v8::String>("ctrl"),
            NanNew<v8::Integer>(event.key->ctrl));
        jsobj->Set(NanNew<v8::String>("char"),
            NanNew<v8::String>(event.key->c));
        jsobj->Set(NanNew<v8::String>("code"),
            NanNew<v8::Integer>(event.key->code));
        jsobj->Set(NanNew<v8::String>("which"),
            NanNew<v8::Integer>(event.key->which));
        break;
      case EVENT_MOUSEDOWN:
      case EVENT_MOUSEUP:
      case EVENT_MOUSEMOVE:
      case EVENT_MOUSEWHEEL:
      case EVENT_MOUSEHWHEEL:
        if (event.type == EVENT_MOUSEDOWN) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEDOWN);
        } else if (event.type == EVENT_MOUSEUP) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEUP);
        } else if (event.type == EVENT_MOUSEMOVE) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEMOVE);
        } else if (event.type == EVENT_MOUSEWHEEL) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEWHEEL);
        } else if (event.type == EVENT_MOUSEHWHEEL) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEHWHEEL);
        }
        jsobj->Set(NanNew<v8::String>("button"),
            NanNew<v8::Integer>(event.mouse->button));
        jsobj->Set(NanNew<v8::String>("x"),
            NanNew<v8::Integer>(event.mouse->x));
        jsobj->Set(NanNew<v8::String>("y"),
            NanNew<v8::Integer>(event.mouse->y));
        jsobj->Set(NanNew<v8::String>("ctrl"),
            NanNew<v8::Integer>(event.mouse->ctrl));
        break;
      default:  // EVENT_ERROR, EVENT_SIGNAL
        jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_ERROR);
        jsobj->Set(NanNew<v8::String>("error"), NanError("..."));
        event.type = EVENT_ERROR;
        break;
    }

    ee_emit(&obj->emitter, event.type, jsobj);
  }
  emit_stack.clear();
  obj->check_queue();
}

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

TTYU_INLINE int ttyu_unix_key(int which) {
  #define XXKEY(w, key, shift) if (w == which) {                               \
    return key;                                                                \
  }
  TTYU_UNIX_KW(XXKEY);
  #undef XXKEY
  return WHICH_UNKNOWN;
}

TTYU_INLINE int ttyu_unix_which(int key) {
  #define XXWHICH(which, k, shift) if (k == key) {                             \
    return which;                                                              \
  }
  TTYU_UNIX_KW(XXWHICH);
  #undef XXWHICH
  return WHICH_UNKNOWN;
}
