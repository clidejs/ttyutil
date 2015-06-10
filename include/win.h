/* ttyutil - win.h - additional header file for windows systems,
 * implementation in src/win.cc
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
#ifndef INCLUDE_WIN_H_
#define INCLUDE_WIN_H_

#include <windows.h>

#define WIN_BUFFER_SIZE 128

// worker class, heavily inspired by nan's NanAsyncProgressWorker
class ttyu_worker_c : public NanAsyncWorker {
 public:
  class ttyu_progress_c {
    friend class ttyu_worker_c;
   public:
    void send(const ttyu_event_t *event) const {
        that_->send_(event);
    }
   private:
    explicit ttyu_progress_c(ttyu_worker_c *that) : that_(that) {}
    // prevent movement
    ttyu_progress_c(const ttyu_progress_c&);
    void operator=(const ttyu_progress_c&);
#if __cplusplus >= 201103L
    ttyu_progress_c(const ttyu_progress_c&&) V8_DELETE;  // NOLINT(build/c++11)
    void operator=(const ttyu_progress_c&&) V8_DELETE;  // NOLINT(build/c++11)
#endif
    ttyu_worker_c *const that_;
  };

  explicit ttyu_worker_c(ttyu_js_c *obj) : NanAsyncWorker(NULL),
      asyncdata_(NULL), obj_(obj) {
    async = new uv_async_t;
    async_lock = new uv_mutex_t;
    uv_mutex_init(async_lock);
    uv_async_init(uv_default_loop(), async, async_progress_);
    async->data = this;
  }
  ~ttyu_worker_c() {
    uv_mutex_destroy(async_lock);
    ttyu_event_destroy(asyncdata_);
    free(asyncdata_);
  }

  void progress() {
    uv_mutex_lock(async_lock);
    ttyu_event_t *event = asyncdata_;
    asyncdata_ = NULL;
    uv_mutex_unlock(async_lock);

    if (event) {
      handle(event);
    }
    ttyu_event_destroy(event);
    free(event);
  }

  bool execute(const ttyu_progress_c& progress, ttyu_js_c *obj);
  void handle(ttyu_event_t *event);

  virtual void Destroy() {
    uv_close(reinterpret_cast<uv_handle_t*>(async), async_close_);
  }

  void Execute();

  void WorkComplete() { /* do nothing */ }

 private:
  void send_(const ttyu_event_t *event) {
    ttyu_event_t *new_event =
        reinterpret_cast<ttyu_event_t *>(malloc(sizeof(event)));
    memcpy(&new_event, &event, sizeof(event));
    uv_mutex_lock(async_lock);
    ttyu_event_t *old_event = asyncdata_;
    asyncdata_ = new_event;
    uv_mutex_unlock(async_lock);

    ttyu_event_destroy(old_event);
    free(old_event);
    uv_async_send(async);
  }

  static NAUV_WORK_CB(async_progress_) {
    ttyu_worker_c *worker = static_cast<ttyu_worker_c*>(async->data);
    worker->progress();
  }
  static void async_close_(uv_handle_t *handle) {
    ttyu_worker_c *worker = static_cast<ttyu_worker_c*>(handle->data);
    delete reinterpret_cast<uv_async_t*>(handle);
    delete worker;
  }

  uv_async_t *async;
  uv_mutex_t *async_lock;
  ttyu_event_t *asyncdata_;
  ttyu_js_c *obj_;
};

#define PLATFORM_DEPENDENT_FIELDS                                              \
  HANDLE hin;                                                                  \
  HANDLE hout;                                                                 \
  DWORD old_mode;                                                              \
  DWORD top;                                                                   \
  DWORD width;                                                                 \
  DWORD height;                                                                \
  DWORD curx;                                                                  \
  DWORD cury;                                                                  \
  uv_mutex_t emitlock;                                                         \
  uv_barrier_t barrier;                                                        \
  ttyu_worker_c worker

bool ttyu_win_scr_update(ttyu_js_c *obj, bool initial);
int ttyu_win_which(DWORD code);
int ttyu_win_ctrl(DWORD state);
DWORD ttyu_win_state(int ctrl);

#endif  // INCLUDE_WIN_H_
