#include <ttyu.h>

void ttyu_data_init(ttyu_data_t *data) {
  data->win = initscr();
  noecho();
  cbreak();
  keypad(data->win, TRUE);
  mousemask(ALL_MOUSE_EVENTS + 1, &(data->old_mouse_mask));
  mouseinterval(0);
  data->ctrl = 0;
  data->mode = MODE_VT100;
}

void ttyu_data_destroy(ttyu_data_t *data) {
  keypad(data->win, FALSE);
  mousemask(data->old_mouse_mask, NULL);
  endwin();
}

bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    ttyu_data_t *data) {
  int c = wgetch(data->win);
  MEVENT mev;
  char ch;
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
      if(mev.bstate & BUTTON1_RELEASED) {
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
        // still could be VT100 (Terminal.app + SIMBL + MouseTerm)
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
    ch = *const_cast<char *>(keyname(c));

    // TODO ctrl chars
    // TODO which
    // TODO c = 258 / 259 on Terminal.app => scrolling!

    ttyu_event_create_key(event, data->ctrl, ch, c, WHICH_UNKNOWN);
    progress.send(const_cast<const ttyu_event_t *>(event));
  }
  return TRUE;
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
  //ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
  int fg = args[1]->IsNumber() ? args[1]->Int32Value() : (
      args[1]->IsString() ? util_color(
      (new v8::String::Utf8Value(args[1]->ToString()))->operator*()) : - 1);
  int bg = args[2]->IsNumber() ? args[2]->Int32Value() : (
      args[2]->IsString() ? util_color(
      (new v8::String::Utf8Value(args[2]->ToString()))->operator*()) : - 1);
  // TODO fix the -Wformat-security warning
  printf(util_render(ch->operator*(), fg, bg));
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
    //int x = util_max(args[0]->Int32Value(), 0);
    //int y = util_max(args[1]->Int32Value(), 0);
    //int width = util_min(args[2]->Int32Value(), COLS);
    //int height = util_min(args[3]->Int32Value(), LINES);

    // TODO!
  } else {
    wclear(obj->data->win);
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
