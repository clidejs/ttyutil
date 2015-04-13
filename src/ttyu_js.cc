/* ttyutil - ttyu_js.cc - implements the javascript module
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

TTYU_INLINE NAN_METHOD(js_start) {
  NanScope();
  _data->running = TRUE;
  _data->stop = FALSE;
  uv_mutex_init(_data->mutex);
  uv_cond_init(_data->cv);
  uv_thread_create(_data->thread, loop, &_data);
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_stop) {
  NanScope();
  _data->running = FALSE;
  _data->stop = TRUE;
  uv_thread_join(_data->thread);
  uv_mutex_destroy(_data->mutex);
  uv_cond_destroy(_data->cv);
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_on) {
  NanScope();
  ee_on(_data->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_off) {
  NanScope();
  ee_off(_data->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_emit) {
  NanScope();

  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_running) {
  NanScope();
  NanReturnValue(NanNew<v8::Boolean>(_data->running));
}
