var Const = require("./const");

module.exports = function(ttyu) {
    ttyu.TTYUtil.prototype.on = function(ev, listener) {
        if(ev in Const.Event && listener instanceof Function) {
            this.__on__(Const.Event[ev], listener);
        }
        return this;
    };
    ttyu.TTYUtil.prototype.off =
            ttyu.TTYUtil.prototype.removeListener = function(ev, listener) {
        if(ev in Const.Event && listener instanceof Function) {
            this.__off__(Const.Event[ev], listener);
        }
        return this;
    };

    ttyu.TTYUtil.EVENT = {
        ERROR: "error",
        KEY: "key",
        RESIZE: "resize",
        MOUSEDOWN: "mousedown",
        MOUSEUP: "mouseup",
        MOUSEMOVE: "mousemove",
        MOUSEWHEEL: "mousewheel",
        MOUSEHWHEEL: "mousehwheel"
    };
    ttyu.TTYUtil.MOUSE = Const.Mouse;
    ttyu.TTYUtil.WHICH = Const.Which;
    ttyu.TTYUtil.CTRL = Const.Ctrl;
    ttyu.TTYUtil.MODE = Const.Mode;

    return ttyu.TTYUtil;
};
