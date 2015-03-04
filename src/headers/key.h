#ifndef TTYUTIL_KEY_H_
#define TTYUTIL_KEY_H_

#include "event.h"

// unified key event
struct ttyutil_key {
    int ctrl;
    char c;
    int code;
    int which;
};

#define KEY_WHICH_UNDEF 0x0000
//TODO

ttyutil_key* ttyutil_key_destroy(ttyutil_key *ptr);
ttyutil_key* ttyutil_key_create(int ctrl, char c, int code, int which);

#endif // TTYUTIL_KEY_H_
