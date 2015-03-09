var ttyu = require("./build/Release/ttyu");
var Const = require("./const");

var TTYUtil = module.exports = ttyu.TTYUtil;

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
