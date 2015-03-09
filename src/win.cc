#include <ttyu.h>

void ttyu_data_init(ttyu_data_t *data) {
  DWORD new_mode;

  data->err = (ttyu_error_t *)malloc(sizeof(ttyu_error_t));
  data->hin = GetStdHandle(STD_INPUT_HANDLE);
  data->hout = GetStdHandle(STD_OUTPUT_HANDLE);

  if(INVALID_HANDLE_VALUE == data->hin || INVALID_HANDLE_VALUE == data->hout) {
    data->err->msg = ERROR_WIN_INIT;
    data->err->kill = TRUE;
    return;
  }
  data->err->msg = NULL;
  data->err->kill = FALSE;

  GetConsoleMode(data->hin, &(data->old_mode));
  new_mode = ((data->old_mode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS |
          ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT) &
      ~(ENABLE_QUICK_EDIT_MODE));
  SetConsoleMode(data->hin, new_mode);

  ttyu_win_scr_update(data, TRUE);
}

void ttyu_data_destroy(ttyu_data_t *data) {
  SetConsoleMode(data->hin, data->old_mode);
}

bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    ttyu_data_t *data) {
  DWORD readed;
  INPUT_RECORD ir[WIN_BUFFER_SIZE];
  DWORD i;

  if(data->err->msg) {
    ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));
    ttyu_event_create_error(event, data->err->msg);
    progress.send(const_cast<const ttyu_event_t *>(event));
    if(data->err->kill) {
      return FALSE;
    }
    data->err->msg = NULL;
    data->err->kill = FALSE;
  }

  ReadConsoleInput(data->hin, ir, WIN_BUFFER_SIZE, &readed);

  for(i = 0; i < readed; ++i) {
    if(MOUSE_EVENT == ir[i].EventType) {
      ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));
      ttyu_event_create_mouse(event, EVENT_ERROR,
          (int)ir[i].Event.MouseEvent.dwButtonState,
          (int)ir[i].Event.MouseEvent.dwMousePosition.X,
          (int)ir[i].Event.MouseEvent.dwMousePosition.Y - data->top,
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
    } else if(KEY_EVENT == ir[i].EventType && ir[i].Event.KeyEvent.bKeyDown) {
      ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));
      ttyu_event_create_key(event, ttyu_win_ctrl(
              ir[i].Event.KeyEvent.dwControlKeyState),
          (char)ir[i].Event.KeyEvent.uChar.UnicodeChar,
          (int)ir[i].Event.KeyEvent.wVirtualKeyCode,
          WHICH_UNKNOWN); // TODO which

      progress.send(const_cast<const ttyu_event_t *>(event));
    } else if(WINDOW_BUFFER_SIZE_EVENT == ir[i].EventType) {
      ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));

      if(!ttyu_win_scr_update(data)) {
        ttyu_event_create_error(event, ERROR_WIN_GET);
      } else {
        ttyu_event_create_resize(event);
      }
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

bool ttyu_win_scr_update(ttyu_data_t *data, bool initial) {
  if(initial) {
    CONSOLE_SCREEN_BUFFER_INFOEX con_info;

    if(!GetConsoleScreenBufferInfoEx(data->hout, &con_info)) {
      DBG(GetLastError());
      data->err->msg = ERROR_WIN_GET;
      //data->err->kill = TRUE;
      //return FALSE;
    }

    data->top = (int)con_info.srWindow.Top;
    data->width = (int)con_info.dwSize.X;
    data->height = (int)con_info.dwSize.Y - data->top;

    data->curx = (int)con_info.dwCursorPosition.X;
    data->cury = (int)con_info.dwCursorPosition.Y - data->top;

    for(int i = 0; i < WIN_COLORS; ++i) {
      data->initial_color_table[i] = data->color_table[i] =
          (unsigned long) con_info.ColorTable[i];
    }
  } else {
    CONSOLE_SCREEN_BUFFER_INFO con_info;

    if(!GetConsoleScreenBufferInfo(data->hout, &con_info)) {
      data->err->msg = ERROR_WIN_GET;
      data->err->kill = TRUE;
      return FALSE;
    }

    data->top = (int)con_info.srWindow.Top;
    data->width = (int)con_info.dwSize.X;
    data->height = (int)con_info.dwSize.Y - data->top;

    data->curx = (int)con_info.dwCursorPosition.X;
    data->cury = (int)con_info.dwCursorPosition.Y - data->top;
  }
  return TRUE;
}

int ttyu_win_color(char *c, ttyu_data_t *data) {
  // TODO
  return 0;
}

const char *ttyu_win_render(char *c, int fg, int bg) {
  // TODO
  return c;
}

NAN_GETTER(ttyu_js_c::get_width) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Integer>(obj->data->width));
}

