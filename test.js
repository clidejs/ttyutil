var TTYUtil = require("./index");

var tty = new TTYUtil(console.error, function(event) {
    console.log(event);
    if(event.type === TTYUtil.TYPE.KEY && event.state === TTYUtil.STATE.LEFT_CTRL && event.name === "c") {
        tty.stop();
    }
});

tty.start();
