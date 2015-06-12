/* ttyutil - unix/worker.cc
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

void ttyu_js_c::check_queue() {
  if (running && !stop) {
    NanAsyncQueueWorker(new ttyu_worker_c(this));
  }
}

void ttyu_worker_c::Execute() {
  MUTEX_LOCK(&obj->emitstacklock,{
    if (obj->worker_run) {
      obj->worker_run = FALSE;
      uv_barrier_wait(&obj->barrier);
    }

    while (obj->emit_stack.size() == 0) {
      uv_cond_wait(&obj->condition, &obj->emitstacklock);
    }

    SDBG("::Execute %zu", obj->emit_stack.size());

    // copy stack into emit_worker and clear stack
    std::copy(obj->emit_stack.begin(), obj->emit_stack.end(),
        std::back_inserter(emit_stack));
    obj->emit_stack.clear();
  });
}

void ttyu_worker_c::HandleOKCallback() {
  NanScope();
  for (std::vector<ttyu_event_t>::size_type i = 0;
      i < emit_stack.size(); ++i) {
    ttyu_event_t event = emit_stack[i];
    SDBG("::HandleOKCallback %d %d", i, event.type);

    MUTEX_LOCK(&obj->emitlock, {
      if (ee_count(&obj->emitter, event.type) == 0 ||
          event.type == EVENT_NONE) {
        continue;  // fast skip
      }
    });

    v8::Local<v8::Object> jsobj = NanNew<v8::Object>();
    switch (event.type) {
      case EVENT_RESIZE:
        jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_RESIZE);
        break;
      case EVENT_KEY:
        jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_KEY);
        jsobj->Set(NanNew<v8::String>("ctrl"),
            NanNew<v8::Integer>(event.key->ctrl));
        jsobj->Set(NanNew<v8::String>("char"),
            NanNew<v8::String>(event.key->c));
        jsobj->Set(NanNew<v8::String>("code"),
            NanNew<v8::Integer>(event.key->code));
        jsobj->Set(NanNew<v8::String>("which"),
            NanNew<v8::Integer>(event.key->which));
        break;
      case EVENT_MOUSEDOWN:
      case EVENT_MOUSEUP:
      case EVENT_MOUSEMOVE:
      case EVENT_MOUSEWHEEL:
      case EVENT_MOUSEHWHEEL:
        if (event.type == EVENT_MOUSEDOWN) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEDOWN);
        } else if (event.type == EVENT_MOUSEUP) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEUP);
        } else if (event.type == EVENT_MOUSEMOVE) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEMOVE);
        } else if (event.type == EVENT_MOUSEWHEEL) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEWHEEL);
        } else if (event.type == EVENT_MOUSEHWHEEL) {
          jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEHWHEEL);
        }
        jsobj->Set(NanNew<v8::String>("button"),
            NanNew<v8::Integer>(event.mouse->button));
        jsobj->Set(NanNew<v8::String>("x"),
            NanNew<v8::Integer>(event.mouse->x));
        jsobj->Set(NanNew<v8::String>("y"),
            NanNew<v8::Integer>(event.mouse->y));
        jsobj->Set(NanNew<v8::String>("ctrl"),
            NanNew<v8::Integer>(event.mouse->ctrl));
        break;
      default:  // EVENT_ERROR, EVENT_SIGNAL
        jsobj->Set(NanNew<v8::String>("type"), EVENTSTRING_ERROR);
        jsobj->Set(NanNew<v8::String>("error"), NanError("..."));
        event.type = EVENT_ERROR;
        break;
    }

    MUTEX_LOCK(&obj->emitlock, {
      ee_emit(&obj->emitter, event.type, jsobj);
    });
  }
  emit_stack.clear();
  obj->check_queue();
}
