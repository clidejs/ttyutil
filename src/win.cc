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

  obj->hin = GetStdHandle(STD_INPUT_HANDLE);
  obj->hout = GetStdHandle(STD_OUTPUT_HANDLE);

  if(INVALID_HANDLE_VALUE == data->hin || INVALID_HANDLE_VALUE == data->hout) {
    NanThrowError("invalid std handles");
  }

  GetConsoleMode(obj->hin, &(obj->old_mode));
  DWORD new_mode = ((obj->old_mode | ENABLE_MOUSE_INPUT |
      ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT) &
      ~(ENABLE_QUICK_EDIT_MODE));
  SetConsoleMode(obj->hin, new_mode);

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
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::js_write) {
  NanScope();
  NanReturnThis();
}
