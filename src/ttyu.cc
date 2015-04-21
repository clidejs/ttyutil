/* ttyutil - ttyu.cc - implements threads and additional functions
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

int ttyu_ee_cb_call(ee__listener_t *l, v8::Local<v8::Value> data) {
  v8::Local<v8::Value> args[] = {
    data
  };
  int count = 0;
  do {
    if (l->cb) {
      l->cb->Call(1, args);
      ++count;
    }
  } while ((l = l->next));
  return count;
}

int ttyu_ee_compare(EE_CB_ARG(cb1), EE_CB_ARG(cb2)) {
  return static_cast<int>(cb1->GetFunction() == cb2->GetFunction());
}

NAN_METHOD(ttyu_js_c::js_new) {
    NanScope();
    ttyu_js_c *obj = new ttyu_js_c();
    obj->Wrap(args.This());
    NanReturnThis();
}

// initialize node module
void ttyu_js_c::init(v8::Handle<v8::Object> exports,
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