NAN_GETTER(ttyu_js_c::get_height) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Integer>(obj->data->height));
}

NAN_GETTER(ttyu_js_c::get_mode) {
  NanScope();
  NanReturnValue(NanNew<v8::Integer>(MODE_CMD));
}

NAN_GETTER(ttyu_js_c::get_colors) {
  NanScope();
  NanReturnValue(NanNew<v8::Integer>(WIN_COLORS));
}

NAN_GETTER(ttyu_js_c::getx) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  ttyu_win_scr_update(obj->data);
  NanReturnValue(NanNew<v8::Integer>(obj->data->curx));
}

NAN_SETTER(ttyu_js_c::setx) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->data->curx = args.Data()->Int32Value();

  CONSOLE_SCREEN_BUFFER_INFOEX con_info;
  if(GetConsoleScreenBufferInfoEx(obj->data->hout, &con_info)) {
    con_info.dwCursorPosition.X = obj->data->curx;
    if(!SetConsoleScreenBufferInfoEx(obj->data->hout, &con_info)) {
      obj->data->err->msg = ERROR_WIN_SET;
    }
  } else {
    obj->data->err->msg = ERROR_WIN_GET;
  }
}

NAN_GETTER(ttyu_js_c::gety) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  ttyu_win_scr_update(obj->data);
  NanReturnValue(NanNew<v8::Integer>(obj->data->cury));
}

NAN_SETTER(ttyu_js_c::sety) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->data->cury = args.Data()->Int32Value();

  CONSOLE_SCREEN_BUFFER_INFOEX con_info;
  if(GetConsoleScreenBufferInfoEx(obj->data->hout, &con_info)) {
    con_info.dwCursorPosition.Y = obj->data->cury;
    if(!SetConsoleScreenBufferInfoEx(obj->data->hout, &con_info)) {
      obj->data->err->msg = ERROR_WIN_SET;
    }
  } else {
    obj->data->err->msg = ERROR_WIN_GET;
  }
}

NAN_METHOD(ttyu_js_c::gotoxy) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->data->curx = args[0]->Int32Value();
  obj->data->cury = args[1]->Int32Value();

  CONSOLE_SCREEN_BUFFER_INFOEX con_info;
  if(GetConsoleScreenBufferInfoEx(obj->data->hout, &con_info)) {
    con_info.dwCursorPosition.X = obj->data->curx;
    con_info.dwCursorPosition.Y = obj->data->cury;
    if(!SetConsoleScreenBufferInfoEx(obj->data->hout, &con_info)) {
      obj->data->err->msg = ERROR_WIN_SET;
    }
  } else {
    obj->data->err->msg = ERROR_WIN_GET;
  }
  NanReturnUndefined();
}

NAN_METHOD(ttyu_js_c::write) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  int fg = args[1]->IsNumber() ? args[1]->Int32Value() : (
      args[1]->IsString() ? ttyu_win_color(
      (new v8::String::Utf8Value(args[1]->ToString()))->operator*(), obj->data)
          : - 1);
  int bg = args[2]->IsNumber() ? args[2]->Int32Value() : (
      args[2]->IsString() ? ttyu_win_color(
      (new v8::String::Utf8Value(args[2]->ToString()))->operator*(), obj->data)
          : - 1);
  // TODO WriteConsoleOutput
  printf(ttyu_win_render(ch->operator*(), fg, bg));
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::beep) {
  NanScope();
  Beep(750, 300);
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::clear) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  int x = 0;
  int y = 0;
  int width = obj->data->width;
  int height = obj->data->height;

  if(args.length == 4 && args[0]->IsNumber() && args[1]->IsNumber() &&
      args[2]->IsNumber() && args[3]->IsNumber()) {
    x = util_max(args[0]->Int32Value(), x);
    y = util_max(args[1]->Int32Value(), y);
    width = util_min(args[2]->Int32Value(), width);
    height = util_min(args[3]->Int32Value(), height);
  }

  // TODO?!
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::prepare) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  int fg = args[1]->IsNumber() ? args[1]->Int32Value() : (
      args[1]->IsString() ? ttyu_win_color(
      (new v8::String::Utf8Value(args[1]->ToString()))->operator*(), obj->data)
          : - 1);
  int bg = args[2]->IsNumber() ? args[2]->Int32Value() : (
      args[2]->IsString() ? ttyu_win_color(
      (new v8::String::Utf8Value(args[2]->ToString()))->operator*(), obj->data)
          : - 1);
  NanReturnValue(NanNew<v8::String>(ttyu_win_render(ch->operator*(), fg, bg)));
}

NAN_METHOD(ttyu_js_c::color) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  NanReturnValue(NanNew<v8::Integer>(ttyu_win_color(ch->operator*(),
      obj->data)));
}
