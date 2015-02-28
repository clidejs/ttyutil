#ifdef PLATFORM_WINDOWS

#include "../headers/ttyinputworker.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/mouse.h"
#include "../headers/key.h"
#include "../headers/ctrl.h"

using namespace v8;

int GetMouseType(DWORD flags, DWORD button);
int GetCtrlCodes(DWORD state);

void TTYInputWorker::Execute(const NanAsyncProgressWorker::ExecutionProgress& progress) {
    HANDLE hIn;
    HANDLE hOut;
    DWORD old_mode;
    DWORD new_mode;
    DWORD readed;
    INPUT_RECORD ir[1];
    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    SMALL_RECT windowSize;
    std::string buf;

    SetErrorMessage("unknown error!");

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
            windowSize = conInfo.srWindow;

            ttyutil_mouse ev = {
                (int)ir[0].Event.MouseEvent.dwButtonState,
                (int)ir[0].Event.MouseEvent.dwMousePosition.X,
                (int)ir[0].Event.MouseEvent.dwMousePosition.Y - (int)windowSize.Top,
                GetMouseType(ir[0].Event.MouseEvent.dwEventFlags, ir[0].Event.MouseEvent.dwButtonState),
                GetCtrlCodes(ir[0].Event.MouseEvent.dwControlKeyState)
            };

             // TODO do something with ev
        } else if(KEY_EVENT == ir[0].EventType) {

            ttyutil_key ev = {
                GetCtrlCodes(ir[0].Event.KeyEvent.dwControlKeyState),
                (char)ir[0].Event.KeyEvent.uChar.UnicodeChar,
                (int)ir[0].Event.KeyEvent.wVirtualKeyCode
            };

             // TODO do something with ev
        }
        goto read;

    return;
};

#endif // PLATFORM_WINDOWS
