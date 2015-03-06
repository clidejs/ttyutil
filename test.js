var ttyu = require("./build/Debug/ttyu");
var Const = require("./const");

var TTYUtil = ttyu.TTYUtil;

TTYUtil.prototype.on = function(ev, listener) {
    if(ev in Const.Event && listener instanceof Function) {
        this.__on__(Const.Event[ev], listener);
    }
    return this;
};
TTYUtil.prototype.off =
        TTYUtil.prototype.removeListener = function(ev, listener) {
    if(ev in Const.Event && listener instanceof Function) {
        this.__off__(Const.Event[ev], listener);
    }
    return this;
};

// Test:

var ttyu = new TTYUtil();

for(var e in Const.Event) {
    ttyu.on(e, emitter(e));
}


ttyu.start();

ttyu.write(JSON.stringify(ttyu) + "\r\n", "#365cff", "#000");

function emitter(e) {
    return function(obj) {
        if(e === "error") {
            ttyu.write(e + ": " + JSON.stringify(obj) + "\r\n", "#F00", "#400");
        } else {
            ttyu.write(e + ": " + JSON.stringify(obj) + "\r\n", "#0F0", "#040");
        }
    };
}
