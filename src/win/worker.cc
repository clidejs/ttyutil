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
  DWORD i;
  DWORD type;
  INPUT_RECORD ir[WIN_BUFFER_SIZE];

  ReadConsoleInput(obj->hin, ir, WIN_BUFFER_SIZE, &readed);
  if (obj->stop) { return FALSE; } // exit
  for (i = 0; i < readed; ++i) {
    if (MOUSE_EVENT == ir[i].EventType) {
      type = MOUSE_EVENT;
    } else if (KEY_EVENT == ir[i].EventType && ir[i].Event.KeyEvent.bKeyDown) {
      type = KEY_EVENT;
    } else if (WINDOW_BUFFER_SIZE_EVENT == ir[i].EventType) {
      type = WINDOW_BUFFER_SIZE_EVENT;
    } else {
      SDBG("  uncaught event %d", ir[i].EventType);
      continue;
    }
    SDBG("  capturing event (type: %d)", type);
    ttyu_event_t event;
    switch (type) {
      case MOUSE_EVENT:
        ttyu_event_create_mouse(&event, EVENT_ERROR,
            static_cast<int>(ir[i].Event.MouseEvent.dwButtonState),
            static_cast<int>(ir[i].Event.MouseEvent.dwMousePosition.X),
            static_cast<int>(ir[i].Event.MouseEvent.dwMousePosition.Y-obj->top),
            ttyu_win_ctrl(ir[i].Event.MouseEvent.dwControlKeyState));

        if (ir[i].Event.MouseEvent.dwButtonState == 0 &&
            ir[i].Event.MouseEvent.dwEventFlags == 0) {
          event.type = EVENT_MOUSEUP;
        } else if (ir[i].Event.MouseEvent.dwEventFlags == 0 ||
            ir[i].Event.MouseEvent.dwEventFlags == 2) {
          event.type = EVENT_MOUSEDOWN;
        } else if (ir[i].Event.MouseEvent.dwEventFlags == 1) {
          event.type = EVENT_MOUSEMOVE;
        } else if (ir[i].Event.MouseEvent.dwEventFlags == 4) {
          event.type = EVENT_MOUSEWHEEL;
        } else if (ir[i].Event.MouseEvent.dwEventFlags == 8) {
          event.type = EVENT_MOUSEHWHEEL;
        }
        break;
      case KEY_EVENT: {
        char ch[3];
        size_t ret = wcstombs(ch, &(ir[i].Event.KeyEvent.uChar.UnicodeChar),
            sizeof(char) * 2);
        ch[ret-1] = '\0';

        ttyu_event_create_key(&event,
            ttyu_win_ctrl(ir[i].Event.KeyEvent.dwControlKeyState), ch,
            static_cast<int>(ir[i].Event.KeyEvent.wVirtualKeyCode),
            ttyu_win_which(ir[i].Event.KeyEvent.wVirtualKeyCode));
        free(ch);
        } break;
      default:  // WINDOW_BUFFER_SIZE_EVENT
        if (!ttyu_win_scr_update(obj, FALSE)) {
          ttyu_event_create_error(&event);
        } else {
          ttyu_event_create_resize(&event);
        }
        break;
    }

    progress.send(const_cast<const ttyu_event_t *>(&event));
  }
  return TRUE;
}

void ttyu_worker_c::handle(ttyu_event_t *event) {
  NanScope();
  EMIT_EVENT_OBJECT(event, obj_->emitter);
}

void ttyu_worker_c::Execute() {
  ttyu_progress_c progress(this);
  uv_barrier_wait(&obj_->barrier);
  // loop execute until it returns false (error)
  while (execute(progress, obj_)) continue;
}
