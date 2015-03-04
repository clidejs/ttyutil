#ifndef TTYUTIL_IMPL_H_
#define TTYUTIL_IMPL_H_

#include <node.h>
#include <stdlib.h>
#include <stdio.h>

#include "event.h"
#include "mouse.h"
#include "key.h"

ttyutil_key* ttyutil_key_destroy(ttyutil_key *ptr) {
    if(ptr) { delete[] ptr; }
    return NULL;
};
ttyutil_key* ttyutil_key_create(int ctrl, char c, int code, int which) {
    ttyutil_key *ev = (struct ttyutil_key *) malloc(sizeof(struct ttyutil_key));
    ev->ctrl = ctrl;
    ev->c = c;
    ev->code = code;
    ev->which = which;

    return ev;
};

ttyutil_mouse* ttyutil_mouse_destroy(ttyutil_mouse *ptr) {
    if(ptr) { delete[] ptr; }
    return NULL;
};
ttyutil_mouse* ttyutil_mouse_create(int button, int x, int y, int action,
        int ctrl) {
    ttyutil_mouse *ev =
            (struct ttyutil_mouse *) malloc(sizeof(struct ttyutil_mouse));
    ev->button = button;
    ev->x = x;
    ev->y = y;
    ev->action = action;
    ev->ctrl = ctrl;

    return ev;
};

ttyutil_event* ttyutil_event_destroy(ttyutil_event *ptr) {
    if(ptr) {
        if(ptr->key) { ptr->key = ttyutil_key_destroy(ptr->key); }
        if(ptr->mouse) { ptr->mouse = ttyutil_mouse_destroy(ptr->mouse); }

        delete[] ptr;
    }
    return NULL;
};
ttyutil_event* ttyutil_event_create(int type, void *ptr) {
    struct ttyutil_event *event =
        (struct ttyutil_event *) malloc(sizeof(struct ttyutil_event));
    event->type = type;
    event->key = NULL;
    event->mouse = NULL;

    switch(type) {
        case EVENT_MOUSE:
            event->mouse = (struct ttyutil_mouse *) ptr;
            break;
        case EVENT_KEY:
            event->key = (struct ttyutil_key *) ptr;
            break;
        case EVENT_RESIZE:
            // everything is already initialized correctly
            break;
        default:
            event->type = EVENT_UNDEF;
            break;
    }

    return event;
};

#endif // TTYUTIL_IMPL_H_
