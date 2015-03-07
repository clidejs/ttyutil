#ifndef TTYU_WIN_H_
#define TTYU_WIN_H_

#include <windows.h>

#define WIN_BUFFER_SIZE 128

#define ERROR_WIN_INIT "could not get console handles"
#define ERROR_WIN_BUFFER "could not get console output buffer information"


// unixy data structure
struct ttyu_data_s {
  HANDLE hIn;
  HANDLE hOut;
  DWORD old_mode;
  bool init_error;
};

int ttyu_win_ctrl(DWORD state);

#endif // TTYU_WIN_H_
