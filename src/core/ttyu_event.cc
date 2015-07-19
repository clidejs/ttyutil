/* ttyutil - ttyu_event.cc - implements methods to work with ttyu_event_t struct
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

void ttyu_event_create_error(ttyu_event_t *event) {
  event->type = EVENT_ERROR;
  event->key = NULL;
  event->mouse = NULL;
}

void ttyu_event_create_resize(ttyu_event_t *event) {
  event->type = EVENT_RESIZE;
  event->key = NULL;
  event->mouse = NULL;
}

void ttyu_event_create_key(ttyu_event_t *event, int ctrl, char *c,
    int code, int which) {
  size_t len = strlen(c);
  char *ch = reinterpret_cast<char *>(std::malloc(sizeof(char) * (len + 1)));
  memcpy(ch, c, sizeof(char) * len);
  ch[len] = '\0';

  event->type = EVENT_KEY;
  event->key = reinterpret_cast<ttyu_key_t *>(std::malloc(sizeof(ttyu_key_t)));
  event->mouse = NULL;

  event->key->ctrl = ctrl;
  event->key->c = ch;
  event->key->code = code;
  event->key->which = which;
}

void ttyu_event_create_mouse(ttyu_event_t *event, int type, int button, int x,
    int y, int ctrl) {
  event->type = type;
  event->key = NULL;
  event->mouse =
      reinterpret_cast<ttyu_mouse_t *>(std::malloc(sizeof(ttyu_mouse_t)));

  event->mouse->button = button;
  event->mouse->x = x;
  event->mouse->y = y;
  event->mouse->ctrl = ctrl;
}

void ttyu_event_destroy(ttyu_event_t *event) {
  if (event) {
    if (event->mouse) {
      std::free(event->mouse);
    }
    if (event->key) {
      if (event->key->c) {
        std::free(event->key->c);
      }
      std::free(event->mouse);
    }
  }
}
