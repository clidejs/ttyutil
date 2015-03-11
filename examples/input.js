var TTYUtil = require("../index");

var events = [ "error", "resize", "key", "mouseup", "mousedown", "mousemove",
        "mousewheel", "mousehwheel"];

var ttyu = new TTYUtil();

for(var i = 0; i < events.length; ++i) {
    ttyu.on(events[i], listener(events[i]));
}

function listener(name) {
    return function(ev) {
        console.log(name + ": " + JSON.stringify(ev) + "\r");
    };
}

ttyu.start();
