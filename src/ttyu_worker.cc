/* ttyutil - ttyu_worker.cc - implements the background (async) worker
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

void ttyu_worker_c::ttyu_progress_c::send(const ttyu_event_t *event) const {
  that_->send_(event);
}

ttyu_worker_c::ttyu_progress_c::ttyu_progress_c(ttyu_worker_c *that) :
    that_(that) {}

ttyu_worker_c::ttyu_worker_c(ttyu_js_c *obj) :
    NanAsyncWorker(NULL), asyncdata_(NULL), obj_(obj) {
  async = new uv_async_t;
  async_lock = new uv_mutex_t;
  uv_mutex_init(async_lock);
  uv_async_init(uv_default_loop(), async, async_progress_);
  async->data = this;
}

ttyu_worker_c::~ttyu_worker_c() {
  uv_mutex_destroy(async_lock);
  ttyu_event_destroy(asyncdata_);
  free(asyncdata_);
}

void ttyu_worker_c::progress() {
  uv_mutex_lock(async_lock);
  ttyu_event_t *event = asyncdata_;
  asyncdata_ = NULL;
  uv_mutex_unlock(async_lock);

  if(event) {
    handle(event);
  }
  ttyu_event_destroy(event);
  free(event);
}

void ttyu_worker_c::handle(ttyu_event_t *event) {
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
      obj->Set(NanNew<v8::String>("type"), EVENTSTRING_RESIZE);
      obj->Set(NanNew<v8::String>("error"), NanError(event->err));
      event->type = EVENT_ERROR;
      break;
  }
  ee_emit(obj_->emitter, event->type, obj);
}

void ttyu_worker_c::Destroy() {
  uv_close(reinterpret_cast<uv_handle_t*>(async), async_close_);
}

void ttyu_worker_c::Execute() {
  ttyu_progress_c progress(this);
  // loop execute until it returns false (error)
  while(execute(progress, obj_->data));
}

void ttyu_worker_c::send_(const ttyu_event_t *event) {
  ttyu_event_t *new_event = (ttyu_event_t *)malloc(sizeof(event));
  memcpy(&new_event, &event, sizeof(event));
  uv_mutex_lock(async_lock);
  ttyu_event_t *old_event = asyncdata_;
  asyncdata_ = new_event;
  uv_mutex_unlock(async_lock);

  ttyu_event_destroy(old_event);
  free(old_event);
  uv_async_send(async);
}

NAUV_WORK_CB(ttyu_worker_c::async_progress_) {
  ttyu_worker_c *worker = static_cast<ttyu_worker_c*>(async->data);
  worker->progress();
}

void ttyu_worker_c::async_close_(uv_handle_t *handle) {
  ttyu_worker_c *worker = static_cast<ttyu_worker_c*>(handle->data);
  delete reinterpret_cast<uv_async_t*>(handle);

  if(worker->obj_->emitter) {
    ee_destroy(worker->obj_->emitter);
    delete worker->obj_->emitter;
  }

  delete worker;
}

void ttyu_worker_c::WorkComplete() {
  // do nothing
}
