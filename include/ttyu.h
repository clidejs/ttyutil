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

// error types
#define E_ALL 0
#define E_WIN 1
#define E_UNIX 2

// error messages
#define __ERRMSG(XX)                                                           \
  XX(0x00, "critical unknown error (0x00)", E_ALL);                            \
  XX(0x01, "invalid console handle value (0x01)", E_WIN);                      \
  XX(0x02, "could not get console output buffer information (0x02)", E_WIN);   \
  XX(0x03, "could not set console output buffer information (0x03)", E_WIN);   \
  XX(0x04, "could not fill console output buffer (0x04)", E_WIN)

template<typename T>
TTYU_INLINE T _ERRMSG(int id) {
  #define XX(i, name, platform) if (i == id) { return name; }
  __ERRMSG(XX);
  #undef XX
  return "";
}
#define ERRMSG(id) _ERRMSG(id)

// callback call function for the event emitter
int ttyu_ee_cb_call(ee__listener_t *l, EE_DATA_ARG(data));
// callback compare function for the event emitter
int ttyu_ee_compare(EE_CB_ARG(cb1), EE_CB_ARG(cb2));

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
  static NAN_METHOD(js_on);
  static NAN_METHOD(js_off);
  static NAN_METHOD(js_emit);
  // static NAN_METHOD(js_running);

  static NAN_METHOD(js_write);

  bool running;
  bool stop;
  ee_emitter_t emitter;
  PLATFORM_DEPENDENT_FIELDS;
};

#endif  // INCLUDE_TTYU_H_
