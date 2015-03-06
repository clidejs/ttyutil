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

function emitter(e) {
    return function(obj) {
        console.log(e, obj, "\r");
    };
}
