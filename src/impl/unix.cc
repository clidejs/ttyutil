#ifndef PLATFORM_WINDOWS // so we are on unix

#include "../headers/impl.h"
#include "../headers/tty.h"
#include "../headers/util.h"

#include <stdio.h>

void TTYUTIL_DATA::init() {
    win = initscr();
    noecho();
    cbreak();
    keypad(win, TRUE);
    mousemask(ALL_MOUSE_EVENTS + 1, &old_mouse_mask);
    mouseinterval(0); // disable clicks
    fps = DEFAULT_FPS;
}

void TTYUTIL_DATA::destroy() {
    keypad(win, FALSE);
    mousemask(old_mouse_mask, NULL);
    endwin();
}

bool run(const TTYInputWorker::ExecutionProgress& progress) {
    int c = getch();
    int ctrl = CTRL_NULL;
    MEVENT event;
    char ch;

    if(c == ERR) {
        return TRUE;
    } else if(c == KEY_MOUSE) {
        if(getmouse(&event) == OK) {
            // mouse event
            ttyutil_mouse *ev =
                    ttyutil_mouse_create(0, event.x, event.y, -1, ctrl);

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

            } else if(event.bstate & BUTTON2_RELEASED) {
                ev->button = MOUSE_BUTTON_LEFT2;
                ev->action = MOUSE_ACTION_UP;
            } else if(event.bstate & BUTTON2_PRESSED) {
                ev->button = MOUSE_BUTTON_LEFT2;
                ev->action = MOUSE_ACTION_DOWN;

            } else if(event.bstate & BUTTON3_RELEASED) {
                ev->button = MOUSE_BUTTON_LEFT3;
                ev->action = MOUSE_ACTION_UP;
            } else if(event.bstate & BUTTON3_PRESSED) {
                ev->button = MOUSE_BUTTON_LEFT3;
                ev->action = MOUSE_ACTION_DOWN;
            } else
#if NCURSES_MOUSE_VERSION > 1
            if(event.bstate & BUTTON4_RELEASED) {
                ev->button = MOUSE_BUTTON_LEFT4;
                ev->action = MOUSE_ACTION_UP;
            } else if(event.bstate & BUTTON4_PRESSED) {
                ev->button = MOUSE_BUTTON_LEFT4;
                ev->action = MOUSE_ACTION_DOWN;

            } else if(event.bstate & BUTTON5_RELEASED) {
                ev->button = MOUSE_BUTTON_RIGHT;
                ev->action = MOUSE_ACTION_UP;
            } else if(event.bstate & BUTTON5_PRESSED) {
                ev->button = MOUSE_BUTTON_RIGHT;
                ev->action = MOUSE_ACTION_DOWN;
            }
#else
            if(event.bstate & BUTTON4_RELEASED) {
                ev->button = MOUSE_BUTTON_RIGHT;
                ev->action = MOUSE_ACTION_UP;
            } else if(event.bstate & BUTTON4_PRESSED) {
                ev->button = MOUSE_BUTTON_RIGHT;
                ev->action = MOUSE_ACTION_DOWN;
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
    return FALSE;
}

void TTYInputWorker::Execute(const ExecutionProgress& progress) {
    read:
        if(run(progress)) {
            return;
        } else {
            goto read;
        }
}

NAN_GETTER(TTYUtil::GetWidth) {
    NanScope();
    NanReturnValue(NanNew<v8::Number>(COLS));
}

NAN_GETTER(TTYUtil::GetHeight) {
    NanScope();
    NanReturnValue(NanNew<v8::Number>(LINES));
}

NAN_GETTER(TTYUtil::GetMode) {
    NanScope();
    //TODO get difference to VT102
    NanReturnValue(NanNew<v8::Number>(MODE_VT100));
}

NAN_GETTER(TTYUtil::GetColors) {
    NanScope();
    NanReturnValue(NanNew<v8::Number>(COLORS));
}

NAN_GETTER(TTYUtil::GetX) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    NanReturnValue(NanNew<v8::Number>(obj->data->win->_curx));
}

NAN_SETTER(TTYUtil::SetX) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    obj->data->win->_curx = args.Data()->Int32Value();
}

NAN_GETTER(TTYUtil::GetY) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    NanReturnValue(NanNew<v8::Number>(obj->data->win->_cury));
}

NAN_SETTER(TTYUtil::SetY) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    obj->data->win->_cury = args.Data()->Int32Value();
}

NAN_GETTER(TTYUtil::GetFPS) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    NanReturnValue(NanNew<v8::Number>(obj->data->fps));
}

NAN_SETTER(TTYUtil::SetFPS) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    obj->data->fps = args.Data()->Int32Value();
}

NAN_METHOD(TTYUtil::Goto) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    wmove(obj->data->win, args[1]->Int32Value(), args[0]->Int32Value());
    NanReturnThis();
}

NAN_METHOD(TTYUtil::Color) {
    NanScope();
    v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
    NanReturnValue(NanNew<v8::Integer>(util_color(ch->operator*())));
}

NAN_METHOD(TTYUtil::Beep) {
    NanScope();
    beep();
    NanReturnThis();
}

NAN_METHOD(TTYUtil::Clear) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());

    if(args[0]->IsNumber() && args[1]->IsNumber() && args[2]->IsNumber() &&
            args[3]->IsNumber()) {
        //int x = util_max(args[0]->Int32Value(), 0);
        //int y = util_max(args[1]->Int32Value(), 0);
        //int width = util_min(args[2]->Int32Value(), COLS);
        //int height = util_min(args[3]->Int32Value(), LINES);

        // TODO!
    } else {
        wclear(obj->data->win);
    }
    NanReturnThis();
}

NAN_METHOD(TTYUtil::Prepare) {
    NanScope();
    v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
    int fg = args[1]->IsNumber() ? args[1]->Int32Value() : (
            args[1]->IsString() ? util_color(
            (new v8::String::Utf8Value(args[1]->ToString()))->operator*())
            : - 1);
    int bg = args[2]->IsNumber() ? args[2]->Int32Value() : (
            args[2]->IsString() ? util_color(
            (new v8::String::Utf8Value(args[2]->ToString()))->operator*())
            : - 1);
    NanReturnValue(NanNew<v8::String>(util_render(ch->operator*(), fg, bg)));
}

NAN_METHOD(TTYUtil::Write) {
    NanScope();
    v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
    int fg = args[1]->IsNumber() ? args[1]->Int32Value() : (
            args[1]->IsString() ? util_color(
            (new v8::String::Utf8Value(args[1]->ToString()))->operator*())
            : - 1);
    int bg = args[2]->IsNumber() ? args[2]->Int32Value() : (
            args[2]->IsString() ? util_color(
            (new v8::String::Utf8Value(args[2]->ToString()))->operator*())
            : - 1);
    printf(util_render(ch->operator*(), fg, bg));
    //refresh();
    NanReturnThis();
}

#endif // !PLATFORM_WINDOWS
