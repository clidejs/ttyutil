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
    ttyu.TTYUtil.prototype.emit = function(ev, arg1, arg2, arg3, arg4) {
        if(ev === ttyu.TTYUtil.EVENT.SIGNAL) {
            signal.emit(arg1);
        } else if(ev in Const.Event && ev !== ttyu.TTYUtil.EVENT.ERROR) {
            this.__emit__(Const.Event[ev], arg1, arg2, arg3, arg4);
        }
        return this;
    };
    ttyu.TTYUtil.prototype.start = function() {
        this.__start__();
    };

    ttyu.TTYUtil.EVENT = {
        ERROR: "error",
        SIGNAL: "signal",
        KEY: "key",
        RESIZE: "resize",
        MOUSEDOWN: "mousedown",
        MOUSEUP: "mouseup",
        MOUSEMOVE: "mousemove",
        MOUSEWHEEL: "mousewheel",
        MOUSEHWHEEL: "mousehwheel"
    };
    ttyu.TTYUtil.SIGNAL = {
        SIGINT: "SIGINT",
        SIGTERM: "SIGTERM",
        SIGPIPE: "SIGPIPE",
        SIGHUP: "SIGHUP"
    };
    ttyu.TTYUtil.MOUSE = Const.Mouse;
    ttyu.TTYUtil.WHICH = Const.Which;
    ttyu.TTYUtil.CTRL = Const.Ctrl;
    ttyu.TTYUtil.MODE = Const.Mode;

    return ttyu.TTYUtil;
};
