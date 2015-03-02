#ifndef TTYUTIL_KEY_H_
#define TTYUTIL_KEY_H_

#include "event.h"

// unified key event
struct ttyutil_key {
    int ctrl;
    char c;
    int code;
};

ttyutil_key* ttyutil_key_destroy(ttyutil_key *ptr);
ttyutil_key* ttyutil_key_create(int ctrl, char c, int code);

#endif // TTYUTIL_KEY_H_
