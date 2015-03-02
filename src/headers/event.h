#ifndef TTYUTIL_EVENT_H_
#define TTYUTIL_EVENT_H_

#define EVENT_UNDEF     0x00
#define EVENT_MOUSE     0x01
#define EVENT_KEY       0x02
#define EVENT_RESIZE    0x03

struct ttyutil_key;
struct ttyutil_mouse;

struct ttyutil_event {
    int type;
    ttyutil_key *key;
    ttyutil_mouse *mouse;
};

ttyutil_event* ttyutil_event_destroy(ttyutil_event *ptr);
ttyutil_event* ttyutil_event_create(int type, void *ptr);

#endif // TTYUTIL_EVENT_H_
