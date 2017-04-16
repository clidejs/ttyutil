var TTYUtil = require("../index");

var ttyu = new TTYUtil();

for(var event in TTYUtil.EVENT) {
    ttyu.on(TTYUtil.EVENT[event], listener(TTYUtil.EVENT[event]));
}

var l = function(ev) {
    ttyu.write("asdf");
}

ttyu.on(TTYUtil.EVENT.KEY, l);

function listener(name) {
    return function(ev) {
        ttyu.write(name + ": " + JSON.stringify(ev) + "\r\n", name === "error" ?
                "#F00" : "#0F0", name === "error" ? "#400" : "#040");
        if(name === "signal") {
            ttyu.destroy();
        }
    };
}

ttyu.start();
ttyu.off(TTYUtil.EVENT.KEY, l);