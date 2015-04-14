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

// global data variable
ttyu_data_t *_data;

TTYU_INLINE NAN_METHOD(js_start) {
  NanScope();
  ttyu_pi_t pi;
  ttyu_pi_init(&pi);
  _data->running = TRUE;
  _data->stop = FALSE;
  _data->pi = &pi;
  uv_mutex_init(_data->emitter_mutex);
  uv_mutex_init(_data->emit_mutex);
  uv_mutex_init(_data->handler_mutex);
  uv_cond_init(_data->cv);
  uv_thread_create(_data->emitter_thread, emitter, &_data);
  uv_thread_create(_data->handler_thread, handler, &_data);
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_stop) {
  NanScope();
  _data->running = FALSE;
  _data->stop = TRUE;
  uv_thread_join(_data->emitter_thread);
  uv_thread_join(_data->handler_thread);
  uv_mutex_destroy(_data->emitter_mutex);
  uv_mutex_destroy(_data->emit_mutex);
  uv_mutex_destroy(_data->handler_mutex);
  uv_cond_destroy(_data->cv);
  // TODO destroy pi
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_on) {
  NanScope();
  uv_mutex_lock(_data->emit_mutex);
  ee_on(_data->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  uv_mutex_unlock(_data->emit_mutex);
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_off) {
  NanScope();
  uv_mutex_lock(_data->emit_mutex);
  ee_off(_data->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  uv_mutex_unlock(_data->emit_mutex);
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_emit) {
  NanScope();
  ttyu_event_t event;
  event_generate(&event, args[0]->Int32Value(), args[1]->Int32Value(),
      args[2]->Int32Value(), args[3]->Int32Value(), args[4]->Int32Value());
  uv_mutex_lock(_data->handler_mutex);
  _data->unget->push_back(&event);
  uv_mutex_unlock(_data->handler_mutex);
  NanReturnUndefined();
}

TTYU_INLINE NAN_METHOD(js_running) {
  NanScope();
  NanReturnValue(NanNew<v8::Boolean>(_data->running));
}

// initialize node module
void init(v8::Handle<v8::Object> exports) {
  EXPORT_METHOD(exports, "start", js_start);
  EXPORT_METHOD(exports, "stop", js_stop);
  EXPORT_METHOD(exports, "on", js_on);
  EXPORT_METHOD(exports, "off", js_off);
  EXPORT_METHOD(exports, "emit", js_emit);
/*
  EXPORT_GET(exports, "running", js_running);
  EXPORT_GET(exports, "width", js_width);
  EXPORT_GET(exports, "height", js_height);
  EXPORT_GET(exports, "mode", js_mode);
  EXPORT_GET(exports, "colors", js_colors);
  EXPORT_GETSET(exports, "x", js_getx, js_setx);
  EXPORT_GETSET(exports, "y", js_gety, js_sety);
  EXPORT_METHOD(exports, "goto", js_goto);
  EXPORT_METHOD(exports, "color", js_color);
  EXPORT_METHOD(exports, "beep", js_beep);
  EXPORT_METHOD(exports, "clear", js_clear);
  EXPORT_METHOD(exports, "prepare", js_prepare);
  EXPORT_METHOD(exports, "write", js_write);*/
}
NODE_MODULE(ttyu, init);
