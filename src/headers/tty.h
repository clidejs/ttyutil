#ifndef TTYUTIL_TTY_H_
#define TTYUTIL_TTY_H_

#include <nan.h>

#include "ttyinputworker.h"
#include "ctrl.h"

struct ttyutil_eventlistener {
    NanCallback *listener;
    struct ttyutil_eventlistener *next;
};

void ttyutil_eventlistener_add(ttyutil_eventlistener **root,
        ttyutil_eventlistener *ptr);
bool ttyutil_eventlistener_delete(ttyutil_eventlistener **root,
        NanCallback *cb);
void ttyutil_eventlistener_exec(ttyutil_eventlistener **root, const int len,
        v8::Local<v8::Value> argv[]);

#define EXPORT_CONSTANT(target, name, constant)                                \
    do {                                                                       \
        v8::Isolate* isolate = v8::Isolate::GetCurrent();                      \
        v8::Local<v8::String> constant_name =                                  \
                v8::String::NewFromUtf8(isolate, name);                        \
        v8::Local<v8::Number> constant_value =                                 \
                v8::Number::New(isolate, static_cast<double>(constant));       \
        v8::PropertyAttribute constant_attributes =                            \
                static_cast<v8::PropertyAttribute>(v8::ReadOnly |              \
                        v8::DontDelete);                                       \
        (target)->ForceSet(constant_name, constant_value, constant_attributes);\
    } while(0)

#define EXPORT_PROTOTYPE_METHOD NODE_SET_PROTOTYPE_METHOD

#define EXPORT_PROTOTYPE_GET(tpl, name, fn)                                    \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (fn))

#define EXPORT_PROTOTYPE_GETSET(tpl, name, get, set)                           \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (get), (set))


class TTY : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports,
            v8::Handle<v8::Object> target);
    void HandleError(const char *err);
    void HandleResize();
    void HandleKey(int ctrl, const char *c, int code);
    void HandleMouse(int button, int x, int y, int action, int ctrl);
private:
    explicit TTY();
    ~TTY();

    // input
    static NAN_METHOD(New);
    static NAN_METHOD(Start);
    static NAN_METHOD(Stop);
    static NAN_GETTER(IsRunning);

    static NAN_METHOD(On);
    static NAN_METHOD(RemoveListener);

    // output
    static NAN_GETTER(GetWidth);
    static NAN_GETTER(GetHeight);
    static NAN_GETTER(GetMode);
    static NAN_GETTER(GetColors);
    static NAN_GETTER(GetX);
    static NAN_SETTER(SetX);
    static NAN_GETTER(GetY);
    static NAN_SETTER(SetY);
    static NAN_GETTER(GetTitle);
    static NAN_SETTER(SetTitle);
    static NAN_GETTER(GetCursor);
    static NAN_SETTER(SetCursor);
    static NAN_GETTER(GetFPS);
    static NAN_SETTER(SetFPS);
    static NAN_METHOD(Goto);
    static NAN_METHOD(Write);
    static NAN_METHOD(Beep);
    static NAN_METHOD(Clear);
    static NAN_METHOD(Prepare);
    static NAN_METHOD(Color);
    static NAN_METHOD(RequestAnimationFrame);
    static NAN_METHOD(CancelAnimationFrame);

    static v8::Persistent<v8::Function> constructor;

    ttyutil_eventlistener **GetRoot(const char *type);

    bool running_;
    TTYInputWorker *worker_;
    // mouse event listeners
    struct ttyutil_eventlistener *mouseroot[MOUSE_ACTION_LENGTH];
    // key event listeners
    struct ttyutil_eventlistener *keyroot;
    // resize event listeners
    struct ttyutil_eventlistener *resizeroot;
    // error event listeners
    struct ttyutil_eventlistener *errorroot;
};

#endif //TTYUTIL_TTY_H_
