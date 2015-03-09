#ifndef TTYU_WIN_H_
#define TTYU_WIN_H_

#include <windows.h>

#define WIN_BUFFER_SIZE 128
#define WIN_COLORS 16

typedef struct ttyu_error_s ttyu_error_t;

#define ERROR_WIN_INIT "could not get console handles"
#define ERROR_WIN_GET "could not get console output buffer information"
#define ERROR_WIN_SET "could not set console output buffer information"
#define ERROR_WIN_FILL "could not fill console output buffer"

// windows data structure
struct ttyu_data_s {
  HANDLE hin;
  HANDLE hout;
  DWORD old_mode;
  ttyu_error_t *err;
  int width;
  int height;
  int top;
  int curx;
  int cury;
  unsigned long color_table[WIN_COLORS];
  unsigned long initial_color_table[WIN_COLORS];
};

// windows console error
struct ttyu_error_s {
  char *msg;
  bool kill;
};

int ttyu_win_ctrl(DWORD state);
bool ttyu_win_scr_update(ttyu_data_t *data, bool initial = FALSE);
int ttyu_win_color(char *c, ttyu_data_t *data);
void ttyu_win_render(char *c, ttyu_data_t *data);
bool ttyu_win_clrscr(ttyu_data_t *data, int x, int y, int width, int height);

#endif // TTYU_WIN_H_
