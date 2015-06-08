/* ttyutil - win.cc - implements methods for windows systems (see include/win.h)
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

ttyu_js_c::ttyu_js_c() : running(FALSE), stop(TRUE), worker(this), top(0) {
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

  DBG("::js_start()");
  uv_mutex_init(&obj->emitlock);
  uv_barrier_init(&obj->barrier, 2);

  obj->hin = GetStdHandle(STD_INPUT_HANDLE);
  obj->hout = GetStdHandle(STD_OUTPUT_HANDLE);

  if (INVALID_HANDLE_VALUE == obj->hin || INVALID_HANDLE_VALUE == obj->hout) {
    NanThrowError("invalid std handles");
  }

  GetConsoleMode(obj->hin, &(obj->old_mode));
  DWORD new_mode = ((obj->old_mode | ENABLE_MOUSE_INPUT |
      ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT) &
      ~(ENABLE_QUICK_EDIT_MODE));
  SetConsoleMode(obj->hin, new_mode);

  DBG("  async queue start");
  NanAsyncQueueWorker(&obj->worker);

  DBG("  wait barrier");
  uv_barrier_wait(&obj->barrier);
  DBG("  destroy barrier");
  uv_barrier_destroy(&obj->barrier);
  DBG("  destroyed barrier");
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_stop) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->running = FALSE;
  obj->stop = TRUE;

  SetConsoleMode(obj->hin, obj->old_mode);

  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_on) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  uv_mutex_lock(&obj->emitlock);
  ee_on(&obj->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  uv_mutex_unlock(&obj->emitlock);
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_off) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  uv_mutex_lock(&obj->emitlock);
  ee_off(&obj->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  uv_mutex_unlock(&obj->emitlock);
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_emit) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  if (obj->running) {
    int ev = args[0]->Int32Value();
    INPUT_RECORD in[1];
    DWORD w;

    in[0].EventType = 0;

    switch (ev) {
      case EVENT_KEY:
        KEY_EVENT_RECORD kev;

        kev.bKeyDown = TRUE;
        kev.wVirtualKeyCode = (WORD)args[1]->Int32Value();
        kev.dwControlKeyState = ttyu_win_state(args[2]->Int32Value());
        kev.uChar.UnicodeChar = static_cast<WCHAR>(kev.wVirtualKeyCode);
        kev.uChar.AsciiChar = static_cast<CHAR>(kev.wVirtualKeyCode);
        kev.wRepeatCount = 1;
        kev.wVirtualScanCode = MapVirtualKey(kev.wVirtualKeyCode,
            MAPVK_VK_TO_VSC);

        in[0].EventType = KEY_EVENT;
        in[0].Event.KeyEvent = kev;
        break;
      case EVENT_RESIZE:
        WINDOW_BUFFER_SIZE_RECORD wev;
        COORD size;

        size.X = args[1]->Int32Value();
        size.Y = args[2]->Int32Value();

        wev.dwSize = size;
        in[0].EventType = WINDOW_BUFFER_SIZE_EVENT;
        in[0].Event.WindowBufferSizeEvent = wev;
        break;
      case EVENT_MOUSEDOWN:
      case EVENT_MOUSEUP:
      case EVENT_MOUSEMOVE:
      case EVENT_MOUSEWHEEL:
      case EVENT_MOUSEHWHEEL:
        MOUSE_EVENT_RECORD mev;
        COORD pos;

        in[0].EventType = MOUSE_EVENT;
        pos.X = static_cast<int16_t>(args[2]->Int32Value());
        pos.Y = static_cast<int16_t>(args[3]->Int32Value() + obj->top);
        mev.dwControlKeyState = ttyu_win_state(args[4]->Int32Value());

        if (ev == EVENT_MOUSEUP) {
          mev.dwButtonState = args[1]->Int32Value();
          mev.dwEventFlags = 0;
        } else if (ev == EVENT_MOUSEDOWN) {
          mev.dwButtonState = args[1]->Int32Value();
          mev.dwEventFlags = 2;
        } else if (ev == EVENT_MOUSEMOVE) {
          mev.dwEventFlags = 1;
        } else if (ev == EVENT_MOUSEWHEEL) {
          mev.dwEventFlags = 4;
        } else if (ev == EVENT_MOUSEHWHEEL) {
          mev.dwEventFlags = 8;
        } else {
          // invalidate event
          in[0].EventType = 0;
        }

        mev.dwMousePosition = pos;
        in[0].Event.MouseEvent = mev;
        break;
      default:  // EVENT_ERROR, EVENT_SIGNAL
        // do nothing
        break;
    }

    if (in[0].EventType != 0) {
      WriteConsoleInput(obj->hin, in, 1, &w);
    }
  }
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_write) {
  NanScope();
  printf("%s", TTYU_TOSTRING(args[0]));
  NanReturnThis();
}


bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    ttyu_js_c *obj) {
  DBG("::execute()", 2);
  DWORD readed;
  INPUT_RECORD ir[WIN_BUFFER_SIZE];
  DWORD i;
  DBG("  start", 2);
  if (obj->err) {
    ttyu_event_t *event =
        reinterpret_cast<ttyu_event_t *>(malloc(sizeof(ttyu_event_t)));
    ttyu_event_create_error(event);
    progress.send(const_cast<const ttyu_event_t *>(event));
    if (obj->err) {
      DBG("  killed", 2);
      return FALSE;
    }
    obj->err->msg = NULL;
  }

  DBG("  read input", 2);

  ReadConsoleInput(obj->hin, ir, WIN_BUFFER_SIZE, &readed);
  if (obj->stop) { DBG("  exited", 2); return FALSE; }  // exit

  for (i = 0; i < readed; ++i) {
    if (MOUSE_EVENT == ir[i].EventType) {
      ttyu_event_t *event =
          reinterpret_cast<ttyu_event_t *>(malloc(sizeof(ttyu_event_t)));
      ttyu_event_create_mouse(event, EVENT_ERROR,
          static_cast<int>(ir[i].Event.MouseEvent.dwButtonState),
          static_cast<int>(ir[i].Event.MouseEvent.dwMousePosition.X),
          static_cast<int>(ir[i].Event.MouseEvent.dwMousePosition.Y - obj->top),
          ttyu_win_ctrl(ir[i].Event.MouseEvent.dwControlKeyState));

      if (ir[i].Event.MouseEvent.dwButtonState == 0 &&
          ir[i].Event.MouseEvent.dwEventFlags == 0) {
        event->type = EVENT_MOUSEUP;
      } else if (ir[i].Event.MouseEvent.dwEventFlags == 0 ||
          ir[i].Event.MouseEvent.dwEventFlags == 2) {
        event->type = EVENT_MOUSEDOWN;
      } else if (ir[i].Event.MouseEvent.dwEventFlags == 1) {
        event->type = EVENT_MOUSEMOVE;
      } else if (ir[i].Event.MouseEvent.dwEventFlags == 4) {
        event->type = EVENT_MOUSEWHEEL;
      } else if (ir[i].Event.MouseEvent.dwEventFlags == 8) {
        event->type = EVENT_MOUSEHWHEEL;
      }

      progress.send(const_cast<const ttyu_event_t *>(event));
    } else if (KEY_EVENT == ir[i].EventType && ir[i].Event.KeyEvent.bKeyDown) {
      ttyu_event_t *event =
          reinterpret_cast<ttyu_event_t *>(malloc(sizeof(ttyu_event_t)));
      char *ch = reinterpret_cast<char *>(malloc(sizeof(char) * 3));
      memcpy(ch, &(ir[i].Event.KeyEvent.uChar.UnicodeChar), sizeof(char) * 2);
      ch[2] = '\0';

      ttyu_event_create_key(event, ttyu_win_ctrl(
              ir[i].Event.KeyEvent.dwControlKeyState), ch,
          static_cast<int>(ir[i].Event.KeyEvent.wVirtualKeyCode),
          ttyu_win_which(ir[i].Event.KeyEvent.wVirtualKeyCode));

      free(ch);

      progress.send(const_cast<const ttyu_event_t *>(event));
    } else if (WINDOW_BUFFER_SIZE_EVENT == ir[i].EventType) {
      ttyu_event_t *event =
          reinterpret_cast<ttyu_event_t *>(malloc(sizeof(ttyu_event_t)));

      if (!ttyu_win_scr_update(obj, FALSE)) {
        ttyu_event_create_error(event);
      } else {
        ttyu_event_create_resize(event);
      }
      progress.send(const_cast<const ttyu_event_t *>(event));
    }
  }
  DBG("  end", 2);
  return TRUE;
}

void ttyu_worker_c::handle(ttyu_event_t *event) {
  DBG("::handle()", 1);
  if (ee_count(&obj_->emitter, event->type) == 0 || obj_->stop ||
      !obj_->running) {
    return;
  }

  v8::Local<v8::Object> obj = NanNew<v8::Object>();
  switch (event->type) {
    case EVENT_RESIZE:
      obj->Set(NanNew<v8::String>("type"), EVENTSTRING_RESIZE);
      break;
    case EVENT_KEY:
      obj->Set(NanNew<v8::String>("type"), EVENTSTRING_KEY);
      obj->Set(NanNew<v8::String>("ctrl"),
          NanNew<v8::Integer>(event->key->ctrl));
      obj->Set(NanNew<v8::String>("char"), NanNew<v8::String>(event->key->c));
      obj->Set(NanNew<v8::String>("code"),
          NanNew<v8::Integer>(event->key->code));
      obj->Set(NanNew<v8::String>("which"),
          NanNew<v8::Integer>(event->key->which));
      break;
    case EVENT_MOUSEDOWN:
    case EVENT_MOUSEUP:
    case EVENT_MOUSEMOVE:
    case EVENT_MOUSEWHEEL:
    case EVENT_MOUSEHWHEEL:
      if (event->type == EVENT_MOUSEDOWN) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEDOWN);
      } else if (event->type == EVENT_MOUSEUP) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEUP);
      } else if (event->type == EVENT_MOUSEMOVE) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEMOVE);
      } else if (event->type == EVENT_MOUSEWHEEL) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEWHEEL);
      } else if (event->type == EVENT_MOUSEHWHEEL) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEHWHEEL);
      }
      obj->Set(NanNew<v8::String>("button"),
          NanNew<v8::Integer>(event->mouse->button));
      obj->Set(NanNew<v8::String>("x"), NanNew<v8::Integer>(event->mouse->x));
      obj->Set(NanNew<v8::String>("y"), NanNew<v8::Integer>(event->mouse->y));
      obj->Set(NanNew<v8::String>("ctrl"),
          NanNew<v8::Integer>(event->mouse->ctrl));
      break;
    default:  // EVENT_ERROR, EVENT_SIGNAL
      obj->Set(NanNew<v8::String>("type"), EVENTSTRING_ERROR);
      obj->Set(NanNew<v8::String>("error"), NanError("TODO"));
      event->type = EVENT_ERROR;
      break;
  }
  uv_mutex_lock(&obj_->emitlock);
  ee_emit(&obj_->emitter, event->type, obj);
  uv_mutex_unlock(&obj_->emitlock);
}

void ttyu_worker_c::Execute() {
  DBG("::Execute()", 1);
  ttyu_progress_c progress(this);
  DBG("  barrier wait", 1);
  uv_barrier_wait(&obj_->barrier);
  DBG("  start execute loop", 1);
  // loop execute until it returns false (error)
  while (execute(progress, obj_)) continue;
}

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
