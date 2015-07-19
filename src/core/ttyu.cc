/* ttyutil - ttyu.cc - implements additional functions
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

JSFUNCTION(ttyu_js_c::js_new, {
  ttyu_js_c *obj = new ttyu_js_c();
  obj->Wrap(args.This());
  obj->emitter = new NanCallback(v8::Local<v8::Function>::Cast(args[0]));
  NanReturnThis();
})

JSFUNCTION(ttyu_js_c, js_running, {
  NanReturnValue(that->running ? NanTrue() : NanFalse());
})

// initialize node module
void ttyu_js_c::init(v8::Handle<v8::Object> exports,
    v8::Handle<v8::Object> module) {
  int _exports = 0;
  v8::Local<v8::FunctionTemplate> tpl =
      NanNew<v8::FunctionTemplate>(js_new);
  tpl->SetClassName(NanNew<v8::String>("ttyu_js_c"));

  EXPORT_METHOD(tpl, "start", js_start);
  EXPORT_METHOD(tpl, "stop", js_stop);
  EXPORT_METHOD(tpl, "emit", js_emit);
  EXPORT_METHOD(tpl, "write", js_write);
  EXPORT_METHOD(tpl, "getrunning", js_running);
  EXPORT_METHOD(tpl, "getwidth", js_getwidth);
  EXPORT_METHOD(tpl, "getheight", js_getheight);
  EXPORT_METHOD(tpl, "setwidth", js_setwidth);
  EXPORT_METHOD(tpl, "setheight", js_setheight);
  EXPORT_METHOD(tpl, "resize", js_resize);
  EXPORT_METHOD(tpl, "getmode", js_mode);
  EXPORT_METHOD(tpl, "getcolors", js_colors);
  EXPORT_METHOD(tpl, "setx", js_setx);
  EXPORT_METHOD(tpl, "getx", js_getx);
  EXPORT_METHOD(tpl, "sety", js_sety);
  EXPORT_METHOD(tpl, "gety", js_gety);
  EXPORT_METHOD(tpl, "goto", js_goto);
  EXPORT_METHOD(tpl, "beep", js_beep);
  EXPORT_METHOD(tpl, "clear", js_clear);
  EXPORT_METHOD(tpl, "hide", js_hide);
  EXPORT_METHOD(tpl, "show", js_show);

  tpl->InstanceTemplate()->SetInternalFieldCount(_exports);
  module->Set(NanNew<v8::String>("exports"), tpl->GetFunction());
}
NODE_MODULE(ttyu, ttyu_js_c::init);
