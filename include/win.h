#ifndef TTYU_WIN_H_
#define TTYU_WIN_H_

#include <windows.h>

#define WIN_BUFFER_SIZE 128

typedef struct ttyu_error_s {
  char *msg;
  bool kill;
} ttyu_error_t;
typedef struct ttyu_color_s {
  unsigned long argb;
  short id;
} ttyu_color_t;

#define ERROR_WIN_INIT "could not get console handles - %d"
#define ERROR_WIN_GET "could not get console output buffer information - %d"
#define ERROR_WIN_SET "could not set console output buffer information - %d"
#define ERROR_WIN_FILL "could not fill console output buffer - %d"

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
  ttyu_color_t color_table[WIN_COLORS];
  unsigned long initial_color_table[WIN_COLORS];
  unsigned long base_fg;
  unsigned long base_bg;
};

int ttyu_win_which(DWORD code);
int ttyu_win_ctrl(DWORD state);
bool ttyu_win_scr_update(ttyu_data_t *data, bool initial = FALSE);
short ttyu_win_color(int color, ttyu_data_t *data);
short ttyu_win_color_argb(unsigned long argb, ttyu_data_t *data);
short ttyu_win_color_index(ttyu_color_t table[], unsigned long argb);
short ttyu_win_color_first(ttyu_color_t table[], short index = WIN_COLORS - 1);
short ttyu_win_color_push(ttyu_data_t *data, unsigned long argb);
void ttyu_win_color_update(ttyu_data_t *data, bool close = FALSE);
void ttyu_win_render(char *c, ttyu_data_t *data);
bool ttyu_win_clrscr(ttyu_data_t *data, int x, int y, int width, int height);

#endif // TTYU_WIN_H_
