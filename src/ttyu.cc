#include <ttyu.h>

int ttyu_ee_cb_call(ee__listener_t *l, v8::Local<v8::Value> data) {
  v8::Local<v8::Value> args[] = {
    data
  };
  int count = 0;
  do {
    if(l->cb) {
      l->cb->Call(1, args);
    }
    ++count;
  } while((l = l->next));
  return count;
}
