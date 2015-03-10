/* ee - eventemitter for c - ee.h
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

#ifndef EE_H_
#define EE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EE_DATA_TYPE
#define EE_DATA_TYPE void *
#endif

#ifndef EE_DATA_ARG
#define EE_DATA_ARG(name) EE_DATA_TYPE name
#endif

#ifndef EE_CB_ARG
#define EE_CB_ARG(name) void(*name)(EE_DATA_TYPE)
#endif

typedef struct ee__event_s ee__event_t;
typedef struct ee__listener_s ee__listener_t;
typedef struct ee_emitter_s ee_emitter_t;

struct ee__event_s {
  int id; // event identifier
  ee__event_t *next;
  ee__listener_t *root;
};

struct ee__listener_s {
  EE_CB_ARG(cb);
  ee__listener_t *next;
};

struct ee_emitter_s {
  ee__event_t *root;
  int (*emit)(ee__listener_t *, EE_DATA_TYPE);
};

void ee_init(ee_emitter_t *emitter,
    int (*emit)(ee__listener_t *, EE_DATA_TYPE));
void ee_on(ee_emitter_t *emitter, int event, EE_CB_ARG(cb));
void ee_off(ee_emitter_t *emitter, int event, EE_CB_ARG(cb));
int ee_emit(ee_emitter_t *emitter, int event, EE_DATA_ARG(data));
int ee_count(ee_emitter_t *emitter, int event);
void ee_destroy(ee_emitter_t *emitter);

ee__listener_t *ee__listener_new(EE_CB_ARG(cb));
ee__listener_t *ee__listener_add(ee__listener_t *listener, EE_CB_ARG(cb));
void ee__listener_remove(ee__listener_t *listener, EE_CB_ARG(cb));
void ee__listener_destroy(ee__listener_t *listener);

ee__event_t *ee__event_new(int event);
ee__event_t *ee__event_find(ee__event_t *element, int event);
void ee__event_destroy(ee__event_t *element);

#ifdef __cplusplus
}
#endif

#endif // EE_H_
