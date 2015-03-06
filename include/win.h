#ifndef TTYU_WIN_H_
#define TTYU_WIN_H_

#include <windows.h>

#define WIN_BUFFER_SIZE 128

// unixy data structure
struct ttyu_data_s {
    HANDLE hIn;
    HANDLE hOut;
    DWORD old_mode;
};

#endif // TTYU_WIN_H_
