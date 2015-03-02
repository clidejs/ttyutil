#ifdef PLATFORM_WINDOWS

#include "../headers/impl.h"

#include "../headers/ttyinputworker.h"
#include "../headers/mouse.h"
#include "../headers/key.h"
#include "../headers/ctrl.h"

#define INPUT_BUFFER_SIZE 128

int GetMouseAction(DWORD flags, DWORD button);
int GetCtrlCodes(DWORD state);

void TTYInputWorker::Execute(const ExecutionProgress& progress) {
    HANDLE hIn;
    HANDLE hOut;
    DWORD old_mode;
    DWORD new_mode;
    DWORD readed;
    INPUT_RECORD ir[INPUT_BUFFER_SIZE];
    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    DWORD i;

    // get stdio handles
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if(INVALID_HANDLE_VALUE == hIn || INVALID_HANDLE_VALUE == hOut) {
        SetErrorMessage("could not get stdio handles");
        return;
    }

    // set necessary console mode
    GetConsoleMode(hIn, &old_mode);
    new_mode = ((old_mode | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT) & ~(ENABLE_QUICK_EDIT_MODE));
    SetConsoleMode(hIn, new_mode);

    // read loop
    read:
        ReadConsoleInput(hIn, ir, INPUT_BUFFER_SIZE, &readed);

        for(i = 0; i < readed; ++i) {
            if(!GetConsoleScreenBufferInfo(hOut, &conInfo)) {
                SetErrorMessage("could not read screen buffer info");
                return;
            }

            if(MOUSE_EVENT == ir[i].EventType) {
                progress.Send(const_cast<const ttyutil_event*>(ttyutil_event_create(EVENT_MOUSE, ttyutil_mouse_create(
                    (int)ir[i].Event.MouseEvent.dwButtonState,
                    (int)ir[i].Event.MouseEvent.dwMousePosition.X,
                    (int)ir[i].Event.MouseEvent.dwMousePosition.Y - (int)conInfo.srWindow.Top,
                    GetMouseAction(ir[i].Event.MouseEvent.dwEventFlags, ir[i].Event.MouseEvent.dwButtonState),
                    GetCtrlCodes(ir[i].Event.MouseEvent.dwControlKeyState)))));
            } else if(KEY_EVENT == ir[i].EventType) {
                progress.Send(const_cast<const ttyutil_event*>(ttyutil_event_create(EVENT_KEY, ttyutil_key_create(
                    GetCtrlCodes(ir[i].Event.KeyEvent.dwControlKeyState),
                    (char)ir[i].Event.KeyEvent.uChar.UnicodeChar,
                    (int)ir[i].Event.KeyEvent.wVirtualKeyCode))));
            } else if(WINDOW_BUFFER_SIZE_EVENT == ir[i].EventType) {
                progress.Send(const_cast<const ttyutil_event*>(ttyutil_event_create(EVENT_RESIZE, NULL)));
            }
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
