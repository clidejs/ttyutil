var TTYUtil = require("../build/Debug/tty");

var events = [
    "error",
    "resize",
    "key",
    "mouseup",
    "mousedown",
    "mousemove",
    "mousewheel",
    "mousehwheel"
];
function listen(tty, event) {
    tty.on(event, function(out) {
        console.log(event + ": ", out, "\r");
    });
}
var tty = new TTYUtil.TTYUtil();

for(var i = 0; i < events.length; ++i) {
    listen(tty, events[i]);
}

tty.start();
