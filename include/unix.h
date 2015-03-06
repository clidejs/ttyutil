#ifndef TTYU_UNIX_H_
#define TTYU_UNIX_H_

#define NCURSES_OPAQUE FALSE
#include <curses.h>

// unixy data structure
struct ttyu_data_s {
    WINDOW *win;
    mmask_t old_mouse_mask;
};

void ttyu_data_init(ttyu_data_t *data) {
    data->win = initscr();
    noecho();
    cbreak();
    keypad(data->win, TRUE);
    mousemask(ALL_MOUSE_EVENTS + 1, &(data->old_mouse_mask));
    mouseinterval(0);
}

void ttyu_data_destroy(ttyu_data_t *data) {
    keypad(data->win, FALSE);
    mousemask(data->old_mouse_mask, NULL);
    endwin();
}

#endif // TTYU_UNIX_H_
