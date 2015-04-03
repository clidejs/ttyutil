/* ttyutil - ttyu.h
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

#ifndef TTYU_H_
#define TTYU_H_

#include <cstdlib>
#include <signal.h>

#include <uv.h>
#include <node.h>
#include <nan.h>
#include <generated.h>
#include "util.h"

// predefine event data and callbacks for ee.c
#define EE_DATA_TYPE v8::Local<v8::Value>
#define EE_DATA_ARG(name) v8::Local<v8::Value> name
#define EE_CB_TYPE NanCallback *
#define EE_CB_ARG(name) NanCallback *name
#include <ee.h>

// callback call function for the event emitter
int ttyu_ee_cb_call(ee__listener_t *l, EE_DATA_ARG(data));
// callback compare function for the event emitter
int ttyu_ee_compare(EE_CB_ARG(cb1), EE_CB_ARG(cb2));

// predefine classes
class ttyu_js_c;
class ttyu_worker_c;

// data structure for caching platform-dependent terminal handles
// these are defined in platform-dependent source and header files
typedef struct ttyu_data_s ttyu_data_t;
void ttyu_data_init(ttyu_data_t *data);
void ttyu_data_destroy(ttyu_data_t *data);

// key event structure
typedef struct ttyu_key_s {
  int ctrl;
  char *c;
  int code;
  int which;
} ttyu_key_t;
// mouse event structure
typedef struct ttyu_mouse_s {
  int button;
  int x;
  int y;
  int ctrl;
} ttyu_mouse_t;
// event data structure
typedef struct ttyu_event_s {
  int type;
  const char *err;
  ttyu_key_t *key;
  ttyu_mouse_t *mouse;
} ttyu_event_t;
void ttyu_event_create_error(ttyu_event_t *event, const char *err);
void ttyu_event_create_resize(ttyu_event_t *event);
void ttyu_event_create_key(ttyu_event_t *event, int ctrl, char *c, int code,
    int which);
void ttyu_event_create_mouse(ttyu_event_t *event, int type, int button, int x,
    int y, int ctrl);
void ttyu_event_destroy(ttyu_event_t *event);

// helper functions for exception throwing
#define TTYU_THROW_IF_DESTROYED(obj) do {                                      \
  if(obj->throw_ && obj->destroyed_) {                                         \
    NanThrowError("TTYUtil object was already destroyed");                     \
    NanReturnUndefined();                                                      \
  }                                                                            \
} while(0)
#define TTYU_THROW_IF_NOT_RUNNING(obj) do {                                    \
  TTYU_THROW_IF_DESTROYED(obj);                                                \
  if(obj->throw_ && !obj->running) {                                           \
    NanThrowError("TTYUtil object was not started");                           \
    NanReturnUndefined();                                                      \
  }                                                                            \
} while(0)
#define TTYU_THROW_IF_NOT_RUNNING_VOID(obj) do {                               \
  if(obj->throw_ && obj->destroyed_) {                                         \
    NanThrowError("TTYUtil object was already destroyed");                     \
    return;                                                                    \
  }                                                                            \
  if(obj->throw_ && !obj->running) {                                           \
    NanThrowError("TTYUtil object was not started");                           \
    return;                                                                    \
  }                                                                            \
} while(0)

// definition of the node module class
class ttyu_js_c : public node::ObjectWrap {
public:
  static void init(v8::Handle<v8::Object> target);
  explicit ttyu_js_c();
  void destroy();

  ttyu_data_t *data;
  ee_emitter_t *emitter;
  bool running;
  bool paused;
private:
  ~ttyu_js_c();

  static NAN_METHOD(new_instance);
  static NAN_METHOD(start);
  static NAN_METHOD(pause);
  static NAN_METHOD(destroy);
  static NAN_METHOD(on);
  static NAN_METHOD(off);
  static NAN_METHOD(emit);
  static NAN_GETTER(is_running);
  static NAN_GETTER(get_width);
  static NAN_GETTER(get_height);
  static NAN_GETTER(get_mode);
  static NAN_GETTER(get_colors);
  static NAN_GETTER(getx);
  static NAN_SETTER(setx);
  static NAN_GETTER(gety);
  static NAN_SETTER(sety);
  static NAN_METHOD(gotoxy);
  static NAN_METHOD(write);
  static NAN_METHOD(beep);
  static NAN_METHOD(clear);
  static NAN_METHOD(prepare);
  static NAN_METHOD(color);

  static v8::Persistent<v8::Function> constructor;

  bool throw_;
  bool destroyed_;
  ttyu_worker_c *worker_;
};

// definition of the terminal input listener class
class ttyu_worker_c : public NanAsyncWorker {
public:
  class ttyu_progress_c {
    friend class ttyu_worker_c;
  public:
    void send(const ttyu_event_t *event) const;
  private:
    explicit ttyu_progress_c(ttyu_worker_c *that);
    // prevent movement
    ttyu_progress_c(const ttyu_progress_c&);
    void operator=(const ttyu_progress_c&);
#if __cplusplus >= 201103L
    ttyu_progress_c(const ttyu_progress_c&&) V8_DELETE;
    void operator=(const ttyu_progress_c&&) V8_DELETE;
#endif
    ttyu_worker_c *const that_;
  };

  ttyu_worker_c(ttyu_js_c *obj);
  virtual ~ttyu_worker_c();

  void progress();

  bool execute(const ttyu_progress_c& progress, ttyu_data_t *data);
  void handle(ttyu_event_t *event);

  virtual void Destroy();

  void Execute();
  virtual void WorkComplete();
private:
  void send_(const ttyu_event_t *event);

  static NAUV_WORK_CB(async_progress_);
  static void async_close_(uv_handle_t *handle);

  uv_async_t *async;
  uv_mutex_t *async_lock;
  ttyu_event_t *asyncdata_;
  ttyu_js_c *obj_;
};

// include platform dependent headers
#ifdef PLATFORM_WINDOWS
# include <win.h>
#else
# include <unix.h>
#endif

#endif // TTYU_H_
