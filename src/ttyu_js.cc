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

ttyu_js_c::ttyu_js_c() {
  running = FALSE;
  stop = TRUE;
}

ttyu_js_c::~ttyu_js_c() {
  running = FALSE;
  stop = TRUE;
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_new) {
  NanScope();
  ttyu_js_c *obj = new ttyu_js_c();
  obj->Wrap(args.This());
  NanReturnThis();
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_start) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  ttyu_pi_init(&obj->pi);
  DBG("asdf");
  obj->running = TRUE;
  obj->stop = FALSE;
  uv_barrier_init(&obj->barrier, 3);
  uv_mutex_init(&obj->emitter_mutex);
  uv_mutex_init(&obj->emit_mutex);
  uv_mutex_init(&obj->handler_mutex);
  uv_cond_init(&obj->cv);
  DBG("started threads");
  uv_thread_create(&obj->emitter_thread, emitter_thread_func, obj);
  uv_thread_create(&obj->handler_thread, handler_thread_func, obj);

  if(uv_barrier_wait(&obj->barrier) > 0)
      uv_barrier_destroy(&obj->barrier);

  NanReturnThis();
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_stop) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  obj->running = FALSE;
  obj->stop = TRUE;
  uv_thread_join(&obj->emitter_thread);
  uv_thread_join(&obj->handler_thread);
  uv_mutex_destroy(&obj->emitter_mutex);
  uv_mutex_destroy(&obj->emit_mutex);
  uv_mutex_destroy(&obj->handler_mutex);
  uv_cond_destroy(&obj->cv);
  // TODO destroy obj->pi
  NanReturnThis();
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_on) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  uv_mutex_lock(&obj->emit_mutex);
  ee_on(&obj->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  uv_mutex_unlock(&obj->emit_mutex);
  NanReturnThis();
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_off) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  uv_mutex_lock(&obj->emit_mutex);
  ee_off(&obj->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  uv_mutex_unlock(&obj->emit_mutex);
  NanReturnThis();
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_emit) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  ttyu_event_t event;
  DBG(".emit()");
  event_generate(obj, &event, args[0]->Int32Value(), args[1]->Int32Value(),
      args[2]->Int32Value(), args[3]->Int32Value(), args[4]->Int32Value());
  uv_mutex_lock(&obj->handler_mutex);
  obj->unget.push_back(&event);
  uv_mutex_unlock(&obj->handler_mutex);
  NanReturnThis();
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_running) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Boolean>(&obj->running));
}

TTYU_INLINE NAN_METHOD(ttyu_js_c::js_write) {
  NanScope();
  //ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  // TODO
  printf("%s\r\n",
      (new v8::String::Utf8Value(args[0]->ToString()))->operator*());
  NanReturnThis();
}

// initialize node module
TTYU_INLINE void ttyu_js_c::init(v8::Handle<v8::Object> exports,
    v8::Handle<v8::Object> module) {
  v8::Local<v8::FunctionTemplate> tpl =
      NanNew<v8::FunctionTemplate>(js_new);
  tpl->SetClassName(NanNew<v8::String>("ttyu_js_c"));
  tpl->InstanceTemplate()->SetInternalFieldCount(19);

  EXPORT_METHOD(tpl, "start", js_start);
  EXPORT_METHOD(tpl, "stop", js_stop);
  EXPORT_METHOD(tpl, "on", js_on);
  EXPORT_METHOD(tpl, "off", js_off);
  EXPORT_METHOD(tpl, "emit", js_emit);

  EXPORT_METHOD(tpl, "write", js_write);
  module->Set(NanNew<v8::String>("exports"), tpl->GetFunction());
/*
  EXPORT_GET(tpl, "running", js_running);
  EXPORT_GET(tpl, "width", js_width);
  EXPORT_GET(tpl, "height", js_height);
  EXPORT_GET(tpl, "mode", js_mode);
  EXPORT_GET(tpl, "colors", js_colors);
  EXPORT_GETSET(tpl, "x", js_getx, js_setx);
  EXPORT_GETSET(tpl, "y", js_gety, js_sety);
  EXPORT_METHOD(tpl, "goto", js_goto);
  EXPORT_METHOD(tpl, "color", js_color);
  EXPORT_METHOD(tpl, "beep", js_beep);
  EXPORT_METHOD(tpl, "clear", js_clear);
  EXPORT_METHOD(tpl, "prepare", js_prepare);*/
}
NODE_MODULE(ttyu, ttyu_js_c::init);
