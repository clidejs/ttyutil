#ifndef TTYUTIL_TTYINPUTWORKER_H_
#define TTYUTIL_TTYINPUTWORKER_H_

#include <node.h>
#include <nan.h>

class TTYInputWorker : public NanAsyncProgressWorker {
public:
    TTYInputWorker(NanCallback *callback, NanCallback *event) :
        NanAsyncProgressWorker(callback), event_(event) {};
    virtual ~TTYInputWorker() {
        delete event_;
    };

    void Execute(const ExecutionProgress& progress);
    void HandleProgressCallback(const char *data, size_t size);

private:
    NanCallback *event_;
};

#endif //TTYUTIL_TTYINPUTWORKER_H_
