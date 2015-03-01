#ifndef TTYUTIL_KEY_H_
#define TTYUTIL_KEY_H_

#include "event.h"

// unified key event
struct ttyutil_key {
    int ctrl;
    char c;
    int code;
};

#endif // TTYUTIL_KEY_H_
