#ifndef TTYUTIL_KEY_H_
#define TTYUTIL_KEY_H_

// unified key event
typedef struct _ttyutil_key {
    int ctrl;
    char c;
    int code;
} ttyutil_key;

#endif // TTYUTIL_KEY_H_
