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
  MUTEX_LOCK(obj->emitstacklock, {
    if (obj->worker_run) {
      obj->worker_run = FALSE;
      uv_barrier_wait(obj->barrier);
    }

    while (obj->emit_stack.size() == 0) {
      uv_cond_wait(obj->condition, obj->emitstacklock);
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
    EMIT_EVENT_OBJECT((&emit_stack[i]), obj->emitter);
  }
  emit_stack.clear();
  obj->check_queue();
}
