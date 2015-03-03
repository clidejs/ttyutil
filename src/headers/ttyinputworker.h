#ifndef TTYUTIL_TTYINPUTWORKER_H_
#define TTYUTIL_TTYINPUTWORKER_H_

#include <uv.h>
#include <node.h>
#include <nan.h>

#include "event.h"
#include "mouse.h"
#include "key.h"

using namespace v8;

class TTYInputWorker : public NanAsyncWorker {
public:
    class ExecutionProgress {
        friend class TTYInputWorker;
    public:
        void Send(const ttyutil_event* event) const {
            that_->SendProgress_(event);
        };
    private:
        explicit ExecutionProgress(TTYInputWorker* that) : that_(that) {};
        ExecutionProgress(const ExecutionProgress&);
        void operator=(const ExecutionProgress&);
#if __cplusplus >= 201103L
        // Prohibit C++11 move semantics.
        ExecutionProgress(ExecutionProgress&&) = delete;
        void operator=(ExecutionProgress&&) = delete;
#endif
        TTYInputWorker* const that_;
    };

    TTYInputWorker(NanCallback *callback) :
        NanAsyncWorker(callback), asyncdata_(NULL) {
            async = new uv_async_t;
            uv_async_init(uv_default_loop(), async, AsyncProgress_);
            async->data = this;

            uv_mutex_init(&async_lock);
        };
    virtual ~TTYInputWorker() {
        uv_mutex_destroy(&async_lock);
        asyncdata_ = ttyutil_event_destroy(asyncdata_);
    };

    void WorkProgress() {
        // get the current event
        uv_mutex_lock(&async_lock);
        ttyutil_event *event = asyncdata_;
        asyncdata_ = NULL;
        uv_mutex_unlock(&async_lock);

        if(callback && event) {
            HandleProgressCallback(event);
        }

        ttyutil_event_destroy(event);
    };

    void Execute(const ExecutionProgress& progress);

    void HandleProgressCallback(ttyutil_event *event) {
        NanScope();

        if(event->type == EVENT_MOUSE) {
            Local<Value> argv[] = {
                NanNull(),
                NanNew<Integer>(event->type),
                NanNew<Integer>(event->mouse->button),
                NanNew<Integer>(event->mouse->x),
                NanNew<Integer>(event->mouse->y),
                NanNew<Integer>(event->mouse->action),
                NanNew<Integer>(event->mouse->ctrl)
            };

            callback->Call(7, argv);
        } else if(event->type == EVENT_KEY) {
            Local<Value> argv[] = {
                NanNull(),
                NanNew<Integer>(event->type),
                NanNew<Integer>(event->key->ctrl),
                NanNew<String>(&event->key->c),
                NanNew<Integer>(event->key->code)
            };

            callback->Call(5, argv);
        } else if(event->type == EVENT_RESIZE) {
            Local<Value> argv[] = {
                NanNull(),
                NanNew<Integer>(event->type)
            };

            callback->Call(2, argv);
        } else {
            Local<Value> argv[] = {
                NanNew<String>("unhandled event occured")
            };
            callback->Call(1, argv);
        }
    };

    virtual void Destroy() {
        uv_close(reinterpret_cast<uv_handle_t*>(async), AsyncClose_);
    };
private:
    void Execute() {
        ExecutionProgress progress(this);
        Execute(progress);
    };

    void SendProgress_(const ttyutil_event *event) {
        ttyutil_event *new_event = (struct ttyutil_event *) malloc(sizeof(struct ttyutil_event));
        memcpy(&new_event, &event, sizeof(event));

        uv_mutex_lock(&async_lock);
        ttyutil_event *old_event = asyncdata_;
        asyncdata_ = new_event;
        uv_mutex_unlock(&async_lock);

        old_event = ttyutil_event_destroy(old_event);
        uv_async_send(async);
    };

    NAN_INLINE static NAUV_WORK_CB(AsyncProgress_) {
        TTYInputWorker *worker = static_cast<TTYInputWorker*>(async->data);
        worker->WorkProgress();
    };

    NAN_INLINE static void AsyncClose_(uv_handle_t* handle) {
        TTYInputWorker *worker = static_cast<TTYInputWorker*>(handle->data);
        delete reinterpret_cast<uv_async_t*>(handle);
        delete worker;
    };

    uv_async_t *async;
    uv_mutex_t async_lock;
    ttyutil_event *asyncdata_;
};

#endif // TTYUTIL_TTYINPUTWORKER_H_
