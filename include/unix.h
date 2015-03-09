#ifndef TTYU_UNIX_H_
#define TTYU_UNIX_H_

#define NCURSES_OPAQUE FALSE
#include <curses.h>

#define ERROR_UNIX_UNDEF "unknown error occured while reading input"
#define ERROR_UNIX_MOUSEBAD "skipping unreadable mouse event"
#define ERROR_UNIX_MOUSEUNCAUGHT "skipping unknown mouse event"

// unixy data structure
struct ttyu_data_s {
  WINDOW *win;
  mmask_t old_mouse_mask;
  int ctrl;
  int mode;
};

#endif // TTYU_UNIX_H_
