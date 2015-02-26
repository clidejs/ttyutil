var TTYUtil = require("./index");

var tty = new TTYUtil(console.error, function(event) {
    console.log(event);
    if(event.type === "key" && event.ctrl && event.name == "c") {
        tty.stop();
    }
});

tty.start();
