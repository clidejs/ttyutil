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

ttyu_js_c *instance = NULL;

void ttyu_init() {
  // register SIGINT handler
  signal(SIGINT, ttyu_destroy);
}

ttyu_js_c *ttyu_get() {
  if(instance == NULL) {
    instance = new ttyu_js_c();
  }
  return instance;
}

void ttyu_destroy(int param) {
  if(instance != NULL) {
    ee_emit(instance->emitter, EVENT_SIGNAL, NanNew<v8::Integer>(param));
    instance->destroy();
  }
  exit(0);
}
