/* ee - eventemitter for c - ee.c
 * https://github.com/clidejs/ee.c
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

#ifdef __cplusplus
extern "C" {
#endif

// header file
#include "ee.h"

// for malloc, free
#include <stdlib.h>

// initialize empty emitter
void ee_init(ee_emitter_t *emitter,
    int (*emit)(ee__listener_t *, EE_DATA_TYPE())) {
  emitter->root = 0;
  emitter->emit = emit;
}

void ee_on(ee_emitter_t *emitter, int event, EE_CB_TYPE(cb)) {
  ee__event_t *ev;
  if(emitter->root == 0) {
    ev = ee__event_new(event);
    emitter->root = ev;
  } else if(emitter->root->id == event) {
    ev = emitter->root;
  } else {
    ev = ee__event_find(emitter->root, event);
  }
  if(ev->root == 0) {
    ev->root = ee__listener_new(cb);
  } else {
    ee__listener_add(ev->root, cb);
  }
}

void ee_off(ee_emitter_t *emitter, int event, EE_CB_TYPE(cb)) {
  ee__event_t *ev;
  if(emitter->root == 0) {
    return;
  } else if(emitter->root->id == event) {
    ev = emitter->root;
  } else {
    ev = ee__event_find(emitter->root, event);
  }
  if(ev->root == 0) {
    return;
  } else if(ev->root->cb == cb) {
    ee__listener_t *old = ev->root;
    ev->root = old->next;
    ee__listener_destroy(old);
  } else {
    ee__listener_remove(ev->root, cb);
  }
}

int ee_emit(ee_emitter_t *emitter, int event, EE_DATA_TYPE(data)) {
  ee__listener_t *l;
  int count = 0;
  if(emitter->root == 0) {
    return count;
  } else if(emitter->root->id == event) {
    l = emitter->root->root;
  } else {
    l = ee__event_find(emitter->root, event)->root;
  }
  if(l != 0) {
    if(emitter->emit) {
      return emitter->emit(l, data);
    } else {
      do {
        if(l->cb) {
          l->cb(data);
        }
        ++count;
      } while((l = l->next));
    }
  }
  return count;
}

int ee_count(ee_emitter_t *emitter, int event) {
  ee__listener_t *l;
  int count = 0;
  if(emitter->root == 0) {
    return count;
  } else if(emitter->root->id == event) {
    l = emitter->root->root;
  } else {
    l = ee__event_find(emitter->root, event)->root;
  }
  if(l != 0) {
    do {
      ++count;
    } while((l = l->next));
  }
  return count;
}

void ee_destroy(ee_emitter_t *emitter) {
  ee__event_t *tmp;
  while((tmp = emitter->root)) {
    emitter->root = tmp->next;
    ee__event_destroy(tmp);
  }
}

ee__listener_t *ee__listener_new(EE_CB_TYPE(cb)) {
  ee__listener_t *new_listener =
      (ee__listener_t *)malloc(sizeof(ee__listener_t));
  new_listener->next = 0;
  new_listener->cb = cb;
  return new_listener;
}

ee__listener_t *ee__listener_add(ee__listener_t *listener, EE_CB_TYPE(cb)) {
  if(listener->next == 0) {
    return (listener->next = ee__listener_new(cb));
  } else {
    return ee__listener_add(listener->next, cb);
  }
}

void ee__listener_remove(ee__listener_t *listener, EE_CB_TYPE(cb)) {
  if(listener->next != 0) {
    if(listener->next->cb == cb) {
      ee__listener_t *old = listener->next;
      listener->next = old->next;
      ee__listener_destroy(old);
    } else {
      ee__listener_remove(listener->next, cb);
    }
  }
}

void ee__listener_destroy(ee__listener_t *listener) {
  listener->next = 0;
  listener->cb = 0;
  free(listener);
}

ee__event_t *ee__event_new(int event) {
  ee__event_t *new_event = (ee__event_t *)malloc(sizeof(ee__event_t));
  new_event->id = event;
  new_event->next = 0;
  new_event->root = 0;
  return new_event;
}

ee__event_t *ee__event_find(ee__event_t *element, int event) {
  if(element->next == 0) {
      return (element->next = ee__event_new(event));
  } else if(element->next->id == event) {
    return element->next;
  } else {
    return ee__event_find(element->next, event);
  }
}

void ee__event_destroy(ee__event_t *element) {
  ee__listener_t *tmp;
  element->next = 0;
  while((tmp = element->root)) {
    element->root = tmp->next;
    ee__listener_destroy(tmp);
  }
  free(element);
}

#ifdef __cplusplus
}
#endif
