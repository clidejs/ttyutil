#include <ttyu.h>

void ttyu_data_init(ttyu_data_t *data) {
  DWORD new_mode;

  data->hIn = GetStdHandle(STD_INPUT_HANDLE);
  data->hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  if(INVALID_HANDLE_VALUE == data->hIn || INVALID_HANDLE_VALUE == data->hOut) {
    data->init_error = TRUE;
    return;
  }
  data->init_error = FALSE;

  GetConsoleMode(data->hIn, &(data->old_mode));
  new_mode = ((data->old_mode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS |
          ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT) &
      ~(ENABLE_QUICK_EDIT_MODE));
  SetConsoleMode(data->hIn, new_mode);
}

void ttyu_data_destroy(ttyu_data_t *data) {
  SetConsoleMode(data->hIn, data->old_mode);
}

bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    ttyu_data_t *data) {
  DWORD readed;
  INPUT_RECORD ir[WIN_BUFFER_SIZE];
  CONSOLE_SCREEN_BUFFER_INFO conInfo;
  DWORD i;

  if(data->init_error == TRUE) {
    ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));
    ttyu_event_create_error(event, ERROR_WIN_INIT);
    progress.send(const_cast<const ttyu_event_t *>(event));
    return FALSE;
  }

  ReadConsoleInput(data->hIn, ir, WIN_BUFFER_SIZE, &readed);

  for(i = 0; i < readed; ++i) {
    if(MOUSE_EVENT == ir[i].EventType) {
      ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));
      if(!GetConsoleScreenBufferInfo(data->hOut, &conInfo)) {
        ttyu_event_create_error(event, ERROR_WIN_BUFFER);
        progress.send(const_cast<const ttyu_event_t *>(event));
        return FALSE;
      }
      ttyu_event_create_mouse(event, EVENT_ERROR,
          (int)ir[i].Event.MouseEvent.dwButtonState,
            (int)ir[i].Event.MouseEvent.dwMousePosition.X,
            (int)ir[i].Event.MouseEvent.dwMousePosition.Y -
                (int)conInfo.srWindow.Top,
            ttyu_win_ctrl(ir[i].Event.MouseEvent.dwControlKeyState));

      if(ir[i].Event.MouseEvent.dwButtonState == 0 &&
          ir[i].Event.MouseEvent.dwEventFlags == 0) {
        event->type = EVENT_MOUSEUP;
      } else if(ir[i].Event.MouseEvent.dwEventFlags == 0) {
        event->type = EVENT_MOUSEDOWN;
      } else if(ir[i].Event.MouseEvent.dwEventFlags == 2) {
        event->type = EVENT_MOUSEDOWN;
      } else if(ir[i].Event.MouseEvent.dwEventFlags == 1) {
        event->type = EVENT_MOUSEMOVE;
      } else if(ir[i].Event.MouseEvent.dwEventFlags == 4) {
        event->type = EVENT_MOUSEWHEEL;
      } else if(ir[i].Event.MouseEvent.dwEventFlags == 8) {
        event->type = EVENT_MOUSEHWHEEL;
      }

      progress.send(const_cast<const ttyu_event_t *>(event));
    } else if(KEY_EVENT == ir[i].EventType) {
      ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));
      ttyu_event_create_key(event, ttyu_win_ctrl(
              ir[i].Event.KeyEvent.dwControlKeyState),
          (char)ir[i].Event.KeyEvent.uChar.UnicodeChar,
          (int)ir[i].Event.KeyEvent.wVirtualKeyCode,
          WHICH_UNKNOWN); // TODO which

      progress.send(const_cast<const ttyu_event_t *>(event));
    } else if(WINDOW_BUFFER_SIZE_EVENT == ir[i].EventType) {
      ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));
      ttyu_event_create_resize(event);
      progress.send(const_cast<const ttyu_event_t *>(event));
    }
  }

  return TRUE;
}

int ttyu_win_ctrl(DWORD state) {
  int ctrl = CTRL_NULL;
  if(state | RIGHT_ALT_PRESSED || state | LEFT_ALT_PRESSED) {
    ctrl |= CTRL_ALT;
  }
  if(state | RIGHT_CTRL_PRESSED || state | RIGHT_CTRL_PRESSED) {
    ctrl |= CTRL_CTRL;
  }
  if(state | SHIFT_PRESSED) {
    ctrl |= CTRL_SHIFT;
  }
  if(state | ENHANCED_KEY) {
    ctrl |= CTRL_ENHANCED;
  }
  if(state | NUMLOCK_ON) {
    ctrl |= CTRL_NUMLOCK;
  }
  if(state | SCROLLLOCK_ON) {
    ctrl |= CTRL_SCROLLLOCK;
  }
  if(state | CAPSLOCK_ON) {
    ctrl |= CTRL_CAPSLOCK;
  }
  return ctrl;
}
NAN_GETTER(ttyu_js_c::get_width) {
  NanScope();
  NanReturnUndefined();
}

NAN_GETTER(ttyu_js_c::get_height) {
  NanScope();
  NanReturnUndefined();
}

NAN_GETTER(ttyu_js_c::get_mode) {
  NanScope();
  NanReturnUndefined();
}

NAN_GETTER(ttyu_js_c::get_colors) {
  NanScope();
  NanReturnUndefined();
}

NAN_GETTER(ttyu_js_c::getx) {
  NanScope();
  NanReturnUndefined();
}

NAN_SETTER(ttyu_js_c::setx) {
  NanScope();
}

NAN_GETTER(ttyu_js_c::gety) {
  NanScope();
  NanReturnUndefined();
}

NAN_SETTER(ttyu_js_c::sety) {
  NanScope();
}

NAN_METHOD(ttyu_js_c::gotoxy) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::write) {
  NanScope();
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  printf(ch->operator*());
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::beep) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::clear) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::prepare) {
  NanScope();
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::color) {
  NanScope();
  NanReturnUndefined();
}
