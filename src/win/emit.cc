/* ttyutil - win/emit.cc
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

NAN_METHOD(ttyu_js_c::js_emit) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  THROW_IF_STOPPED(obj);
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
