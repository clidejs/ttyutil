#include "headers/tty.h"

v8::Persistent<v8::Function> TTY::constructor;

void ttyutil_eventlistener_add(ttyutil_eventlistener **root,
        ttyutil_eventlistener *ptr) {
    if(*root == NULL) {
        *root = ptr;
    } else {
        ttyutil_eventlistener_add(&((*root)->next), ptr);
    }
}

bool ttyutil_eventlistener_delete(ttyutil_eventlistener **root,
        NanCallback *cb) {
    if(*root != NULL) {
        if((*root)->listener == cb) {
            ttyutil_eventlistener *old = *root;
            *root = (*root)->next;
            delete old;
            return true;
        } else {
            return ttyutil_eventlistener_delete(&((*root)->next), cb);
        }
    }
    return false;
}

void ttyutil_eventlistener_exec(ttyutil_eventlistener **root, const int len,
        v8::Handle<v8::Value> argv[]) {
    if(*root != NULL) {
        if(!(*root)->listener->IsEmpty()) {
            (*root)->listener->Call(len, argv);
        }
        if((*root)->next != NULL) {
            ttyutil_eventlistener_exec(&((*root)->next), len, argv);
        }
    }
}

TTY::TTY() : running_(false) {
    keyroot = NULL;
    resizeroot = NULL;

    for(int i = 0; i < MOUSE_ACTION_LENGTH; ++i) {
        mouseroot[i] = NULL;
    }
}
TTY::~TTY() {
    delete worker_;
}

ttyutil_eventlistener **TTY::GetRoot(const char *type) {
    if(strcmp(type, "error")) {
        return &errorroot;
    } else if(strcmp(type, "key")) {
        return &keyroot;
    } else if(strcmp(type, "resize")) {
        return &resizeroot;
    } else if(strcmp(type, "mouserelease")) {
        return &(mouseroot[0]);
    } else if(strcmp(type, "mousepress")) {
        return &(mouseroot[1]);
    } else if(strcmp(type, "mouseclick")) {
        return &(mouseroot[2]);
    } else if(strcmp(type, "doubleclick")) {
        return &(mouseroot[3]);
    } else if(strcmp(type, "tripleclick")) {
        return &(mouseroot[4]);
    } else if(strcmp(type, "mousemove")) {
        return &(mouseroot[5]);
    } else if(strcmp(type, "mousewheel")) {
        return &(mouseroot[6]);
    } else if(strcmp(type, "mousehwheel")) {
        return &(mouseroot[7]);
    }
    return NULL;
}

NAN_METHOD(TTY::New) {
    NanScope();
    TTY *obj = new TTY();
    obj->Wrap(args.This());
    NanReturnThis();
}

NAN_METHOD(TTY::Start) {
    NanScope();
    TTY *obj = ObjectWrap::Unwrap<TTY>(args.This());
    if(!obj->running_) {
        obj->running_ = true;

        TTYInputWorker *w = new TTYInputWorker(obj);
        NanAsyncQueueWorker(w);
        obj->worker_ = w;
    }
    NanReturnThis();
}

NAN_METHOD(TTY::Stop) {
    NanScope();
    TTY *obj = ObjectWrap::Unwrap<TTY>(args.This());
    if(obj->running_) {
        obj->running_ = false;
        obj->worker_->Destroy();
        delete obj->worker_;
    }
    NanReturnThis();
}

NAN_GETTER(TTY::IsRunning) {
    NanScope();
    TTY *obj = ObjectWrap::Unwrap<TTY>(args.This());
    NanReturnValue(NanNew<v8::Boolean>(obj->running_));
}

NAN_METHOD(TTY::On) {
    NanScope();

    if(!args[0]->IsString()) {
        NanThrowTypeError("first argument must be string");
    }
    if(!args[1]->IsFunction()) {
        NanThrowTypeError("second argument must be a function");
    }

    TTY *obj = ObjectWrap::Unwrap<TTY>(args.This());
    const char *type = *v8::String::Utf8Value(args[0]->ToString());
    NanCallback *cb = new NanCallback(v8::Local<v8::Function>::Cast(args[1]));
    ttyutil_eventlistener *ev =
            (ttyutil_eventlistener *)malloc(sizeof(ttyutil_eventlistener));
    ev->next = NULL;
    ev->listener = cb;

    ttyutil_eventlistener_add(obj->GetRoot(type), ev);
    NanReturnThis();
}

