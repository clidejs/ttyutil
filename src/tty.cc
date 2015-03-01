#include "tty.h"

using namespace v8;

void TTYUtil::Init(Handle<Object> target) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(v8::Isolate::GetCurrent(), New);
    tpl->SetClassName(NanNew<String>("TTYUtil"));
    tpl->InstanceTemplate()->SetInternalFieldCount(3);
    // Prototype
    tpl->PrototypeTemplate()->Set(NanNew<String>("start"),
        FunctionTemplate::New(v8::Isolate::GetCurrent(), Start)->GetFunction());
    tpl->PrototypeTemplate()->Set(NanNew<String>("stop"),
        FunctionTemplate::New(v8::Isolate::GetCurrent(), Stop)->GetFunction());
    tpl->InstanceTemplate()->SetAccessor(NanNew<String>("running"), (IsRunning));

    target->Set(NanNew<String>("exports"), tpl->GetFunction());
}

NAN_METHOD(TTYUtil::New) {
    NanScope();

    if(!args[0]->IsFunction()) {
        NanThrowTypeError("first argument must be an event callback function");
    }

    TTYUtil* obj = new TTYUtil(new NanCallback(args[0].As<Function>()));
    obj->Wrap(args.This());
    NanReturnThis();
}

NAN_METHOD(TTYUtil::Start) {
    NanScope();
    TTYUtil* obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    if(!obj->running_) {
        obj->running_ = true;
        TTYInputWorker *w = new TTYInputWorker(obj->event_);
        NanAsyncQueueWorker(w);
        obj->worker_ = w;
    }
    NanReturnThis();
}

NAN_METHOD(TTYUtil::Stop) {
    NanScope();
    TTYUtil* obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    if(obj->running_) {
        obj->running_ = false;
        obj->worker_->Destroy();
        delete obj->worker_;
    }
    NanReturnThis();
}

NAN_GETTER(TTYUtil::IsRunning) {
    NanScope();
    TTYUtil* obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    NanReturnValue(NanNew<Boolean>(obj->running_));
}


void Init(Handle<Object> exports, Handle<Object> module) {
    TTYUtil::Init(module);
}
NODE_MODULE(tty, Init);
