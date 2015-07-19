/* ttyutil - utils.h - header file to define utility macros
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
#ifndef INCLUDE_UTILS_H_
#define INCLUDE_UTILS_H_

#define EXPORT_METHOD(tpl, name, cb) do {                                      \
  v8::Local<v8::FunctionTemplate> t = NanNew<v8::FunctionTemplate>(cb);        \
  tpl->InstanceTemplate()->Set(NanNew<v8::String>(name),                       \
      t->GetFunction(), v8::ReadOnly);                                         \
  ++_exports;                                                                  \
} while (0)

#define TTYU_TOSTRING(handle)                                                  \
  (new v8::String::Utf8Value(handle->ToString()))->operator*()

#define MUTEX_LOCK(mutex, action) do {                                         \
  uv_mutex_lock(mutex);                                                        \
  action;                                                                      \
  uv_mutex_unlock(mutex);                                                      \
} while (0)

#ifdef PLATFORM_WINDOWS
# define JSFUNCTION(_0, ...) JSFUNCTION__CALL(JSFUNCTION_((__VA_ARGS__,        \
  JSFUNCTION_PROTO, JSFUNCTION_STATIC, 0)), (_0, __VA_ARGS__))
# define JSFUNCTION__CALL(fun, args) fun args
# define JSFUNCTION_(args) JSFUNCTION__ args
# define JSFUNCTION__(_1, _2, _3, ...) _3
#else
# define JSFUNCTION(_0, ...) JSFUNCTION_(__VA_ARGS__, JSFUNCTION_PROTO,        \
  JSFUNCTION_STATIC, 0)(_0, __VA_ARGS__)
# define JSFUNCTION_(_1, _2, _3, ...) _3
#endif
#define JSFUNCTION_STATIC(name, body) NAN_METHOD(name) {                       \
  NanScope();                                                                  \
  if(1) body                                                                   \
  NanReturnUndefined();                                                        \
}
#define JSFUNCTION_PROTO(clas, name, body) NAN_METHOD(clas::name) {            \
  NanScope();                                                                  \
  clas *that = ObjectWrap::Unwrap<clas>(args.This());                          \
  if(1) body                                                                   \
  NanReturnUndefined();                                                        \
}

#define EMIT_EVENT_OBJECT(event, cb) do {                                      \
  v8::Local<v8::Object> __obj = NanNew<v8::Object>();                          \
  switch (event->type) {                                                       \
    case EVENT_RESIZE:                                                         \
      __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_RESIZE);              \
      break;                                                                   \
    case EVENT_KEY:                                                            \
      __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_KEY);                 \
      __obj->Set(NanNew<v8::String>("ctrl"),                                   \
          NanNew<v8::Integer>(event->key->ctrl));                              \
      __obj->Set(NanNew<v8::String>("char"),                                   \
          NanNew<v8::String>(event->key->c));                                  \
      __obj->Set(NanNew<v8::String>("code"),                                   \
          NanNew<v8::Integer>(event->key->code));                              \
      __obj->Set(NanNew<v8::String>("which"),                                  \
          NanNew<v8::Integer>(event->key->which));                             \
      break;                                                                   \
    case EVENT_MOUSEDOWN:                                                      \
    case EVENT_MOUSEUP:                                                        \
    case EVENT_MOUSEMOVE:                                                      \
    case EVENT_MOUSEWHEEL:                                                     \
    case EVENT_MOUSEHWHEEL:                                                    \
      if (event->type == EVENT_MOUSEDOWN) {                                    \
        __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEDOWN);         \
      } else if (event->type == EVENT_MOUSEUP) {                               \
        __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEUP);           \
      } else if (event->type == EVENT_MOUSEMOVE) {                             \
        __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEMOVE);         \
      } else if (event->type == EVENT_MOUSEWHEEL) {                            \
        __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEWHEEL);        \
      } else if (event->type == EVENT_MOUSEHWHEEL) {                           \
        __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_MOUSEHWHEEL);       \
      }                                                                        \
      __obj->Set(NanNew<v8::String>("button"),                                 \
          NanNew<v8::Integer>(event->mouse->button));                          \
      __obj->Set(NanNew<v8::String>("x"),                                      \
          NanNew<v8::Integer>(event->mouse->x));                               \
      __obj->Set(NanNew<v8::String>("y"),                                      \
          NanNew<v8::Integer>(event->mouse->y));                               \
      __obj->Set(NanNew<v8::String>("ctrl"),                                   \
          NanNew<v8::Integer>(event->mouse->ctrl));                            \
      break;                                                                   \
    default:                                                                   \
      __obj->Set(NanNew<v8::String>("type"), EVENTSTRING_ERROR);               \
      __obj->Set(NanNew<v8::String>("error"), NanError("TODO"));               \
      event->type = EVENT_ERROR;                                               \
      break;                                                                   \
  }                                                                            \
                                                                               \
  v8::Local<v8::Value> __args[] = { __obj };                                   \
  DBG("  emit_event_object calling");                                          \
  cb->Call(1, __args);                                                         \
  DBG("  emit_event_object called");                                           \
} while (0)

#if defined(__GNUC__) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE __forceinline
#else
# define TTYU_INLINE inline
#endif

#endif  // INCLUDE_UTILS_H_
