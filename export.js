var Const = require("./const");
var signal = require("./signal");

module.exports = function(ttyu) {
    ttyu.on = function(ev, listener) {
        /*if(ev === ttyu.EVENT.SIGNAL) {
            signal.on(ttyu, ttyu, listener);
        } else*/ if(ev in Const.Event && listener instanceof Function) {
            ttyu.__on__(Const.Event[ev], listener);
        }
        return ttyu;
    };
    ttyu.off = ttyu.removeListener = function(ev, listener) {
        /*if(ev === ttyu.EVENT.SIGNAL) {
            signal.off(ttyu, listener);
        } else*/ if(ev in Const.Event && listener instanceof Function) {
            ttyu.__off__(Const.Event[ev], listener);
        }
        return ttyu;
    };
    ttyu.emit = function(ev) {
        switch(ev.type) {
            /*case ttyu.EVENT.SIGNAL:
                signal.emit(ev.signal);
                break;*/
            case ttyu.EVENT.KEY:
                ttyu.__emit__(Const.Event[ev.type], ev.which, ev.ctrl);
                break;
            case ttyu.EVENT.MOUSEDOWN:
            case ttyu.EVENT.MOUSEUP:
            case ttyu.EVENT.MOUSEMOVE:
            case ttyu.EVENT.MOUSEWHEEL:
            case ttyu.EVENT.MOUSEHWHEEL:
                ttyu.__emit__(Const.Event[ev.type], ev.button, ev.x, ev.y,
                    ev.ctrl);
                break;
            default:
                // ERROR, RESIZE
                break;
        }
    };

    ttyu.SIGNAL = {
        SIGINT: "SIGINT",
        SIGTERM: "SIGTERM",
        SIGPIPE: "SIGPIPE",
        SIGHUP: "SIGHUP"
    };
    ttyu.EVENT = Const.EventString;
    ttyu.MOUSE = Const.Mouse;
    ttyu.WHICH = Const.Which;
    ttyu.CTRL = Const.Ctrl;
    ttyu.MODE = Const.Mode;

    // event constructors
    ttyu.KeyEvent = function(which, ctrl) {
        return {
            type: ttyu.EVENT.KEY,
            which: which,
            ctrl: ctrl
        };
    };
    ttyu.MouseEvent = function(type, button, x, y, ctrl) {
        return {
            type: type,
            button: button,
            x: x,
            y: y,
            ctrl: ctrl
        };
    };
    ttyu.SignalEvent = function(signal) {
        return {
            type: ttyu.EVENT.SIGNAL,
            signal: signal
        };
    };

    return ttyu;
};
