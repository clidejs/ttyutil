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

JSFUNCTION(ttyu_js_c, js_start, {
  if (that->running) NanReturnUndefined();
  that->running = TRUE;
  that->stop = FALSE;

  DBG("::js_start()");
  that->barrier = ALLOC(uv_barrier_t, 1);
  uv_barrier_init(that->barrier, 2);

  that->hin = GetStdHandle(STD_INPUT_HANDLE);
  that->hout = GetStdHandle(STD_OUTPUT_HANDLE);

  if (INVALID_HANDLE_VALUE == that->hin || INVALID_HANDLE_VALUE == that->hout) {
    NanThrowError("invalid std handles");
  }

  GetConsoleMode(that->hin, &(that->old_mode));
  DWORD new_mode = ((that->old_mode | ENABLE_MOUSE_INPUT |
      ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT) &
      ~(ENABLE_QUICK_EDIT_MODE));
  that->worker = new ttyu_worker_c(that);
  SetConsoleMode(that->hin, new_mode);

  DBG("  async queue start");
  NanAsyncQueueWorker(that->worker);

  BARRIER_WAITKILL(that->barrier);
})
