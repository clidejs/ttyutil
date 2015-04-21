/* ttyutil - nauv.h - native abstractions for libuv extending nan.h
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
#ifndef INCLUDE_NAUV_H_
#define INCLUDE_NAUV_H_

#if NAUV_UVVERSION < 0x010000
#define NAUV_BARRIER_WAIT(barrier, kill)                                       \
  uv_barrier_wait(barrier);                                                    \
  if (kill)                                                                    \
    uv_barrier_destroy(barrier);
#else
#define NAUV_BARRIER_WAIT(barrier, kill)                                       \
  if (uv_barrier_wait(barrier) > 0)                                            \
    uv_barrier_destroy(barrier);
#endif

#endif  // INCLUDE_NAUV_H_
