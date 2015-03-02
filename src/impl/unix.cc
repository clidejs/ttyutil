#ifndef PLATFORM_WINDOWS // so we are on unix

#include "../headers/impl.h"

#include <curses.h>

#include "../headers/ttyinputworker.h"
#include "../headers/ctrl.h"

/* TODO any way to implement mouse tracking for VT100 Terminals (at least for Mac Terminal.app)? */
void TTYInputWorker::Execute(const ExecutionProgress& progress) {
    int c;
    int ctrl = CTRL_NULL;
    MEVENT event;
    ttyutil_mouse *ev;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS + 1, NULL);

    read:
        c = getch();

        if(c == ERR) {
            // no event occured
        } else if(c == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                // mouse event
                ev = ttyutil_mouse_create(0, event.x, event.y, 0, ctrl);

                // add button control key sequences if possible
                if(event.bstate & BUTTON_SHIFT) { ev->ctrl |= CTRL_SHIFT; }
                if(event.bstate & BUTTON_CTRL) { ev->ctrl |= CTRL_CTRL; }
                if(event.bstate & BUTTON_ALT) { ev->ctrl |= CTRL_ALT; }

                // convert button codes and event type
                if(event.bstate & BUTTON1_RELEASED) { ev->button = MOUSE_BUTTON_LEFT; ev->action = MOUSE_ACTION_RELEASED; }
                else if(event.bstate & BUTTON1_PRESSED) { ev->button = MOUSE_BUTTON_LEFT; ev->action = MOUSE_ACTION_PRESSED; }
                else if(event.bstate & BUTTON1_CLICKED) { ev->button = MOUSE_BUTTON_LEFT; ev->action = MOUSE_ACTION_CLICKED; }
                else if(event.bstate & BUTTON1_DOUBLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT; ev->action = MOUSE_ACTION_DBLCLICKED; }
                else if(event.bstate & BUTTON1_TRIPLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT; ev->action = MOUSE_ACTION_TRICLICKED; }

                else if(event.bstate & BUTTON2_RELEASED) { ev->button = MOUSE_BUTTON_LEFT2; ev->action = MOUSE_ACTION_RELEASED; }
                else if(event.bstate & BUTTON2_PRESSED) { ev->button = MOUSE_BUTTON_LEFT2; ev->action = MOUSE_ACTION_PRESSED; }
                else if(event.bstate & BUTTON2_CLICKED) { ev->button = MOUSE_BUTTON_LEFT2; ev->action = MOUSE_ACTION_CLICKED; }
                else if(event.bstate & BUTTON2_DOUBLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT2; ev->action = MOUSE_ACTION_DBLCLICKED; }
                else if(event.bstate & BUTTON2_TRIPLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT2; ev->action = MOUSE_ACTION_TRICLICKED; }

                else if(event.bstate & BUTTON3_RELEASED) { ev->button = MOUSE_BUTTON_LEFT3; ev->action = MOUSE_ACTION_RELEASED; }
                else if(event.bstate & BUTTON3_PRESSED) { ev->button = MOUSE_BUTTON_LEFT3; ev->action = MOUSE_ACTION_PRESSED; }
                else if(event.bstate & BUTTON3_CLICKED) { ev->button = MOUSE_BUTTON_LEFT3; ev->action = MOUSE_ACTION_CLICKED; }
                else if(event.bstate & BUTTON3_DOUBLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT3; ev->action = MOUSE_ACTION_DBLCLICKED; }
                else if(event.bstate & BUTTON3_TRIPLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT3; ev->action = MOUSE_ACTION_TRICLICKED; }

#if NCURSES_MOUSE_VERSION > 1
                else if(event.bstate & BUTTON4_RELEASED) { ev->button = MOUSE_BUTTON_LEFT4; ev->action = MOUSE_ACTION_RELEASED; }
                else if(event.bstate & BUTTON4_PRESSED) { ev->button = MOUSE_BUTTON_LEFT4; ev->action = MOUSE_ACTION_PRESSED; }
                else if(event.bstate & BUTTON4_CLICKED) { ev->button = MOUSE_BUTTON_LEFT4; ev->action = MOUSE_ACTION_CLICKED; }
                else if(event.bstate & BUTTON4_DOUBLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT4; ev->action = MOUSE_ACTION_DBLCLICKED; }
                else if(event.bstate & BUTTON4_TRIPLE_CLICKED) { ev->button = MOUSE_BUTTON_LEFT4; ev->action = MOUSE_ACTION_TRICLICKED; }

                else if(event.bstate & BUTTON5_RELEASED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_RELEASED; }
                else if(event.bstate & BUTTON5_PRESSED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_PRESSED; }
                else if(event.bstate & BUTTON5_CLICKED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_CLICKED; }
                else if(event.bstate & BUTTON5_DOUBLE_CLICKED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_DBLCLICKED; }
                else if(event.bstate & BUTTON5_TRIPLE_CLICKED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_TRICLICKED; }
#else
                else if(event.bstate & BUTTON4_RELEASED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_RELEASED; }
                else if(event.bstate & BUTTON4_PRESSED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_PRESSED; }
                else if(event.bstate & BUTTON4_CLICKED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_CLICKED; }
                else if(event.bstate & BUTTON4_DOUBLE_CLICKED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_DBLCLICKED; }
                else if(event.bstate & BUTTON4_TRIPLE_CLICKED) { ev->button = MOUSE_BUTTON_RIGHT; ev->action = MOUSE_ACTION_TRICLICKED; }
#endif

                progress.Send(const_cast<const ttyutil_event*>(ttyutil_event_create(EVENT_MOUSE, ev)));
            } else { /* bad one */ }
        } else if(c == KEY_RESIZE) {
            // TODO handle resize events
        } else {
            // key event

            // TODO save ctrl character codes
            // ctrl =

            progress.Send(const_cast<const ttyutil_event*>(ttyutil_event_create(EVENT_KEY, ttyutil_key_create(ctrl, (char) c, c))));
        }

        goto read;
    return;
};

#endif // !PLATFORM_WINDOWS
