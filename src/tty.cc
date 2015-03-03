#include "tty.h"

using namespace v8;

void TTY::Init(Handle<Object> target) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(v8::Isolate::GetCurrent(), New);
    tpl->SetClassName(NanNew<String>("TTY"));
    tpl->InstanceTemplate()->SetInternalFieldCount(3);
    // Prototype
    tpl->PrototypeTemplate()->Set(NanNew<String>("start"),
        FunctionTemplate::New(v8::Isolate::GetCurrent(), Start)->GetFunction());
    tpl->PrototypeTemplate()->Set(NanNew<String>("stop"),
        FunctionTemplate::New(v8::Isolate::GetCurrent(), Stop)->GetFunction());
    tpl->InstanceTemplate()->SetAccessor(NanNew<String>("running"), (IsRunning));

    target->Set(NanNew<String>("TTY"), tpl->GetFunction());

    // export event types
    Handle<Object> event = NanNew<Object>();
    EXPORT_CONSTANT(event, "UNDEF", EVENT_UNDEF);
    EXPORT_CONSTANT(event, "MOUSE", EVENT_MOUSE);
    EXPORT_CONSTANT(event, "KEY", EVENT_KEY);
    EXPORT_CONSTANT(event, "RESIZE", EVENT_RESIZE);
    target->Set(NanNew<String>("EVENT"), event);

    // export control key types
    Handle<Object> ctrl = NanNew<Object>();
    EXPORT_CONSTANT(ctrl, "NULL", CTRL_NULL);
    EXPORT_CONSTANT(ctrl, "ALT", CTRL_ALT);
    EXPORT_CONSTANT(ctrl, "CTRL", CTRL_CTRL);
    EXPORT_CONSTANT(ctrl, "SHIFT", CTRL_SHIFT);
    EXPORT_CONSTANT(ctrl, "ENHANCED", CTRL_ENHANCED);
    EXPORT_CONSTANT(ctrl, "CMD", CTRL_CMD);
    EXPORT_CONSTANT(ctrl, "NUMLOCK", CTRL_NUMLOCK);
    EXPORT_CONSTANT(ctrl, "SCROLLLOCK", CTRL_SCROLLLOCK);
    EXPORT_CONSTANT(ctrl, "CAPSLOCK", CTRL_CAPSLOCK);
    target->Set(NanNew<String>("CTRL"), ctrl);

    // export mouse button types
    Handle<Object> mouse = NanNew<Object>();
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT", MOUSE_BUTTON_LEFT);
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT2", MOUSE_BUTTON_LEFT2);
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT3", MOUSE_BUTTON_LEFT3);
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT4", MOUSE_BUTTON_LEFT4);
    EXPORT_CONSTANT(mouse, "BUTTON_RIGHT", MOUSE_BUTTON_RIGHT);

    // export mouse actions
    EXPORT_CONSTANT(mouse, "ACTION_RELEASED", MOUSE_ACTION_RELEASED);
    EXPORT_CONSTANT(mouse, "ACTION_PRESSED", MOUSE_ACTION_PRESSED);
    EXPORT_CONSTANT(mouse, "ACTION_CLICKED", MOUSE_ACTION_CLICKED);
    EXPORT_CONSTANT(mouse, "ACTION_DBLCLICKED", MOUSE_ACTION_DBLCLICKED);
    EXPORT_CONSTANT(mouse, "ACTION_TRICLICKED", MOUSE_ACTION_TRICLICKED);
    EXPORT_CONSTANT(mouse, "ACTION_MOVED", MOUSE_ACTION_MOVED);
    EXPORT_CONSTANT(mouse, "ACTION_WHEELED", MOUSE_ACTION_WHEELED);
    EXPORT_CONSTANT(mouse, "ACTION_HWHEELED", MOUSE_ACTION_HWHEELED);
    target->Set(NanNew<String>("MOUSE"), mouse);
}

NAN_METHOD(TTY::New) {
    NanScope();

    if(!args[0]->IsFunction()) {
        NanThrowTypeError("first argument must be an event callback function");
    }

    TTY* obj = new TTY(new NanCallback(args[0].As<Function>()));
    obj->Wrap(args.This());
    NanReturnThis();
}

NAN_METHOD(TTY::Start) {
    NanScope();
    TTY* obj = ObjectWrap::Unwrap<TTY>(args.This());
    if(!obj->running_) {
        obj->running_ = true;
        TTYInputWorker *w = new TTYInputWorker(obj->event_);
        NanAsyncQueueWorker(w);
        obj->worker_ = w;
    }
    NanReturnThis();
}

NAN_METHOD(TTY::Stop) {
    NanScope();
    TTY* obj = ObjectWrap::Unwrap<TTY>(args.This());
    if(obj->running_) {
        obj->running_ = false;
        obj->worker_->Destroy();
        delete obj->worker_;
    }
    NanReturnThis();
}

NAN_GETTER(TTY::IsRunning) {
    NanScope();
    TTY* obj = ObjectWrap::Unwrap<TTY>(args.This());
    NanReturnValue(NanNew<Boolean>(obj->running_));
}


void Init(Handle<Object> exports) {
    TTY::Init(exports);
}
NODE_MODULE(tty, Init);
