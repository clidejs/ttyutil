var events = require("events");
var Const = require("./const");
var signal = require("./signal");
var utils = require("./utils");
var DIRECT = [
    "running",
    "width",
    "height",
    "resize",
    "mode",
    "colors",
    "x",
    "y",
    "goto",
    "beep",
    "clear",
    "start",
    "stop"
];
var NOP = function() {};

module.exports = function(ttyu_js_c) {
    var emitter = new events.EventEmitter();
    var fg, bg, bold, italic, underline, reverse;
    var ttyu = new ttyu_js_c(function(ev) {
        console.log("emit " + JSON.stringify(ev));
        emitter.emit(ev.type, ev);
    });
    emitter.setMaxListeners(100);

    var ttyutil = {
        on: function(ev, listener) {
            if(ev === ttyutil.EVENT.SIGNAL) {
                signal.on(ttyu, ttyu, listener);
            } else if(ev in Const.Event && listener instanceof Function) {
                emitter.on(ev, listener);
            }
            return ttyutil;
        },
        removeListener: off,
        off: off,
        emit: function(ev) {
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
        reset: reset,
        fg: function(r,g,b) {
            if(arguments.length == 3) {
                fg = utils.rgb(r,g,b);
            } else if(arguments.length == 1) {
                if(typeof r === "number") {
                    fg = r;
                } else {
                    fg = utils.hex(r);
                }
            } else {
                fg = -1;
            }
            return ttyutil;
        },
        bg: function(r,g,b) {
            if(arguments.length == 3) {
                bg = utils.rgb(r,g,b);
            } else if(arguments.length == 1) {
                if(typeof r === "number") {
                    bg = r;
                } else {
                    bg = utils.hex(r);
                }
            } else {
                bg = -1;
            }
            return ttyutil;
        },
        bold: function(flag) {
            if(typeof flag === "boolean") {
                bold = flag;
            } else if(typeof flag === "undefined") {
                bold = true;
            }
            return ttyutil;
        },
        italic: function(flag) {
            if(typeof flag === "boolean") {
                italic = flag;
            } else if(typeof flag === "undefined") {
                italic = true;
            }
            return ttyutil;
        },
        underline: function(flag) {
            if(typeof flag === "boolean") {
                underline = flag;
            } else if(typeof flag === "undefined") {
                underline = true;
            }
            return ttyutil;
        },
        reverse: function(flag) {
            if(typeof flag === "boolean") {
                reverse = flag;
            } else if(typeof flag === "undefined") {
                reverse = true;
            }
            return ttyutil;
        },
        write: function(chunk) {
            format = "%s";
            if(fg != -1)
                format = "\x1b[38;5;" + fg + "m" + format + "\x1b[39m";
            if(bg != -1)
                format = "\x1b[48;5;" + bg + "m" + format + "\x1b[49m";
            if(bold)
                format = "\x1b[1m" + format;
            else
                format = "\x1b[22m" + format;
            if(italic)
                format = "\x1b[3m" + format;
            else
                format = "\x1b[23m" + format;
            if(underline)
                format = "\x1b[4m" + format;
            else
                format = "\x1b[24m" + format;
            ttyu.write(reverse ? rev(chunk) : chunk, format);
            return ttyutil;
        }
    };

    for(var i = 0; i < DIRECT.length; ++i) {
        applyDirect(DIRECT[i]);
    }

    function rev(str) {
        for(var i = str.length - 1, o = ""; i >= 0; o += str[i--]);
        return o;
    }

    function applyDirect(d) {
        if(d in ttyu) {
            ttyutil[d] = function() {
                var ret = ttyu[d].apply(ttyu, arguments);
                if(ret)
                    return ret;
                return ttyutil;
            }
        } else if("set" + d in ttyu) {
            Object.defineProperty(ttyutil, d, {
                __proto__: null,
                get: ttyu["get" + d],
                set: ttyu["set" + d],
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

    function reset() {
        fg = -1;
        bg = -1;
        bold = false;
        italic = false;
        underline = false;
        reverse = false;
        return ttyutil;
    }

    function off(ev, listener) {
        if(ev === ttyutil.EVENT.SIGNAL) {
            signal.off(ttyu, listener);
        } else if(ev in Const.Event && listener instanceof Function) {
            emitter.removeListener(ev, listener);
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

    return reset();  // initial reset
};
