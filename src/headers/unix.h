#ifndef TTYUTIL_UNIX_H_
#define TTYUTIL_UNIX_H_

// get more definitions
#define NCURSES_OPAQUE FALSE
#include <curses.h>

struct ttyutil_unix_data;
#define TTYUTIL_DATA ttyutil_unix_data

struct ttyutil_unix_data {
    void init();
    void destroy();
    WINDOW *win;
    mmask_t old_mouse_mask;
    int fps;
};

#endif // TTYUTIL_UNIX_H_
