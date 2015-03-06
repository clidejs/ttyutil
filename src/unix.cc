#include <ttyu.h>

void ttyu_data_init(ttyu_data_t *data) {
  //place
  //holder
  data->win = initscr();
  noecho();
  //cbreak();
  keypad(data->win, TRUE);
  mousemask(ALL_MOUSE_EVENTS + 1, &(data->old_mouse_mask));
  mouseinterval(0);
}

void ttyu_data_destroy(ttyu_data_t *data) {
  keypad(data->win, FALSE);
  mousemask(data->old_mouse_mask, NULL);
  endwin();
}

bool ttyu_worker_c::execute(const ttyu_worker_c::ttyu_progress_c& progress,
    const ttyu_data_t *data) {
  int c = wgetch(data->win);
  MEVENT mev;
  char ch;
  ttyu_event_t *event = (ttyu_event_t *)malloc(sizeof(ttyu_event_t));

  if(c == ERR) {
    ttyu_event_create_error(event, ERROR_UNIX_UNDEF);
    progress.send(const_cast<const ttyu_event_t *>(event));
  } else if(c == KEY_RESIZE) {
    ttyu_event_create_resize(event);
    progress.send(const_cast<const ttyu_event_t *>(event));
  } else if(c == KEY_MOUSE) {
    if(getmouse(&mev) == OK) {
      ttyu_event_create_mouse(event, EVENT_ERROR, 0, mev.x, mev.y, 0);

      // add button control key sequences if possible
      if(mev.bstate & BUTTON_SHIFT) { event->mouse->ctrl |= CTRL_SHIFT; }
      if(mev.bstate & BUTTON_CTRL) { event->mouse->ctrl |= CTRL_CTRL; }
      if(mev.bstate & BUTTON_ALT) { event->mouse->ctrl |= CTRL_ALT; }

      // convert button codes and mev type
      if(mev.bstate & BUTTON1_RELEASED) {
          event->mouse->button = MOUSE_LEFT;
          event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON1_PRESSED) {
          event->mouse->button = MOUSE_LEFT;
          event->type = EVENT_MOUSEDOWN;

      } else if(mev.bstate & BUTTON2_RELEASED) {
          event->mouse->button = MOUSE_LEFT2;
          event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON2_PRESSED) {
          event->mouse->button = MOUSE_LEFT2;
          event->type = EVENT_MOUSEDOWN;

      } else if(mev.bstate & BUTTON3_RELEASED) {
          event->mouse->button = MOUSE_LEFT3;
          event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON3_PRESSED) {
          event->mouse->button = MOUSE_LEFT3;
          event->type = EVENT_MOUSEDOWN;
      } else
#if NCURSES_MOUSE_VERSION > 1
      if(mev.bstate & BUTTON4_RELEASED) {
          event->mouse->button = MOUSE_LEFT4;
          event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON4_PRESSED) {
          event->mouse->button = MOUSE_LEFT4;
          event->type = EVENT_MOUSEDOWN;

      } else if(mev.bstate & BUTTON5_RELEASED) {
          event->mouse->button = MOUSE_RIGHT;
          event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON5_PRESSED) {
          event->mouse->button = MOUSE_RIGHT;
          event->type = EVENT_MOUSEDOWN;
      }
#else
      if(mev.bstate & BUTTON4_RELEASED) {
          event->mouse->button = MOUSE_RIGHT;
          event->type = EVENT_MOUSEUP;
      } else if(mev.bstate & BUTTON4_PRESSED) {
          event->mouse->button = MOUSE_RIGHT;
          event->type = EVENT_MOUSEDOWN;
      }
#endif
      if(event->type == EVENT_ERROR) {
        // uncaught mouse event
        event->err = ERROR_UNIX_MOUSEUNCAUGHT;
      }
    } else {
      // bad mouse event
      ttyu_event_create_error(event, ERROR_UNIX_MOUSEBAD);
    }
    progress.send(const_cast<const ttyu_event_t *>(event));
  } else {
      ch = *const_cast<char *>(keyname(c));

      // TODO ctrl chars
      // TODO which

      ttyu_event_create_key(event, data->ctrl, ch, c, WHICH_UNKNOWN);
      progress.send(const_cast<const ttyu_event_t *>(event));
  }

  return TRUE;
}
