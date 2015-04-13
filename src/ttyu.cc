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

TTYU_INLINE int ttyu_ee_cb_call(ee__listener_t *l, v8::Local<v8::Value> data) {
  v8::Local<v8::Value> args[] = {
    data
  };
  int count = 0;
  do {
    if(l->cb) {
      l->cb->Call(1, args);
      ++count;
    }
  } while((l = l->next));
  return count;
}

TTYU_INLINE int ttyu_ee_compare(EE_CB_ARG(cb1), EE_CB_ARG(cb2)) {
  return (int)(cb1->GetFunction() == cb2->GetFunction());
}

TTYU_INLINE void handle(ttyu_event_t *event) {
  NanScope();
  if(ee_count(obj_->emitter, event->type) == 0 || obj_->paused ||
      !obj_->running) {
    return;
  }

  v8::Local<v8::Object> obj = NanNew<v8::Object>();
  switch(event->type) {
    case EVENT_RESIZE:
      obj->Set(NanNew<v8::String>("type"), EVENTSTRING_RESIZE);
      break;
    case EVENT_KEY:
      obj->Set(NanNew<v8::String>("type"), EVENTSTRING_KEY);
      obj->Set(NanNew<v8::String>("ctrl"),
          NanNew<v8::Integer>(event->key->ctrl));
      obj->Set(NanNew<v8::String>("char"), NanNew<v8::String>(event->key->c));
      obj->Set(NanNew<v8::String>("code"),
          NanNew<v8::Integer>(event->key->code));
      obj->Set(NanNew<v8::String>("which"),
          NanNew<v8::Integer>(event->key->which));
      break;
    case EVENT_MOUSEDOWN:
    case EVENT_MOUSEUP:
    case EVENT_MOUSEMOVE:
    case EVENT_MOUSEWHEEL:
    case EVENT_MOUSEHWHEEL:
      if(event->type == EVENT_MOUSEDOWN) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEDOWN);
      } else if(event->type == EVENT_MOUSEUP) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEUP);
      } else if(event->type == EVENT_MOUSEMOVE) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEMOVE);
      } else if(event->type == EVENT_MOUSEWHEEL) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEWHEEL);
      } else if(event->type == EVENT_MOUSEHWHEEL) {
        obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEHWHEEL);
      }
      obj->Set(NanNew<v8::String>("button"),
          NanNew<v8::Integer>(event->mouse->button));
      obj->Set(NanNew<v8::String>("x"), NanNew<v8::Integer>(event->mouse->x));
      obj->Set(NanNew<v8::String>("y"), NanNew<v8::Integer>(event->mouse->y));
      obj->Set(NanNew<v8::String>("ctrl"),
          NanNew<v8::Integer>(event->mouse->ctrl));
      break;
    default: // EVENT_ERROR, EVENT_SIGNAL
      obj->Set(NanNew<v8::String>("type"), EVENTSTRING_ERROR);
      obj->Set(NanNew<v8::String>("error"), NanError(event->err));
      event->type = EVENT_ERROR;
      break;
  }
  ee_emit(obj_->emitter, event->type, obj);
}
