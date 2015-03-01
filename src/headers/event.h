#ifndef TTYUTIL_EVENT_H_
#define TTYUTIL_EVENT_H_

#define EVENT_UNDEF     0x00
#define EVENT_MOUSE     0x01
#define EVENT_KEY       0x02

struct ttyutil_key;
struct ttyutil_mouse;

struct ttyutil_event {
    int type = EVENT_UNDEF;
    ttyutil_key *key;
    ttyutil_mouse *mouse;
};

#endif // TTYUTIL_EVENT_H_
