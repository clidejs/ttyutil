/* ttyutil - win/worker.cc
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

bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    ttyu_js_c *obj) {
  DBG("::execute()");
  DWORD readed;
  INPUT_RECORD ir[WIN_BUFFER_SIZE];
  DWORD i;
  DBG("  start");
  if (obj->err) {
    ttyu_event_t *event =
        reinterpret_cast<ttyu_event_t *>(malloc(sizeof(ttyu_event_t)));
    ttyu_event_create_error(event);
    progress.send(const_cast<const ttyu_event_t *>(event));
    if (obj->err) {
      DBG("  killed");
      return FALSE;
    }
    obj->err->msg = NULL;
  }

  DBG("  read input");

  ReadConsoleInput(obj->hin, ir, WIN_BUFFER_SIZE, &readed);
  if (obj->stop) { DBG("  exited"); return FALSE; }  // exit

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
  DBG("  end");
  return TRUE;
}

void ttyu_worker_c::handle(ttyu_event_t *event) {
  DBG("::handle()");
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
  // TODO emit
}

void ttyu_worker_c::Execute() {
  DBG("::Execute()");
  ttyu_progress_c progress(this);
  DBG("  barrier wait");
  uv_barrier_wait(&obj_->barrier);
  DBG("  start execute loop");
  // loop execute until it returns false (error)
  while (execute(progress, obj_)) continue;
}
