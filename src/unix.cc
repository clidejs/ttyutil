#include <ttyu.h>

void ttyu_data_init(ttyu_data_t *data) {
  data->win = initscr();
  noecho();
  cbreak();
  keypad(data->win, TRUE);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, &(data->old_mouse_mask));
  mouseinterval(0);
  data->mode = MODE_VT100;
}

void ttyu_data_destroy(ttyu_data_t *data) {
  keypad(data->win, FALSE);
  mousemask(data->old_mouse_mask, NULL);
  echo();
  nocbreak();
  endwin();
}

bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    ttyu_data_t *data) {
  int c = wgetch(data->win);
  MEVENT mev;
  ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));

  if(c == ERR) {
    ttyu_event_create_error(event, ERROR_UNIX_UNDEF);
    progress.send(const_cast<const ttyu_event_t *>(event));
  } else if(c == KEY_RESIZE) {
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

    if(c == KEY_DOWN) {
      which = WHICH_DOWN;
    } else if(c == KEY_UP) {
      which = WHICH_UP;
    } else if(c == KEY_LEFT) {
      which = WHICH_LEFT;
    } else if(c == KEY_RIGHT) {
      which = WHICH_RIGHT;
    } else if(c == KEY_HOME) {
      which = WHICH_HOME;
    } else if(c == KEY_BACKSPACE) {
      which = WHICH_BACKSPACE;
    } else if(c == KEY_F(1)) {
      which = WHICH_F1;
    } else if(c == KEY_F(2)) {
      which = WHICH_F2;
    } else if(c == KEY_F(3)) {
      which = WHICH_F3;
    } else if(c == KEY_F(4)) {
      which = WHICH_F4;
    } else if(c == KEY_F(5)) {
      which = WHICH_F5;
    } else if(c == KEY_F(6)) {
      which = WHICH_F6;
    } else if(c == KEY_F(7)) {
      which = WHICH_F7;
    } else if(c == KEY_F(8)) {
      which = WHICH_F8;
    } else if(c == KEY_F(9)) {
      which = WHICH_F9;
    } else if(c == KEY_F(10)) {
      which = WHICH_F10;
    } else if(c == KEY_F(11)) {
      which = WHICH_F11;
    } else if(c == KEY_F(12)) {
      which = WHICH_F12;
    } else if(c == KEY_F(13)) {
      which = WHICH_F13;
    } else if(c == KEY_F(14)) {
      which = WHICH_F14;
    } else if(c == KEY_F(15)) {
      which = WHICH_F15;
    } else if(c == KEY_F(16)) {
      which = WHICH_F16;
    } else if(c == KEY_F(17)) {
      which = WHICH_F17;
    } else if(c == KEY_F(18)) {
      which = WHICH_F18;
    } else if(c == KEY_F(19)) {
      which = WHICH_F19;
    } else if(c == KEY_F(20)) {
      which = WHICH_F20;
    } else if(c == KEY_F(21)) {
      which = WHICH_F21;
    } else if(c == KEY_F(22)) {
      which = WHICH_F22;
    } else if(c == KEY_F(23)) {
      which = WHICH_F23;
    } else if(c == KEY_F(24)) {
      which = WHICH_F24;
    } else if(c == KEY_DC) {
      which = WHICH_DELETE;
    } else if(c == KEY_IC) {
      which = WHICH_INSERT;
    } else if(c == KEY_EIC) {
      which = WHICH_INSERT;
    } else if(c == KEY_CLEAR) {
      which = WHICH_CLEAR;
    } else if(c == KEY_EOS) {
      which = WHICH_CLEAR;
    } else if(c == KEY_EOL) {
      which = WHICH_CLEAR;
    } else if(c == KEY_NPAGE) {
      which = WHICH_NEXT;
    } else if(c == KEY_PPAGE) {
      which = WHICH_PRIOR;
    } else if(c == KEY_STAB) {
      which = WHICH_TAB;
    } else if(c == KEY_ENTER) {
      which = WHICH_ENTER;
    } else if(c == KEY_PRINT) {
      which = WHICH_PRINT;
    } else if(c == KEY_LL) {
      which = WHICH_HOME;
    } else if(c == KEY_BTAB) {
      which = WHICH_BACKSPACE;
    } else if(c == KEY_BEG) {
      which = WHICH_HOME;
    } else if(c == KEY_COMMAND) {
      ctrl |= CTRL_CMD;
    } else if(c == KEY_END) {
      which = WHICH_END;
    } else if(c == KEY_HELP) {
      which = WHICH_HELP;
    } else if(c == KEY_REFRESH) {
      which = WHICH_BROWSER_REFRESH;
    } else if(c == KEY_SBEG) {
      which = WHICH_HOME;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SCOMMAND) {
      ctrl |= CTRL_SHIFT | CTRL_CMD;
    } else if(c == KEY_SDC) {
      which = WHICH_DELETE;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SEND) {
      which = WHICH_END;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SEOL) {
      which = WHICH_CLEAR;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SHELP) {
      which = WHICH_HELP;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SHOME) {
      which = WHICH_HOME;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SIC) {
      which = WHICH_INSERT;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SLEFT) {
      which = WHICH_LEFT;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SNEXT) {
      which = WHICH_NEXT;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SPREVIOUS) {
      which = WHICH_PRIOR;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SPRINT) {
      which = WHICH_PRINT;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SRIGHT) {
      which = WHICH_RIGHT;
      ctrl |= CTRL_SHIFT;
    } else if(c == KEY_SF) {
      which = WHICH_DOWN;
    } else if(c == KEY_SR) {
      which = WHICH_UP;
    } else {
      if(c >= 65 && c <= 90) {
        ctrl |= CTRL_SHIFT;
        which = c; // WHICH_CHARA to WHICH_CHARZ
      } else if(c >= 97 && c <= 122) {
        which = c - 32; // WHICH_CHARA to WHICH_CHARZ
      } else {
        which = c; // try direct mapping
      }
      if(ch[0] == '^') {
        ctrl |= CTRL_CTRL;
      }
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

NAN_GETTER(ttyu_js_c::get_width) {
  NanScope();
  NanReturnValue(NanNew<v8::Number>(COLS));
}

NAN_GETTER(ttyu_js_c::get_height) {
  NanScope();
  NanReturnValue(NanNew<v8::Number>(LINES));
}

NAN_GETTER(ttyu_js_c::get_mode) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Number>(obj->data->mode));
}

NAN_GETTER(ttyu_js_c::get_colors) {
  NanScope();
  NanReturnValue(NanNew<v8::Number>(COLORS));
}

NAN_GETTER(ttyu_js_c::getx) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Number>(obj->data->win->_curx));
}

NAN_SETTER(ttyu_js_c::setx) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->data->win->_curx = args.Data()->Int32Value();
}

NAN_GETTER(ttyu_js_c::gety) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Number>(obj->data->win->_cury));
}

NAN_SETTER(ttyu_js_c::sety) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->data->win->_cury = args.Data()->Int32Value();
}

NAN_METHOD(ttyu_js_c::gotoxy) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  wmove(obj->data->win, args[1]->Int32Value(), args[0]->Int32Value());
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::write) {
  NanScope();
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
  ::beep();
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::clear) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());

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
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  NanReturnValue(NanNew<v8::Integer>(util_color(ch->operator*())));
}
