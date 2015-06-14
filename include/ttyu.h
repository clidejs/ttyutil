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
#ifndef INCLUDE_TTYU_H_
#define INCLUDE_TTYU_H_

#include <stdio.h>

#include <nan.h>
#include <generated.h>
#include <utils.h>
#include <cdebug.h>
#include <string>

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

// error messages
typedef struct ttyu_error_s {
  int id;
  const char *msg;
} ttyu_error_t;

#define __ERRMSG(XX)                                                           \
  XX(0x00, "critical unknown error");                                          \
  XX(0x01, "invalid console handle value");                                    \
  XX(0x02, "could not get console output buffer information");                 \
  XX(0x03, "could not set console output buffer information");                 \
  XX(0x04, "could not fill console output buffer")

TTYU_INLINE ttyu_error_t _ERRMSG(int id) {
  ttyu_error_t err;
  err.id = id;
  #define XX(i, name) if (i == id) {                                           \
    err.msg = name;                                                            \
    return err;                                                                \
  }
  __ERRMSG(XX);
  #undef XX
  return _ERRMSG(0);
}
#define ERRMSG(id) &_ERRMSG(id)

#define THROW_IF_STOPPED(obj) if (!obj->running) {                             \
  return NanThrowError("Function requires ttyu to be running");                \
}

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
  ttyu_key_t *key;
  ttyu_mouse_t *mouse;
} ttyu_event_t;
void ttyu_event_create_error(ttyu_event_t *event);
void ttyu_event_create_resize(ttyu_event_t *event);
void ttyu_event_create_key(ttyu_event_t *event, int ctrl, char *c, int code,
    int which);
void ttyu_event_create_mouse(ttyu_event_t *event, int type, int button, int x,
    int y, int ctrl);
void ttyu_event_destroy(ttyu_event_t *event);

// predefine data class
class ttyu_js_c;

// include platform dependent headers
#ifdef PLATFORM_WINDOWS
# include <win.h>
#else
# include <unix.h>
#endif

class ttyu_js_c : public node::ObjectWrap {
 public:
  ttyu_js_c();
  ~ttyu_js_c();

  static void init(v8::Handle<v8::Object> exports,
      v8::Handle<v8::Object> module);
  static NAN_METHOD(js_new);
  static NAN_METHOD(js_start);
  static NAN_METHOD(js_stop);
  static NAN_METHOD(js_emit);
  static NAN_METHOD(js_running);
  static NAN_METHOD(js_getwidth);
  static NAN_METHOD(js_getheight);
  static NAN_METHOD(js_setwidth);
  static NAN_METHOD(js_setheight);
  static NAN_METHOD(js_resize);
  static NAN_METHOD(js_mode);
  static NAN_METHOD(js_setx);
  static NAN_METHOD(js_getx);
  static NAN_METHOD(js_sety);
  static NAN_METHOD(js_gety);
  static NAN_METHOD(js_goto);
  static NAN_METHOD(js_beep);
  static NAN_METHOD(js_clear);

  static NAN_METHOD(js_write);

  ttyu_error_t *err;
  bool running;
  bool stop;
  NanCallback *emitter;
  PLATFORM_DEPENDENT_FIELDS;
};

#endif  // INCLUDE_TTYU_H_
