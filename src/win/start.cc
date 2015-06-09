/* ttyutil - win/start.cc
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
