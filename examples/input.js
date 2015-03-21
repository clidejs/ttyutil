var TTYUtil = require("../index");

var events = [ "error", "signal", "resize", "key", "mouseup", "mousedown",
        "mousemove", "mousewheel", "mousehwheel"];

var ttyu = new TTYUtil();

for(var i = 0; i < events.length; ++i) {
    ttyu.on(events[i], listener(events[i]));
}

function listener(name) {
    return function(ev) {
        ttyu.write(name + ": " + JSON.stringify(ev) + "\r\n", name === "error" ?
                "#F00" : "#0F0", name === "error" ? "#400" : "#040");
        if(name === "key" && ev.code === 3) {
            ttyu.stop();
        }
    };
}

ttyu.start();
