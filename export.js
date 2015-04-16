var Const = require("./const");
var signal = require("./signal");

module.exports = function(ttyu_js_c) {
    var ttyu = new ttyu_js_c();

    var ttyutil = {
        start: function() {
            ttyu.start();
        },
        stop: function() {
            ttyu.stop();
        },
        write: function() {
            ttyu.write.apply(ttyu, arguments);
        },
        on: function(ev, listener) {
            if(ev === ttyutil.EVENT.SIGNAL) {
                signal.on(ttyu, ttyu, listener);
            } else if(ev in Const.Event && listener instanceof Function) {
                ttyu.on(Const.Event[ev], listener);
            }
            return ttyutil;
        },
        removeListener: off,
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
        },
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
