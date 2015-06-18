/* ttyutil - util.h - header file to define utility methods,
 * implementation in src/util.cc
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
} while (0)

#define TTYU_TOSTRING(handle)                                                  \
  (new v8::String::Utf8Value(handle->ToString()))->operator*()

#define MUTEX_LOCK(mutex, action) ({                                           \
  uv_mutex_lock(mutex);                                                        \
  action;                                                                      \
  uv_mutex_unlock(mutex);                                                      \
})

#if defined(__GNUC__) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER) && !(defined(DEBUG) && DEBUG)
# define TTYU_INLINE __forceinline
#else
# define TTYU_INLINE inline
#endif

#endif  // INCLUDE_UTILS_H_
