#ifndef TTYUTIL_TTYINPUTWORKER_H_
#define TTYUTIL_TTYINPUTWORKER_H_

#include <uv.h>
#include <node.h>
#include <nan.h>

#include "../../test/dbg.h"

#ifdef PLATFORM_WINDOWS
#   include "win.h"
#else
#   include "unix.h"
#endif

#include "event.h"
#include "mouse.h"
#include "key.h"

class TTYUtil;

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
        ExecutionProgress(ExecutionProgress&&) V8_DELETE;
        void operator=(ExecutionProgress&&) v8_DELETE;
#endif
        TTYInputWorker* const that_;
    };

    TTYInputWorker(TTYUtil *obj) :
        NanAsyncWorker(NULL), asyncdata_(NULL), obj_(obj) {
            async = new uv_async_t;
            uv_mutex_init(&async_lock);
            uv_async_init(uv_default_loop(), async, AsyncProgress_);
            async->data = this;
        };
    virtual ~TTYInputWorker() {
        uv_mutex_destroy(&async_lock);
        asyncdata_ = ttyutil_event_destroy(asyncdata_);
    };

    void WorkProgress() {
DBGA;
        // get the current event
        uv_mutex_lock(&async_lock);
DBGB;
        ttyutil_event *event = asyncdata_;
        asyncdata_ = NULL;
DBGC;
        uv_mutex_unlock(&async_lock);
DBGD;

        if(event) {
            HandleProgressCallback(event);
        }

        ttyutil_event_destroy(event);
    };

    void Execute(const ExecutionProgress& progress);
    void HandleProgressCallback(ttyutil_event *event);

    virtual void Destroy() {
        uv_close(reinterpret_cast<uv_handle_t*>(async), AsyncClose_);
    };
private:
    void Execute() {
        ExecutionProgress progress(this);
        Execute(progress);
    };

    void SendProgress_(const ttyutil_event *event) {
        ttyutil_event *new_event =
                (struct ttyutil_event *) malloc(sizeof(struct ttyutil_event));
        memcpy(&new_event, &event, sizeof(event));

DBGU;
        uv_mutex_lock(&async_lock);
DBGV;
        ttyutil_event *old_event = asyncdata_;
DBGW;
        asyncdata_ = new_event;
DBGX;
        uv_mutex_unlock(&async_lock);

DBGY;
        old_event = ttyutil_event_destroy(old_event);
        uv_async_send(async);
DBGZ;
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
    TTYUtil *obj_;
};

#endif // TTYUTIL_TTYINPUTWORKER_H_
