var Const = require("./const");
var signal = require("./signal");

module.exports = function(ttyu) {
    ttyu.TTYUtil.prototype.on = function(ev, listener) {
        if(ev === ttyu.TTYUtil.EVENT.SIGNAL) {
            signal.on(ttyu, this, listener);
        } else if(ev in Const.Event && listener instanceof Function) {
            this.__on__(Const.Event[ev], listener);
        }
        return this;
    };
    ttyu.TTYUtil.prototype.off =
            ttyu.TTYUtil.prototype.removeListener = function(ev, listener) {
        if(ev === ttyu.TTYUtil.EVENT.SIGNAL) {
            signal.off(this, listener);
        } else if(ev in Const.Event && listener instanceof Function) {
            this.__off__(Const.Event[ev], listener);
        }
        return this;
    };
    ttyu.TTYUtil.prototype.emit = function(ev) {
        switch(ev.type) {
            case ttyu.TTYUtil.EVENT.SIGNAL:
                signal.emit(ev.signal);
                break;
            case ttyu.TTYUtil.EVENT.KEY:
                this.__emit__(Const.Event[ev.type], ev.which, ev.ctrl);
                break;
            case ttyu.TTYUtil.EVENT.MOUSEDOWN:
            case ttyu.TTYUtil.EVENT.MOUSEUP:
            case ttyu.TTYUtil.EVENT.MOUSEMOVE:
            case ttyu.TTYUtil.EVENT.MOUSEWHEEL:
            case ttyu.TTYUtil.EVENT.MOUSEHWHEEL:
                this.__emit__(Const.Event[ev.type], ev.button, ev.x, ev.y,
                    ev.ctrl);
                break;
            default:
                // ERROR, RESIZE
                break;
        }
    };

    ttyu.TTYUtil.SIGNAL = {
        SIGINT: "SIGINT",
        SIGTERM: "SIGTERM",
        SIGPIPE: "SIGPIPE",
        SIGHUP: "SIGHUP"
    };
    ttyu.TTYUtil.EVENT = Const.EventString;
    ttyu.TTYUtil.MOUSE = Const.Mouse;
    ttyu.TTYUtil.WHICH = Const.Which;
    ttyu.TTYUtil.CTRL = Const.Ctrl;
    ttyu.TTYUtil.MODE = Const.Mode;

    // event constructors
    ttyu.TTYUtil.KeyEvent = function(which, ctrl) {
        return {
            type: ttyu.TTYUtil.EVENT.KEY,
            which: which,
            ctrl: ctrl
        };
    };
    ttyu.TTYUtil.MouseEvent = function(type, button, x, y, ctrl) {
        return {
            type: type,
            button: button,
            x: x,
            y: y,
            ctrl: ctrl
        };
    };
    ttyu.TTYUtil.SignalEvent = function(signal) {
        return {
            type: ttyu.TTYUtil.EVENT.SIGNAL,
            signal: signal
        };
    };

    return ttyu.TTYUtil;
};
