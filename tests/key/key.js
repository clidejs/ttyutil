var TTYUtil = require("../../export")(require("../../build/Release/ttyu"));

var ttyu = new TTYUtil();
var send = process.send;
ttyu.start();
ttyu.on(TTYUtil.EVENT.KEY, function(e) {
    send("test", e);
});

process.on("message", function(test) {
    if(test === "exit") {
        ttyu.destroy();
    } else {
        ttyu.emit(TTYUtil.EVENT.KEY, test.which, test.ctrl);
    }
});
