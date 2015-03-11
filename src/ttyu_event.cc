#include <ttyu.h>

void ttyu_event_create_error(ttyu_event_t *event, const char *err) {
  event->type = EVENT_ERROR;
  event->err = err;
  event->key = NULL;
  event->mouse = NULL;
}

void ttyu_event_create_resize(ttyu_event_t *event) {
  event->type = EVENT_RESIZE;
  event->err = NULL;
  event->key = NULL;
  event->mouse = NULL;
}

void ttyu_event_create_key(ttyu_event_t *event, int ctrl, char *c,
    int code, int which) {
  char *ch = (char *)malloc(sizeof(char) * strlen(c));
  memcpy(ch, c, sizeof(char) * strlen(c));

  event->type = EVENT_KEY;
  event->err = NULL;
  event->key = (ttyu_key_t *)malloc(sizeof(ttyu_key_t));
  event->mouse = NULL;

  event->key->ctrl = ctrl;
  event->key->c = ch;
  event->key->code = code;
  event->key->which = which;
}

void ttyu_event_create_mouse(ttyu_event_t *event, int type, int button,
    int x, int y, int ctrl) {
  event->type = type;
  event->err = NULL;
  event->key = NULL;
  event->mouse = (ttyu_mouse_t *)malloc(sizeof(ttyu_mouse_t));

  event->mouse->button = button;
  event->mouse->x = x;
  event->mouse->y = y;
  event->mouse->ctrl = ctrl;
}

void ttyu_event_destroy(ttyu_event_t *event) {
  if(event) {
    if(event->mouse) {
      free(event->mouse);
    }
    if(event->key) {
      if(event->key->c) {
        free(event->key->c);
      }
      free(event->mouse);
    }
  }
}
