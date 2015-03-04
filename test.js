var TTYUtil = require("./index");

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
var tty = new TTYUtil.create();

for(var i = 0; i < events.length; ++i) {
    listen(tty, events[i]);
}

tty.write(JSON.stringify(tty) + "\r\n", "#FF0000", "#00F");
tty.write("#365cff: " + tty.color("#365cff") + "\r\n", "#365cff", "#000");
tty.write("x: " + tty.x + " y: " + tty.y + "\r\n");
//tty.start();
