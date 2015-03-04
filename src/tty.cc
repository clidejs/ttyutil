#include "headers/tty.h"

v8::Persistent<v8::Function> TTYUtil::constructor;

void ttyutil_eventlistener_add(ttyutil_eventlistener *root,
        ttyutil_eventlistener *ptr) {
    if(root->next == NULL) {
        root->next = ptr;
        ptr->next = NULL;
    } else {
        ttyutil_eventlistener_add(root->next, ptr);
    }
}

ttyutil_eventlistener *ttyutil_eventlistener_new(NanCallback *cb) {
    ttyutil_eventlistener *ev =
            (ttyutil_eventlistener *)malloc(sizeof(ttyutil_eventlistener));
    ev->listener = cb;
    ev->next = NULL;
    return ev;
}

bool ttyutil_eventlistener_delete(ttyutil_eventlistener *root,
        NanCallback *cb) {
    if(root->next != NULL) {
        if(root->next->listener == cb) {
            ttyutil_eventlistener *old = root->next;
            root->next = old->next;
            delete old;
            return true;
        } else {
            return ttyutil_eventlistener_delete(root->next, cb);
        }
    }
    return false;
}

void ttyutil_eventlistener_exec(ttyutil_eventlistener *root, const int len,
        v8::Local<v8::Value> argv[]) {
    if(root->next != NULL) {
        if(!(root->next->listener->IsEmpty())) {
            root->next->listener->Call(len, argv);
        }
        if(root->next->next != NULL) {
            ttyutil_eventlistener_exec(root->next, len, argv);
        }
    }
}

TTYUtil::TTYUtil() : running_(false) {
    data = (TTYUTIL_DATA *)malloc(sizeof(TTYUTIL_DATA));
    data->init();

    keyroot = ttyutil_eventlistener_new(NULL);
    resizeroot = ttyutil_eventlistener_new(NULL);
    errorroot = ttyutil_eventlistener_new(NULL);
    for(int i = 0; i < MOUSE_ACTION_LENGTH; ++i) {
        mouseroot[i] = ttyutil_eventlistener_new(NULL);
    }
}
TTYUtil::~TTYUtil() {
    data->destroy();
    delete data;
    delete worker_;
}

ttyutil_eventlistener *TTYUtil::GetRoot(const char *type) {
    if(strcmp(type, "error") == 0) {
        return errorroot;
    } else if(strcmp(type, "key") == 0) {
        return keyroot;
    } else if(strcmp(type, "resize") == 0) {
        return resizeroot;
    } else if(strcmp(type, "mouseup") == 0) {
        return mouseroot[0];
    } else if(strcmp(type, "mousedown") == 0) {
        return mouseroot[1];
    } else if(strcmp(type, "mousemove") == 0) {
        return mouseroot[2];
    } else if(strcmp(type, "mousewheel") == 0) {
        return mouseroot[3];
    } else if(strcmp(type, "mousehwheel") == 0) {
        return mouseroot[4];
    }
    return NULL;
}

NAN_METHOD(TTYUtil::New) {
    NanScope();
    TTYUtil *obj = new TTYUtil();
    obj->Wrap(args.This());
    NanReturnThis();
}

NAN_METHOD(TTYUtil::Start) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    if(!obj->running_) {
        obj->running_ = true;

        TTYInputWorker *w = new TTYInputWorker(obj);
        NanAsyncQueueWorker(w);
        obj->worker_ = w;
    }
    NanReturnThis();
}

NAN_METHOD(TTYUtil::Stop) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    if(obj->running_) {
        obj->running_ = false;
        obj->worker_->Destroy();
        delete obj->worker_;
    }
    NanReturnThis();
}

NAN_GETTER(TTYUtil::IsRunning) {
    NanScope();
    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    NanReturnValue(NanNew<v8::Boolean>(obj->running_));
}

NAN_METHOD(TTYUtil::On) {
    NanScope();

    if(!args[0]->IsString()) {
        NanThrowTypeError("first argument must be string");
    }
    if(!args[1]->IsFunction()) {
        NanThrowTypeError("second argument must be a function");
    }

    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
    NanCallback *cb = new NanCallback(v8::Local<v8::Function>::Cast(args[1]));

    ttyutil_eventlistener_add(obj->GetRoot(ch->operator*()),
            ttyutil_eventlistener_new(cb));
    NanReturnThis();
}

