#ifdef PLATFORM_WINDOWS

#include "ttyinputworker.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace v8;

void TTYInputWorker::Execute(const NanAsyncProgressWorker::ExecutionProgress& progress) {
    HANDLE hIn;
    HANDLE hOut;
    DWORD old_mode;
    DWORD new_mode;
    DWORD readed;
    INPUT_RECORD ir[1];
    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    SMALL_RECT windowSize;

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
            std::string buf = "m ";
            buf += std::to_string((int)ir[0].Event.MouseEvent.dwEventFlags);
            buf += " ";
            buf += std::to_string((int)ir[0].Event.MouseEvent.dwButtonState);
            buf += " ";
            buf += std::to_string((int)ir[0].Event.MouseEvent.dwControlKeyState);
            buf += " ";
            buf += std::to_string((int)ir[0].Event.MouseEvent.dwMousePosition.X);
            buf += " ";
            buf += std::to_string((int)ir[0].Event.MouseEvent.dwMousePosition.Y - (int)windowSize.Top);
            const char *data = buf.c_str();
            progress.Send(data, strlen(data));
        } else if(KEY_EVENT == ir[0].EventType) {
            std::string buf = "k ";
            buf += std::to_string((int)ir[0].Event.KeyEvent.wVirtualKeyCode);
            buf += " ";
            buf += std::to_string((int)ir[0].Event.KeyEvent.bKeyDown);
            buf += " ";
            buf += std::to_string((int)ir[0].Event.KeyEvent.wRepeatCount);
            buf += " ";
            buf += std::to_string((int)ir[0].Event.KeyEvent.dwControlKeyState);
            buf += " ";
            buf += (wchar_t)(ir[0].Event.KeyEvent.uChar.UnicodeChar);
            const char *data = buf.c_str();
            progress.Send(data, strlen(data));
        }
        goto read;

    return;
};

void TTYInputWorker::HandleProgressCallback(const char *data, size_t size) {
    NanScope();

    // 30 should work out for every buffer size
    // TODO performance improvement?!
    char out[30];
    strncpy(out, data, size);
    out[size] = '\0';

    Local<String> type;

    if(out[0] == 'k') {
        type = NanNew<String>("key");
    } else {
        type = NanNew<String>("mouse");

        // only listen for mouse-press
        if(out[3] == '0') {

        }
    }
    Local<Value> argv[] = {
        NanNew<String>(out)
    };

    event_->Call(1, argv);
};

#endif // PLATFORM_WINDOWS
