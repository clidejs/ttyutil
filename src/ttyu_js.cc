#include <ttyu.h>

v8::Persistent<v8::Function> ttyu_js_c::constructor;

void ttyu_js_c::init(v8::Handle<v8::Object> target) {
  ttyu_init();

  v8::Local<v8::FunctionTemplate> tpl =
      NanNew<v8::FunctionTemplate>(new_instance);
  tpl->SetClassName(NanNew<v8::String>("TTYUtil"));
  tpl->InstanceTemplate()->SetInternalFieldCount(17);

  EXPORT_PROTOTYPE_METHOD(tpl, "start", start);
  EXPORT_PROTOTYPE_METHOD(tpl, "stop", stop);
  EXPORT_PROTOTYPE_METHOD(tpl, "__on__", on);
  EXPORT_PROTOTYPE_METHOD(tpl, "__off__", off);
  EXPORT_PROTOTYPE_GET(tpl, "running", is_running);

  EXPORT_PROTOTYPE_GET(tpl, "width", get_width);
  EXPORT_PROTOTYPE_GET(tpl, "height", get_height);
  EXPORT_PROTOTYPE_GET(tpl, "mode", get_mode);
  EXPORT_PROTOTYPE_GET(tpl, "colors", get_colors);
  EXPORT_PROTOTYPE_GETSET(tpl, "x", getx, setx);
  EXPORT_PROTOTYPE_GETSET(tpl, "y", gety, sety);
  EXPORT_PROTOTYPE_METHOD(tpl, "goto", gotoxy);
  EXPORT_PROTOTYPE_METHOD(tpl, "color", color);
  EXPORT_PROTOTYPE_METHOD(tpl, "beep", beep);
  EXPORT_PROTOTYPE_METHOD(tpl, "clear", clear);
  EXPORT_PROTOTYPE_METHOD(tpl, "prepare", prepare);
  EXPORT_PROTOTYPE_METHOD(tpl, "write", write);

  NanAssignPersistent(constructor, tpl->GetFunction());
  target->Set(NanNew<v8::String>("TTYUtil"), tpl->GetFunction());
}

ttyu_js_c::ttyu_js_c() {
  running_ = FALSE;

  data = (ttyu_data_t *)malloc(sizeof(ttyu_data_t));
  ttyu_data_init(data);

  emitter = (ee_emitter_t *)malloc(sizeof(ee_emitter_t));
  ee_init(emitter, ttyu_ee_cb_call);
}

ttyu_js_c::~ttyu_js_c() {
  destroy();
}

void ttyu_js_c::destroy() {
  if(worker_) {
    worker_->destroy();
    delete worker_;
  }
  ee_destroy(emitter);
  ttyu_data_destroy(data);
}

NAN_METHOD(ttyu_js_c::new_instance) {
  NanScope();
  ttyu_js_c *obj = ttyu_get();
  obj->Wrap(args.This());
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::start) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  if(!obj->running_) {
    obj->running_ = true;

    ttyu_worker_c *w = new ttyu_worker_c(obj);
    NanAsyncQueueWorker(w);
    obj->worker_ = w;
  }
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::stop) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  if(obj->running_) {
    obj->running_ = false;
    obj->worker_->destroy();
    delete obj->worker_;
  }
  NanReturnThis();
}

NAN_GETTER(ttyu_js_c::is_running) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  NanReturnValue(NanNew<v8::Boolean>(obj->running_));
}

NAN_METHOD(ttyu_js_c::on) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  ee_on(obj->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  NanReturnThis();
}

NAN_METHOD(ttyu_js_c::off) {
  NanScope();
  ttyu_js_c *obj = ObjectWrap::Unwrap<ttyu_js_c>(args.This());
  ee_off(obj->emitter, args[0]->Int32Value(),
      new NanCallback(v8::Local<v8::Function>::Cast(args[1])));
  NanReturnThis();
}


// export
NODE_MODULE(ttyu, ttyu_js_c::init);