NAN_METHOD(TTYUtil::RemoveListener) {
    NanScope();

    if(!args[0]->IsString()) {
        NanThrowTypeError("first argument must be string");
    }
    if(!args[1]->IsFunction()) {
        NanThrowTypeError("second argument must be a function");
    }

    TTYUtil *obj = ObjectWrap::Unwrap<TTYUtil>(args.This());
    v8::String::Utf8Value *ch = new v8::String::Utf8Value(args[0]->ToString());
    NanCallback *cb = new NanCallback(v8::Local<v8::Function>::Cast(args[1]));

    NanReturnValue(NanNew<v8::Boolean>(
            ttyutil_eventlistener_delete(obj->GetRoot(ch->operator*()), cb)));
}

void TTYInputWorker::HandleProgressCallback(ttyutil_event *event) {
    if(event->type == EVENT_MOUSE) {
        obj_->HandleMouse(event->mouse->button, event->mouse->x,
                event->mouse->y, event->mouse->action, event->mouse->ctrl);
    } else if(event->type == EVENT_KEY) {
        obj_->HandleKey(event->key->ctrl, &event->key->c, event->key->code,
                event->key->which);
    } else if(event->type == EVENT_RESIZE) {
        obj_->HandleResize();
    } else {
        obj_->HandleError("unhandled event occured");
    }
};

void TTYUtil::HandleError(const char *err) {
    v8::Local<v8::Value> argv[] = { NanError(err) };
    ttyutil_eventlistener_exec(errorroot, 1, argv);
}

void TTYUtil::HandleResize() {
    v8::Local<v8::Value> argv[0];
    ttyutil_eventlistener_exec(resizeroot, 0, argv);
}

void TTYUtil::HandleKey(int ctrl, const char *c, int code, int which) {
    v8::Local<v8::Object> obj = NanNew<v8::Object>();
    obj->Set(NanNew<v8::String>("ctrl"), NanNew<v8::Integer>(ctrl));
    obj->Set(NanNew<v8::String>("char"), NanNew<v8::String>(c));
    obj->Set(NanNew<v8::String>("code"), NanNew<v8::Integer>(code));
    obj->Set(NanNew<v8::String>("which"), NanNew<v8::Integer>(which));

    v8::Local<v8::Value> argv[] = { obj };
    ttyutil_eventlistener_exec(keyroot, 1, argv);
}

void TTYUtil::HandleMouse(int button, int x, int y, int action, int ctrl) {
    v8::Local<v8::Object> obj = NanNew<v8::Object>();
    obj->Set(NanNew<v8::String>("button"), NanNew<v8::Integer>(button));
    obj->Set(NanNew<v8::String>("x"), NanNew<v8::Integer>(x));
    obj->Set(NanNew<v8::String>("y"), NanNew<v8::Integer>(y));
    obj->Set(NanNew<v8::String>("ctrl"), NanNew<v8::Integer>(ctrl));

    v8::Local<v8::Value> argv[] = { obj };
    ttyutil_eventlistener *root = mouseroot[0]; // MOUSE_ACTION_UP

    if(action == MOUSE_ACTION_DOWN) {
        root = mouseroot[1];
    } else if(action == MOUSE_ACTION_MOVE) {
        root = mouseroot[2];
    } else if(action == MOUSE_ACTION_WHEEL) {
        root = mouseroot[3];
    } else if(action == MOUSE_ACTION_HWHEEL) {
        root = mouseroot[4];
    } else { root = errorroot; /* bad one */ }

    ttyutil_eventlistener_exec(root, 1, argv);
}

