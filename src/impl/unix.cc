#ifndef PLATFORM_WINDOWS // so we are on unix

#include "../headers/impl.h"
#include "../headers/tty.h"

#include <curses.h>

/* TODO any way to implement mouse tracking for VT100 Terminals
    (at least for Mac Terminal.app)? */
void TTYInputWorker::Execute(const ExecutionProgress& progress) {
    int c;
    int ctrl = CTRL_NULL;
    MEVENT event;
    ttyutil_mouse *ev;
    char ch;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    mousemask(ALL_MOUSE_EVENTS + 1, NULL);

    read:
        c = getch();

        if(c == ERR) {
            goto end;
        } else if(c == KEY_MOUSE) {
            if(getmouse(&event) == OK) {
                // mouse event
                ev = ttyutil_mouse_create(0, event.x, event.y, -1, ctrl);

                // add button control key sequences if possible
                if(event.bstate & BUTTON_SHIFT) { ev->ctrl |= CTRL_SHIFT; }
                if(event.bstate & BUTTON_CTRL) { ev->ctrl |= CTRL_CTRL; }
                if(event.bstate & BUTTON_ALT) { ev->ctrl |= CTRL_ALT; }

                // convert button codes and event type
                if(event.bstate & BUTTON1_RELEASED) {
                    ev->button = MOUSE_BUTTON_LEFT;
                    ev->action = MOUSE_ACTION_UP;
                } else if(event.bstate & BUTTON1_PRESSED) {
                    ev->button = MOUSE_BUTTON_LEFT;
                    ev->action = MOUSE_ACTION_DOWN;
                } else if(event.bstate & BUTTON1_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT;
                    //ev->action = MOUSE_ACTION_CLICKED;
                } else if(event.bstate & BUTTON1_DOUBLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT;
                    //ev->action = MOUSE_ACTION_DBLCLICKED;
                } else if(event.bstate & BUTTON1_TRIPLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT;
                    //ev->action = MOUSE_ACTION_TRICLICKED;

                } else if(event.bstate & BUTTON2_RELEASED) {
                    ev->button = MOUSE_BUTTON_LEFT2;
                    ev->action = MOUSE_ACTION_UP;
                } else if(event.bstate & BUTTON2_PRESSED) {
                    ev->button = MOUSE_BUTTON_LEFT2;
                    ev->action = MOUSE_ACTION_DOWN;
                } else if(event.bstate & BUTTON2_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT2;
                    //ev->action = MOUSE_ACTION_CLICKED;
                } else if(event.bstate & BUTTON2_DOUBLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT2;
                    //ev->action = MOUSE_ACTION_DBLCLICKED;
                } else if(event.bstate & BUTTON2_TRIPLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT2;
                    //ev->action = MOUSE_ACTION_TRICLICKED;

                } else if(event.bstate & BUTTON3_RELEASED) {
                    ev->button = MOUSE_BUTTON_LEFT3;
                    ev->action = MOUSE_ACTION_UP;
                } else if(event.bstate & BUTTON3_PRESSED) {
                    ev->button = MOUSE_BUTTON_LEFT3;
                    ev->action = MOUSE_ACTION_DOWN;
                } else if(event.bstate & BUTTON3_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT3;
                    //ev->action = MOUSE_ACTION_CLICKED;
                } else if(event.bstate & BUTTON3_DOUBLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT3;
                    //ev->action = MOUSE_ACTION_DBLCLICKED;
                } else if(event.bstate & BUTTON3_TRIPLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT3;
                    //ev->action = MOUSE_ACTION_TRICLICKED;
                } else
#if NCURSES_MOUSE_VERSION > 1
                if(event.bstate & BUTTON4_RELEASED) {
                    ev->button = MOUSE_BUTTON_LEFT4;
                    ev->action = MOUSE_ACTION_UP;
                } else if(event.bstate & BUTTON4_PRESSED) {
                    ev->button = MOUSE_BUTTON_LEFT4;
                    ev->action = MOUSE_ACTION_DOWN;
                } else if(event.bstate & BUTTON4_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT4;
                    //ev->action = MOUSE_ACTION_CLICKED;
                } else if(event.bstate & BUTTON4_DOUBLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT4;
                    //ev->action = MOUSE_ACTION_DBLCLICKED;
                } else if(event.bstate & BUTTON4_TRIPLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_LEFT4;
                    //ev->action = MOUSE_ACTION_TRICLICKED;

                } else if(event.bstate & BUTTON5_RELEASED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    ev->action = MOUSE_ACTION_UP;
                } else if(event.bstate & BUTTON5_PRESSED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    ev->action = MOUSE_ACTION_DOWN;
                } else if(event.bstate & BUTTON5_CLICKED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    //ev->action = MOUSE_ACTION_CLICKED;
                } else if(event.bstate & BUTTON5_DOUBLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    //ev->action = MOUSE_ACTION_DBLCLICKED;
                } else if(event.bstate & BUTTON5_TRIPLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    //ev->action = MOUSE_ACTION_TRICLICKED;
                }
#else
                if(event.bstate & BUTTON4_RELEASED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    ev->action = MOUSE_ACTION_UP;
                } else if(event.bstate & BUTTON4_PRESSED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    ev->action = MOUSE_ACTION_DOWN;
                } else if(event.bstate & BUTTON4_CLICKED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    //ev->action = MOUSE_ACTION_CLICKED;
                } else if(event.bstate & BUTTON4_DOUBLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    //ev->action = MOUSE_ACTION_DBLCLICKED;
                } else if(event.bstate & BUTTON4_TRIPLE_CLICKED) {
                    ev->button = MOUSE_BUTTON_RIGHT;
                    //ev->action = MOUSE_ACTION_TRICLICKED;
                }
#endif

                progress.Send(const_cast<const ttyutil_event*>(
                        ttyutil_event_create(EVENT_MOUSE, ev)));
            } else { /* bad one */ }
        } else if(c == KEY_RESIZE) {
            progress.Send(const_cast<const ttyutil_event*>(
                    ttyutil_event_create(EVENT_RESIZE, NULL)));
        } else {
            // key event
            ch = *const_cast<char *>(keyname(c));

            // TODO save ctrl character codes
            // ctrl =

            progress.Send(const_cast<const ttyutil_event*>(
                    ttyutil_event_create(EVENT_KEY,
                            ttyutil_key_create(ctrl, ch, c, -1))));
        }
        goto read;

    end:
        return;
}

NAN_GETTER(TTYUtil::GetWidth) {
    NanScope();
    NanReturnValue(NanNew<v8::Number>(COLS));
}

#endif // !PLATFORM_WINDOWS
