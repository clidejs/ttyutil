var TTYUtil = require("../index");

var events = [ "error", "resize", "key", "mouseup", "mousedown", "mousemove",
        "mousewheel", "mousehwheel"];

var ttyu = new TTYUtil();

for(var i = 0; i < events.length; ++i) {
    ttyu.on(events[i], listener(events[i]));
}

function listener(name) {
    return function(ev) {
        ttyu.write(name + ": " + JSON.stringify(ev) + "\r\n", ev === "error" ?
                "#F00" : "#0F0", ev === "error" ? "#400" : "#040");
    };
}

ttyu.start();