void TTYUtil::Init(v8::Handle<v8::Object> target) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
    tpl->SetClassName(NanNew<v8::String>("TTYUtil"));
    tpl->InstanceTemplate()->SetInternalFieldCount(5);
    // Prototype
    EXPORT_PROTOTYPE_METHOD(tpl, "start", Start);
    EXPORT_PROTOTYPE_METHOD(tpl, "stop", Stop);
    EXPORT_PROTOTYPE_GET(tpl, "running", IsRunning);
    EXPORT_PROTOTYPE_METHOD(tpl, "on", On);
    EXPORT_PROTOTYPE_METHOD(tpl, "removeListener", RemoveListener);
    EXPORT_PROTOTYPE_GET(tpl, "width", GetWidth);
    EXPORT_PROTOTYPE_GET(tpl, "height", GetHeight);
    EXPORT_PROTOTYPE_GET(tpl, "mode", GetMode);
    EXPORT_PROTOTYPE_GET(tpl, "colors", GetColors);
    EXPORT_PROTOTYPE_GETSET(tpl, "x", GetX, SetX);
    EXPORT_PROTOTYPE_GETSET(tpl, "y", GetY, SetY);
    EXPORT_PROTOTYPE_GETSET(tpl, "fps", GetFPS, SetFPS);

    EXPORT_PROTOTYPE_METHOD(tpl, "goto", Goto);
/* TODO:
    EXPORT_PROTOTYPE_METHOD(tpl, "write", Write);
    EXPORT_PROTOTYPE_METHOD(tpl, "beep", Beep);
    EXPORT_PROTOTYPE_METHOD(tpl, "clear", Clear);
    EXPORT_PROTOTYPE_METHOD(tpl, "prepare", Prepare);
    EXPORT_PROTOTYPE_METHOD(tpl, "color", Color);

    EXPORT_PROTOTYPE_GETSET(tpl, "cursor", GetCursor, SetCursor);
    EXPORT_PROTOTYPE_GETSET(tpl, "title", GetTitle, SetTitle);
    EXPORT_PROTOTYPE_METHOD(tpl, "requestAnimationFrame",
            RequestAnimationFrame);
    EXPORT_PROTOTYPE_METHOD(tpl, "cancelAnimationFrame", CancelAnimationFrame);
*/
    // export TTYUtil
    constructor.Reset(v8::Isolate::GetCurrent(), tpl->GetFunction());
    tpl->ReadOnlyPrototype();
    target->Set(NanNew<v8::String>("TTYUtil"), tpl->GetFunction());

    // export control key types
    v8::Handle<v8::Object> ctrl = NanNew<v8::Object>();
    EXPORT_CONSTANT(ctrl, "NULL", CTRL_NULL);
    EXPORT_CONSTANT(ctrl, "ALT", CTRL_ALT);
    EXPORT_CONSTANT(ctrl, "CTRL", CTRL_CTRL);
    EXPORT_CONSTANT(ctrl, "SHIFT", CTRL_SHIFT);
    EXPORT_CONSTANT(ctrl, "ENHANCED", CTRL_ENHANCED);
    EXPORT_CONSTANT(ctrl, "CMD", CTRL_CMD);
    EXPORT_CONSTANT(ctrl, "NUMLOCK", CTRL_NUMLOCK);
    EXPORT_CONSTANT(ctrl, "SCROLLLOCK", CTRL_SCROLLLOCK);
    EXPORT_CONSTANT(ctrl, "CAPSLOCK", CTRL_CAPSLOCK);
    target->Set(NanNew<v8::String>("CTRL"), ctrl);

    // export mouse button types
    v8::Handle<v8::Object> mouse = NanNew<v8::Object>();
    EXPORT_CONSTANT(mouse, "LEFT", MOUSE_BUTTON_LEFT);
    EXPORT_CONSTANT(mouse, "LEFT2", MOUSE_BUTTON_LEFT2);
    EXPORT_CONSTANT(mouse, "LEFT3", MOUSE_BUTTON_LEFT3);
    EXPORT_CONSTANT(mouse, "LEFT4", MOUSE_BUTTON_LEFT4);
    EXPORT_CONSTANT(mouse, "RIGHT", MOUSE_BUTTON_RIGHT);
    target->Set(NanNew<v8::String>("MOUSE"), mouse);

    // export mouse button types
    v8::Handle<v8::Object> mode = NanNew<v8::Object>();
    EXPORT_CONSTANT(mode, "CMD", MODE_CMD);
    EXPORT_CONSTANT(mode, "VT102", MODE_VT102);
    EXPORT_CONSTANT(mode, "VT100", MODE_VT100);
    target->Set(NanNew<v8::String>("MODE"), mode);
}

NODE_MODULE(tty, TTYUtil::Init);
