/* ttyutil - unix/emit.cc
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
  int arg0 = args[0]->Int32Value();
  int arg1 = args[1]->Int32Value();
  int arg2 = args[2]->Int32Value();
  int arg3 = args[3]->Int32Value();
  int arg4 = args[4]->Int32Value();
  ttyu_event_t event;
  THROW_IF_STOPPED(obj);
  switch (arg0) {
    case EVENT_KEY: {
      int c = -1;
      char ch = 0x20;
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
      ttyu_event_create_key(&event, arg2, &ch, c, arg1);
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
    MUTEX_LOCK(&obj->ungetlock, {
      obj->unget_stack.push(event);
    });
  }
  NanReturnThis();
}
