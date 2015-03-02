#ifdef PLATFORM_WINDOWS

#include "../headers/impl.h"

#include "../headers/ttyinputworker.h"
#include "../headers/mouse.h"
#include "../headers/key.h"
#include "../headers/ctrl.h"

int GetMouseAction(DWORD flags, DWORD button);
int GetCtrlCodes(DWORD state);

void TTYInputWorker::Execute(const ExecutionProgress& progress) {
    HANDLE hIn;
    HANDLE hOut;
    DWORD old_mode;
    DWORD new_mode;
    DWORD readed;
    INPUT_RECORD ir[1];
    CONSOLE_SCREEN_BUFFER_INFO conInfo;

    // get stdio handles
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if(INVALID_HANDLE_VALUE == hIn || INVALID_HANDLE_VALUE == hOut) {
        SetErrorMessage("could not get stdio handles");
        return;
    }

    // set necessary console mode
    GetConsoleMode(hIn, &old_mode);
    new_mode =
        ((old_mode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT) &
        ~(ENABLE_QUICK_EDIT_MODE));
    SetConsoleMode(hIn, new_mode);

    // read loop
    read:
        ReadConsoleInput(hIn, ir, 1, &readed);
        if(MOUSE_EVENT == ir[0].EventType) {
            if(!GetConsoleScreenBufferInfo(hOut, &conInfo)) {
                SetErrorMessage("could not read screen buffer info");
                return;
            }

            progress.Send(const_cast<const ttyutil_event*>(ttyutil_event_create(EVENT_MOUSE, ttyutil_mouse_create(
                (int)ir[0].Event.MouseEvent.dwButtonState,
                (int)ir[0].Event.MouseEvent.dwMousePosition.X,
                (int)ir[0].Event.MouseEvent.dwMousePosition.Y - (int)conInfo.srWindow.Top,
                GetMouseAction(ir[0].Event.MouseEvent.dwEventFlags, ir[0].Event.MouseEvent.dwButtonState),
                GetCtrlCodes(ir[0].Event.MouseEvent.dwControlKeyState)
            ))));
        } else if(KEY_EVENT == ir[0].EventType) {
            progress.Send(const_cast<const ttyutil_event*>(ttyutil_event_create(EVENT_KEY, ttyutil_key_create(
                GetCtrlCodes(ir[0].Event.KeyEvent.dwControlKeyState),
                (char)ir[0].Event.KeyEvent.uChar.UnicodeChar,
                (int)ir[0].Event.KeyEvent.wVirtualKeyCode
            ))));
        }
        goto read;

    return;
};

int GetMouseAction(DWORD flags, DWORD button) {
    int action = 0;
    if(button == 0 && flags == 0) { action = MOUSE_ACTION_RELEASED; }
    else if(flags == 0) { action = MOUSE_ACTION_PRESSED; }
    else if(flags == 2) { action = MOUSE_ACTION_DBLCLICKED; }
    else if(flags == 1) { action = MOUSE_ACTION_MOVED; }
    else if(flags == 4) { action = MOUSE_ACTION_WHEELED; }
    else if(flags == 8) { action = MOUSE_ACTION_HWHEELED; }
    return 0;
};

int GetCtrlCodes(DWORD state) {
    int ctrl = CTRL_NULL;
    if(state | RIGHT_ALT_PRESSED || state | LEFT_ALT_PRESSED) { ctrl |= CTRL_ALT; }
    if(state | RIGHT_CTRL_PRESSED || state | RIGHT_CTRL_PRESSED) { ctrl |= CTRL_CTRL; }
    if(state | SHIFT_PRESSED) { ctrl |= CTRL_SHIFT; }
    if(state | ENHANCED_KEY) { ctrl |= CTRL_ENHANCED; }
    if(state | NUMLOCK_ON) { ctrl |= CTRL_NUMLOCK; }
    if(state | SCROLLLOCK_ON) { ctrl |= CTRL_SCROLLLOCK; }
    if(state | CAPSLOCK_ON) { ctrl |= CTRL_CAPSLOCK; }
    return ctrl;
};

#endif // PLATFORM_WINDOWS