NAN_METHOD(TTY::RemoveListener) {
    NanScope();

    if(!args[0]->IsString()) {
        NanThrowTypeError("first argument must be string");
    }
    if(!args[1]->IsFunction()) {
        NanThrowTypeError("second argument must be a function");
    }

    TTY *obj = ObjectWrap::Unwrap<TTY>(args.This());
    const char *type = *v8::String::Utf8Value(args[0]->ToString());
    NanCallback *cb = new NanCallback(v8::Local<v8::Function>::Cast(args[1]));

    NanReturnValue(NanNew<Boolean>(
            ttyutil_eventlistener_delete(obj->GetRoot(type), cb)));
}

void TTYInputWorker::HandleProgressCallback(ttyutil_event *event) {
    if(event->type == EVENT_MOUSE) {
        obj_->HandleMouse(event->mouse->button, event->mouse->x,
                event->mouse->y, event->mouse->action, event->mouse->ctrl);
    } else if(event->type == EVENT_KEY) {
        obj_->HandleKey(event->key->ctrl, &event->key->c, event->key->code);
    } else if(event->type == EVENT_RESIZE) {
        obj_->HandleResize();
    } else {
        obj_->HandleError("unhandled event occured");
    }
};

void TTY::HandleError(const char *err) {
    v8::Handle<v8::Value> argv[] = {
        NanError(err)
    };
    ttyutil_eventlistener_exec(&errorroot, 1, argv);
}

void TTY::HandleResize() {
    v8::Handle<v8::Value> argv[] = {
        NanNull()
    };
    ttyutil_eventlistener_exec(&resizeroot, 1, argv);
}

void TTY::HandleKey(int ctrl, const char *c, int code) {
    v8::Handle<v8::Object> obj = NanNew<v8::Object>();
    obj->Set(NanNew<v8::String>("ctrl"), NanNew<v8::Integer>(ctrl));
    obj->Set(NanNew<v8::String>("char"), NanNew<v8::String>(c));
    obj->Set(NanNew<v8::String>("code"), NanNew<v8::Integer>(code));

    v8::Handle<v8::Value> argv[] = {
        NanNull(),
        obj
    };
    ttyutil_eventlistener_exec(&keyroot, 2, argv);
}

void TTY::HandleMouse(int button, int x, int y, int action, int ctrl) {
    v8::Handle<v8::Object> obj = NanNew<v8::Object>();
    obj->Set(NanNew<v8::String>("button"), NanNew<v8::Integer>(button));
    obj->Set(NanNew<v8::String>("x"), NanNew<v8::Integer>(x));
    obj->Set(NanNew<v8::String>("y"), NanNew<v8::Integer>(y));
    obj->Set(NanNew<v8::String>("action"), NanNew<v8::Integer>(action));
    obj->Set(NanNew<v8::String>("ctrl"), NanNew<v8::Integer>(ctrl));

    v8::Handle<v8::Value> argv[] = {
        NanNull(),
        obj
    };

    ttyutil_eventlistener *root = mouseroot[0]; // MOUSE_ACTION_RELEASED

    if(action == MOUSE_ACTION_PRESSED) {
        root = mouseroot[1];
    } else if(action == MOUSE_ACTION_CLICKED) {
        root = mouseroot[2];
    } else if(action == MOUSE_ACTION_DBLCLICKED) {
        root = mouseroot[3];
    } else if(action == MOUSE_ACTION_TRICLICKED) {
        root = mouseroot[4];
    } else if(action == MOUSE_ACTION_MOVED) {
        root = mouseroot[5];
    } else if(action == MOUSE_ACTION_WHEELED) {
        root = mouseroot[6];
    } else if(action == MOUSE_ACTION_HWHEELED) {
        root = mouseroot[7];
    }

    ttyutil_eventlistener_exec(&root, 2, argv);
}

