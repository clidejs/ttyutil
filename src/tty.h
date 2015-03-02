#ifndef TTYUTIL_TTY_H_
#define TTYUTIL_TTY_H_

#include <node.h>
#include <nan.h>

#include "headers/ttyinputworker.h"

using namespace v8;

class TTYUtil : public node::ObjectWrap {
public:
    static void Init(Handle<Object> target);
private:
    explicit TTYUtil(NanCallback *event) : event_(event), running_(false) {};
    ~TTYUtil() {
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
