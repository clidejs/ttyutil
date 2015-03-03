var events = require("events");
var TTY = require("./build/Release/tty");

var TTYUtil = module.exports = function(/* TODO options */) {
    events.EventEmitter.call(this);
    this.__tty__ = new TTY.TTY(this.__handle__.bind(this));
};

TTYUtil.prototype = events.EventEmitter.prototype;
Object.defineProperties(TTYUtil.prototype, {
//private:
    __handle__: {
        value: function(err, type, arg2, arg3, arg4, arg5, arg6) {
            if(err) {
                this.emit("error", new Error(err));
                return;
            }
            switch(type) {
                case TTY.EVENT.MOUSE:
                    var act;
                    switch(arg5 /* action */) {
                        case TTY.MOUSE.ACTION_PRESSED:
                            act = "mousepress";
                            break;
                        case TTY.MOUSE.ACTION_CLICKED:
                            act = "mouseclick";
                            break;
                        case TTY.MOUSE.ACTION_DBLCLICKED:
                            act = "doubleclick";
                            break;
                        case TTY.MOUSE.ACTION_TRICLICKED:
                            act = "trippleclicked";
                            break;
                        case TTY.MOUSE.ACTION_MOVED:
                            act = "mousemove";
                            break;
                        case TTY.MOUSE.ACTION_WHEELED:
                            act = "mousewheel";
                            break;
                        case TTY.MOUSE.ACTION_HWHEELED:
                            act = "mousehwheel";
                            break;
                        default:
                            act = "mouserelease";
                            break;
                    }
                    this.emit(act, {
                        button: arg2,
                        x: arg3,
                        y: arg4,
                        action: arg5,
                        ctrl: arg6
                    });
                    break;
                case TTY.EVENT.KEY:
                    this.emit("key", {
                        ctrl: arg2,
                        c: arg3,
                        code: arg4
                    });
                    break;
                case TTY.EVENT.RESIZE:
                    this.emit("resize");
                    break;
                default: //EVENT.UNDEF
                    this.emit("error", new Error("undefined event"));
                    break;
            }
        },
        writeable: false,
        configurable: false,
        enumerable: false
    },

//public:
    running: {
        get: function() {
            return this.__tty__.running;
        },
        set: function() {},
        configurable: false,
        enumerable: true
    },

    start: {
        value: function() {
            this.__tty__.start();
            return this;
        },
        writable: false,
        configurable: false,
        enumerable: true
    },

    stop: {
        value: function() {
            this.__tty__.stop();
            return this;
        },
        writable: false,
        configurable: false,
        enumerable: true
    }
});

// expose constants
var keys = Object.keys(TTY);
for(var i = 0; i < keys.length; ++i) {
    if(keys[i] !== "TTY") {
        TTYUtil[keys[i]] = TTY[keys[i]];
    }
}