void TTY::Init(v8::Handle<v8::Object> exports, v8::Handle<v8::Object> target) {
    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = NanNew<v8::FunctionTemplate>(New);
    tpl->SetClassName(NanNew<v8::String>("TTY"));
    tpl->InstanceTemplate()->SetInternalFieldCount(5);
    // Prototype
    EXPORT_PROTOTYPE_METHOD(tpl, "start", Start);
    EXPORT_PROTOTYPE_METHOD(tpl, "stop", Stop);
    EXPORT_PROTOTYPE_GET(tpl, "running", IsRunning);

    EXPORT_PROTOTYPE_METHOD(tpl, "on", On);
    EXPORT_PROTOTYPE_METHOD(tpl, "removeListener", RemoveListener);

/*    EXPORT_PROTOTYPE_GET(tpl, "width", GetWidth);
    EXPORT_PROTOTYPE_GET(tpl, "height", GetHeight);
    EXPORT_PROTOTYPE_GET(tpl, "mode", GetMode);
    EXPORT_PROTOTYPE_GET(tpl, "colors", GetColors);
    EXPORT_PROTOTYPE_GETSET(tpl, "x", GetX, SetX);
    EXPORT_PROTOTYPE_GETSET(tpl, "y", GetY, SetY);
    EXPORT_PROTOTYPE_GETSET(tpl, "title", GetTitle, SetTitle);
    EXPORT_PROTOTYPE_GETSET(tpl, "cursor", GetCursor, SetCursor);
    EXPORT_PROTOTYPE_GETSET(tpl, "fps", GetFPS, SetFPS);
    EXPORT_PROTOTYPE_METHOD(tpl, "goto", Goto);
    EXPORT_PROTOTYPE_METHOD(tpl, "write", Write);
    EXPORT_PROTOTYPE_METHOD(tpl, "beep", Beep);
    EXPORT_PROTOTYPE_METHOD(tpl, "clear", Clear);
    EXPORT_PROTOTYPE_METHOD(tpl, "prepare", Prepare);
    EXPORT_PROTOTYPE_METHOD(tpl, "color", Color);
    EXPORT_PROTOTYPE_METHOD(tpl, "requestAnimationFrame",
            RequestAnimationFrame);
    EXPORT_PROTOTYPE_METHOD(tpl, "cancelAnimationFrame", CancelAnimationFrame);
*/
    // export TTY
    constructor.Reset(v8::Isolate::GetCurrent(), tpl->GetFunction());
    target->Set(NanNew<v8::String>("exports"), tpl->GetFunction());

    // export event types
    v8::Handle<v8::Object> event = NanNew<v8::Object>();
    EXPORT_CONSTANT(event, "UNDEF", EVENT_UNDEF);
    EXPORT_CONSTANT(event, "MOUSE", EVENT_MOUSE);
    EXPORT_CONSTANT(event, "KEY", EVENT_KEY);
    EXPORT_CONSTANT(event, "RESIZE", EVENT_RESIZE);
    tpl->Set(NanNew<v8::String>("EVENT"), event);

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
    tpl->Set(NanNew<v8::String>("CTRL"), ctrl);

    // export mouse button types
    v8::Handle<v8::Object> mouse = NanNew<v8::Object>();
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT", MOUSE_BUTTON_LEFT);
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT2", MOUSE_BUTTON_LEFT2);
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT3", MOUSE_BUTTON_LEFT3);
    EXPORT_CONSTANT(mouse, "BUTTON_LEFT4", MOUSE_BUTTON_LEFT4);
    EXPORT_CONSTANT(mouse, "BUTTON_RIGHT", MOUSE_BUTTON_RIGHT);

    // export mouse actions
    EXPORT_CONSTANT(mouse, "ACTION_RELEASED", MOUSE_ACTION_RELEASED);
    EXPORT_CONSTANT(mouse, "ACTION_PRESSED", MOUSE_ACTION_PRESSED);
    EXPORT_CONSTANT(mouse, "ACTION_CLICKED", MOUSE_ACTION_CLICKED);
    EXPORT_CONSTANT(mouse, "ACTION_DBLCLICKED", MOUSE_ACTION_DBLCLICKED);
    EXPORT_CONSTANT(mouse, "ACTION_TRICLICKED", MOUSE_ACTION_TRICLICKED);
    EXPORT_CONSTANT(mouse, "ACTION_MOVED", MOUSE_ACTION_MOVED);
    EXPORT_CONSTANT(mouse, "ACTION_WHEELED", MOUSE_ACTION_WHEELED);
    EXPORT_CONSTANT(mouse, "ACTION_HWHEELED", MOUSE_ACTION_HWHEELED);
    tpl->Set(NanNew<v8::String>("MOUSE"), mouse);
}

NODE_MODULE(tty, TTY::Init);
