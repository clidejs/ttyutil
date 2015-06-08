var Const = require("./const");
var signal = require("./signal");
var DIRECT = [
    "write",
    "running",
    "width",
    "height",
    "mode",
    "x",
    "y",
    "start",
    "stop"
];
var NOP = function() {};

module.exports = function(ttyu_js_c) {
    var ttyu = new ttyu_js_c();

    var ttyutil = {
        on: function(ev, listener) {
            if(ev === ttyutil.EVENT.SIGNAL) {
                signal.on(ttyu, ttyu, listener);
            } else if(ev in Const.Event && listener instanceof Function) {
                ttyu.on(Const.Event[ev], listener);
            }
            return ttyutil;
        },
        removeListener: off,
        off: off,
        emit:function(ev) {
            switch(ev.type) {
                case ttyutil.EVENT.SIGNAL:
                    signal.emit(ev.signal);
                    break;
                case ttyutil.EVENT.KEY:
                    ttyu.emit(Const.Event[ev.type], ev.which, ev.ctrl);
                    break;
                case ttyutil.EVENT.MOUSEDOWN:
                case ttyutil.EVENT.MOUSEUP:
                case ttyutil.EVENT.MOUSEMOVE:
                case ttyutil.EVENT.MOUSEWHEEL:
                case ttyutil.EVENT.MOUSEHWHEEL:
                    ttyu.emit(Const.Event[ev.type], ev.button, ev.x, ev.y,
                        ev.ctrl);
                    break;
                default:
                    // ERROR, RESIZE
                    break;
            }
        }
    }

    for(var i = 0; i < DIRECT.length; ++i) {
        applyDirect(DIRECT[i]);
    }

    function applyDirect(d) {
        if(d in ttyu) {
            ttyutil[d] = function() {
                ttyu[d].apply(ttyu, arguments);
            }
        } else if("set" + d in ttyu) {
            Object.defineProperty(ttyutil, d, {
                __proto__: null,
                get: ttyu["set" + d],
                set: ttyu["get" + d],
                configurable: false,
                enumerable: true,
                writeable: false
            });
        } else {
            Object.defineProperty(ttyutil, d, {
                __proto__: null,
                get: ttyu["get" + d].bind(ttyu),
                set: NOP,
                configurable: false,
                enumerable: true,
                writeable: false
            });
        }
    }

    function off(ev, listener) {
        if(ev === ttyutil.EVENT.SIGNAL) {
            signal.off(ttyu, listener);
        } else if(ev in Const.Event && listener instanceof Function) {
            ttyu.off(Const.Event[ev], listener);
        }
        return ttyutil;
    };


    ttyutil.SIGNAL = {
        SIGINT: "SIGINT",
        SIGTERM: "SIGTERM",
        SIGPIPE: "SIGPIPE",
        SIGHUP: "SIGHUP"
    };
    ttyutil.EVENT = Const.EventString;
    ttyutil.MOUSE = Const.Mouse;
    ttyutil.WHICH = Const.Which;
    ttyutil.CTRL = Const.Ctrl;
    ttyutil.MODE = Const.Mode;

    // event constructors
    ttyutil.KeyEvent = function(which, ctrl) {
        return {
            type: ttyutil.EVENT.KEY,
            which: which,
            ctrl: ctrl
        };
    };
    ttyutil.MouseEvent = function(type, button, x, y, ctrl) {
        return {
            type: type,
            button: button,
            x: x,
            y: y,
            ctrl: ctrl
        };
    };
    ttyutil.SignalEvent = function(signal) {
        return {
            type: ttyutil.EVENT.SIGNAL,
            signal: signal
        };
    };

    return ttyutil;
};
