#ifndef TTYUTIL_TTY_H_
#define TTYUTIL_TTY_H_

#include <node.h>
#include <nan.h>

#include "headers/ttyinputworker.h"
#include "headers/ctrl.h"

#define EXPORT_CONSTANT(target, name, constant)                                     \
    do {                                                                            \
        v8::Isolate* isolate = v8::Isolate::GetCurrent();                           \
        v8::Local<v8::String> constant_name =                                       \
                v8::String::NewFromUtf8(isolate, name);                             \
        v8::Local<v8::Number> constant_value =                                      \
                v8::Number::New(isolate, static_cast<double>(constant));            \
        v8::PropertyAttribute constant_attributes =                                 \
                static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete);  \
        (target)->ForceSet(constant_name, constant_value, constant_attributes);     \
    } while (0)


using namespace v8;

class TTY : public node::ObjectWrap {
public:
    static void Init(Handle<Object> target);
private:
    explicit TTY(NanCallback *event) : event_(event), running_(false) {};
    ~TTY() {
        delete event_;
        delete worker_;
    };

    static NAN_METHOD(New);
    static NAN_METHOD(Start);
    static NAN_METHOD(Stop);
    static NAN_GETTER(IsRunning);

    NanCallback *event_;
    bool running_;
    TTYInputWorker *worker_;
};

#endif //TTYUTIL_TTY_H_
