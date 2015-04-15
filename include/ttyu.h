/* ttyutil - ttyu.h - main header file
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

#include <stdio.h>
#include <vector>
//include <chrono>

#include <uv.h>
#include <node.h>
#include <nan.h>
#include <generated.h>
#include <utils.h>

// predefine event data and callbacks for ee.c
#define EE_DATA_TYPE v8::Local<v8::Value>
#define EE_DATA_ARG(name) v8::Local<v8::Value> name
#define EE_CB_TYPE NanCallback *
#define EE_CB_ARG(name) NanCallback *name
#include <ee.h>

// define TRUE & FALSE
#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif

// more defines (extending <generated.h>)
#define EVENT_NONE -1
#define EMIT_INTERVAL 20

// callback call function for the event emitter
TTYU_INLINE int ttyu_ee_cb_call(ee__listener_t *l, EE_DATA_ARG(data));
// callback compare function for the event emitter
TTYU_INLINE int ttyu_ee_compare(EE_CB_ARG(cb1), EE_CB_ARG(cb2));

class ttyu_js_c;

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

// data structure for caching platform-dependent terminal handles
// these are defined in platform-dependent source and header files
typedef struct ttyu_pi_s ttyu_pi_t;
void ttyu_pi_init(ttyu_pi_t *pi);
void ttyu_pi_destroy(ttyu_pi_t *pi);

// include platform dependent headers
#ifdef PLATFORM_WINDOWS
# include <win.h>
#else
# include <unix.h>
#endif

// thread functions
void emitter_thread_func(void *that);
void handler_thread_func(void *that);

// event get & unget functions and a platform dependent generator
void getevent(ttyu_js_c *data, ttyu_event_t *event);
bool ungetevent(ttyu_js_c *data, ttyu_event_t *event);
void event_generate(ttyu_js_c *data, ttyu_event_t *event, int arg0, int arg1,
    int arg2, int arg3, int arg4);

class ttyu_js_c : public node::ObjectWrap {
public:
  explicit ttyu_js_c();
  ~ttyu_js_c();

  TTYU_INLINE static void init(v8::Handle<v8::Object> exports,
      v8::Handle<v8::Object> module);
  TTYU_INLINE static NAN_METHOD(js_new);
  TTYU_INLINE static NAN_METHOD(js_start);
  TTYU_INLINE static NAN_METHOD(js_stop);
  TTYU_INLINE static NAN_METHOD(js_on);
  TTYU_INLINE static NAN_METHOD(js_off);
  TTYU_INLINE static NAN_METHOD(js_emit);
  TTYU_INLINE static NAN_METHOD(js_running);

  TTYU_INLINE static NAN_METHOD(js_write);

  ttyu_pi_t pi;
  uv_thread_t emitter_thread;
  uv_thread_t handler_thread;
  uv_barrier_t barrier;
  uv_mutex_t emitter_mutex;
  uv_mutex_t emit_mutex;
  uv_mutex_t handler_mutex;
  uv_cond_t cv;
  bool running;
  bool stop;
  std::vector<ttyu_event_t *> work;
  std::vector<ttyu_event_t *> unget;
  ee_emitter_t emitter;
};
// define export methods for javascript
/*
TTYU_INLINE NAN_METHOD(js_width);
TTYU_INLINE NAN_METHOD(js_height);
TTYU_INLINE NAN_METHOD(js_mode);
TTYU_INLINE NAN_METHOD(js_colors);
TTYU_INLINE NAN_METHOD(js_getx);
TTYU_INLINE NAN_METHOD(js_setx);
TTYU_INLINE NAN_METHOD(js_gety);
TTYU_INLINE NAN_METHOD(js_sety);
TTYU_INLINE NAN_METHOD(js_goto);
TTYU_INLINE NAN_METHOD(js_color);
TTYU_INLINE NAN_METHOD(js_beep);
TTYU_INLINE NAN_METHOD(js_clear);
TTYU_INLINE NAN_METHOD(js_prepare);
*/

#endif // TTYU_H_
