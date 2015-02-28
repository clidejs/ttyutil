#ifndef TTYUTIL_MOUSE_H_
#define TTYUTIL_MOUSE_H_

// unified mouse event
typedef struct _ttyutil_mouse {
    int button;
    int x;
    int y;
    int type;
    int ctrl;
} ttyutil_mouse;

// define buttons
#define MOUSE_BUTTON_LEFT (0x0001)
#define MOUSE_BUTTON_LEFT2 (0x0004)
#define MOUSE_BUTTON_LEFT3 (0x0008)
#define MOUSE_BUTTON_LEFT4 (0x0010)
#define MOUSE_BUTTON_RIGHT (0x0002)

// define event types
#define MOUSE_TYPE_RELEASED (0x0000)
#define MOUSE_TYPE_PRESSED (0x0001)
#define MOUSE_TYPE_CLICKED (0x0002)
#define MOUSE_TYPE_DBLCLICKED (0x0004)
#define MOUSE_TYPE_TRICLICKED (0x0008)
#define MOUSE_TYPE_MOVED (0x0010)
#define MOUSE_TYPE_WHEELED (0x0020)
#define MOUSE_TYPE_HWHEELED (0x0040)

#endif // TTYUTIL_MOUSE_H_
