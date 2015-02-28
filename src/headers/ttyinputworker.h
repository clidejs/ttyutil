#ifndef TTYUTIL_TTYINPUTWORKER_H_
#define TTYUTIL_TTYINPUTWORKER_H_

#include <node.h>
#include <nan.h>
#include <string.h>

using namespace v8;

class TTYInputWorker : public NanAsyncProgressWorker {
public:
    TTYInputWorker(NanCallback *callback, NanCallback *event) :
        NanAsyncProgressWorker(callback), event_(event) {};
    virtual ~TTYInputWorker() {
        delete event_;
    };

    void Execute(const ExecutionProgress& progress);
    void HandleProgressCallback(const char *data, size_t size) {
        NanScope();

        // 30 should work out for every buffer size
        // TODO performance improvement?!
        char out[30];
        strncpy(out, data, size);
        out[size] = '\0';

        Local<Value> argv[] = {
            NanNew<String>(out)
        };

        event_->Call(1, argv);
    };

private:
    NanCallback *event_;
};

#endif //TTYUTIL_TTYINPUTWORKER_H_
