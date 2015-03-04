#ifndef TTYUTIL_TTY_H_
#define TTYUTIL_TTY_H_

#include <nan.h>

#include "ttyinputworker.h"
#include "ctrl.h"

struct ttyutil_eventlistener {
    NanCallback *listener;
    struct ttyutil_eventlistener *next;
};

void ttyutil_eventlistener_add(ttyutil_eventlistener *root,
        ttyutil_eventlistener *ptr);
ttyutil_eventlistener *ttyutil_eventlistener_new(NanCallback *cb);
bool ttyutil_eventlistener_delete(ttyutil_eventlistener *root,
        NanCallback *cb);
void ttyutil_eventlistener_exec(ttyutil_eventlistener *root, const int len,
        v8::Local<v8::Value> argv[]);

#define EXPORT_READONLY                                                        \
        static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete)

#define EXPORT_CONSTANT(target, cname, constant)                               \
    do {                                                                       \
        NanScope();                                                            \
        v8::Local<v8::String> name = NanNew<v8::String>(cname);                \
        v8::Local<v8::Number> value = NanNew<v8::Number>(constant);            \
        v8::PropertyAttribute attributes = EXPORT_READONLY;                    \
        (target)->ForceSet(name, value, attributes);                           \
    } while(0)

#define EXPORT_PROTOTYPE_METHOD NODE_SET_PROTOTYPE_METHOD

#define EXPORT_PROTOTYPE_GET(tpl, name, fn)                                    \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (fn))

#define EXPORT_PROTOTYPE_GETSET(tpl, name, get, set)                           \
    tpl->InstanceTemplate()->SetAccessor(NanNew<v8::String>(name), (get), (set))


class TTYUtil : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> target);
    void HandleError(const char *err);
    void HandleResize();
    void HandleKey(int ctrl, const char *c, int code, int which);
    void HandleMouse(int button, int x, int y, int action, int ctrl);
private:
    explicit TTYUtil();
    ~TTYUtil();
    ttyutil_eventlistener *GetRoot(const char *type);
    static NAN_METHOD(New);

    // event emitter
    static NAN_METHOD(On);
    static NAN_METHOD(RemoveListener);

    static NAN_METHOD(Start);
    static NAN_METHOD(Stop);
    static NAN_GETTER(IsRunning);
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

    // constructor reference
    static v8::Persistent<v8::Function> constructor;

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
