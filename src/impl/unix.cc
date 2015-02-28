#ifndef PLATFORM_WINDOWS // so we are on unix

#include "../headers/ttyinputworker.h"

#include <stdio.h>
#include <curses.h>
#include <stdlib.h>

#include "../headers/mouse.h"
#include "../headers/key.h"
#include "../headers/ctrl.h"

using namespace v8;

/* TODO any way to implement mouse tracking for VT100 Terminals (at least for Mac Terminal.app)? */

void TTYInputWorker::Execute(const ExecutionProgress& progress) {
    int c;
    int ctrl = CTRL_NULL;
    MEVENT event;

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
                ttyutil_mouse ev = { 0, event.x, event.y, 0, ctrl };

                // add button control key sequences if possible
                if(event.bstate & BUTTON_SHIFT) { ev.ctrl |= CTRL_SHIFT; }
                if(event.bstate & BUTTON_CTRL) { ev.ctrl |= CTRL_CTRL; }
                if(event.bstate & BUTTON_ALT) { ev.ctrl |= CTRL_ALT; }

                // convert button codes and event type
                if(event.bstate & BUTTON1_RELEASED) { ev.button = MOUSE_BUTTON_LEFT; ev.type = MOUSE_TYPE_RELEASED; }
                else if(event.bstate & BUTTON1_PRESSED) { ev.button = MOUSE_BUTTON_LEFT; ev.type = MOUSE_TYPE_PRESSED; }
                else if(event.bstate & BUTTON1_CLICKED) { ev.button = MOUSE_BUTTON_LEFT; ev.type = MOUSE_TYPE_CLICKED; }
                else if(event.bstate & BUTTON1_DOUBLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT; ev.type = MOUSE_TYPE_DBLCLICKED; }
                else if(event.bstate & BUTTON1_TRIPLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT; ev.type = MOUSE_TYPE_TRICLICKED; }

                else if(event.bstate & BUTTON2_RELEASED) { ev.button = MOUSE_BUTTON_LEFT2; ev.type = MOUSE_TYPE_RELEASED; }
                else if(event.bstate & BUTTON2_PRESSED) { ev.button = MOUSE_BUTTON_LEFT2; ev.type = MOUSE_TYPE_PRESSED; }
                else if(event.bstate & BUTTON2_CLICKED) { ev.button = MOUSE_BUTTON_LEFT2; ev.type = MOUSE_TYPE_CLICKED; }
                else if(event.bstate & BUTTON2_DOUBLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT2; ev.type = MOUSE_TYPE_DBLCLICKED; }
                else if(event.bstate & BUTTON2_TRIPLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT2; ev.type = MOUSE_TYPE_TRICLICKED; }

                else if(event.bstate & BUTTON3_RELEASED) { ev.button = MOUSE_BUTTON_LEFT3; ev.type = MOUSE_TYPE_RELEASED; }
                else if(event.bstate & BUTTON3_PRESSED) { ev.button = MOUSE_BUTTON_LEFT3; ev.type = MOUSE_TYPE_PRESSED; }
                else if(event.bstate & BUTTON3_CLICKED) { ev.button = MOUSE_BUTTON_LEFT3; ev.type = MOUSE_TYPE_CLICKED; }
                else if(event.bstate & BUTTON3_DOUBLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT3; ev.type = MOUSE_TYPE_DBLCLICKED; }
                else if(event.bstate & BUTTON3_TRIPLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT3; ev.type = MOUSE_TYPE_TRICLICKED; }

#if NCURSES_MOUSE_VERSION > 1
                else if(event.bstate & BUTTON4_RELEASED) { ev.button = MOUSE_BUTTON_LEFT4; ev.type = MOUSE_TYPE_RELEASED; }
                else if(event.bstate & BUTTON4_PRESSED) { ev.button = MOUSE_BUTTON_LEFT4; ev.type = MOUSE_TYPE_PRESSED; }
                else if(event.bstate & BUTTON4_CLICKED) { ev.button = MOUSE_BUTTON_LEFT4; ev.type = MOUSE_TYPE_CLICKED; }
                else if(event.bstate & BUTTON4_DOUBLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT4; ev.type = MOUSE_TYPE_DBLCLICKED; }
                else if(event.bstate & BUTTON4_TRIPLE_CLICKED) { ev.button = MOUSE_BUTTON_LEFT4; ev.type = MOUSE_TYPE_TRICLICKED; }

                else if(event.bstate & BUTTON5_RELEASED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_RELEASED; }
                else if(event.bstate & BUTTON5_PRESSED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_PRESSED; }
                else if(event.bstate & BUTTON5_CLICKED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_CLICKED; }
                else if(event.bstate & BUTTON5_DOUBLE_CLICKED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_DBLCLICKED; }
                else if(event.bstate & BUTTON5_TRIPLE_CLICKED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_TRICLICKED; }
#else
                else if(event.bstate & BUTTON4_RELEASED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_RELEASED; }
                else if(event.bstate & BUTTON4_PRESSED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_PRESSED; }
                else if(event.bstate & BUTTON4_CLICKED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_CLICKED; }
                else if(event.bstate & BUTTON4_DOUBLE_CLICKED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_DBLCLICKED; }
                else if(event.bstate & BUTTON4_TRIPLE_CLICKED) { ev.button = MOUSE_BUTTON_RIGHT; ev.type = MOUSE_TYPE_TRICLICKED; }
#endif

                // TODO do something with ev
            } else { /* bad one */ }
        } else if(c == KEY_RESIZE) {
            // TODO handle resize events
        } else {
            // key event

            // save ctrl character codes
            if(c == KEY_COMMAND) { ctrl ^= CTRL_CMD; }
            if(c == KEY_SCOMMAND) { ctrl ^= CTRL_CMD ^ CTRL_SHIFT; }
            // TODO there must be more about this

            ttyutil_key ev = { ctrl, (char) c, c };

            // TODO do something with ev
        }

        goto read;
    return;
};

#endif // !PLATFORM_WINDOWS
