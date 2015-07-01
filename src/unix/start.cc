/* ttyutil - unix/start.cc
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

JSFUNCTION(ttyu_js_c, js_start, {
  that->running = TRUE;
  that->stop = FALSE;
  that->worker_run = TRUE;
  that->win = initscr();
  that->x = getcurx(that->win);
  that->y = getcury(that->win);

  uv_barrier_init(&that->barrier, 3);
  uv_mutex_init(&that->emitstacklock);
  uv_mutex_init(&that->ungetlock);
  uv_cond_init(&that->condition);

  uv_thread_create(&that->curses_thread, ttyu_js_c::curses_thread_func, that);
  that->check_queue();

  uv_barrier_wait(&that->barrier);
})
