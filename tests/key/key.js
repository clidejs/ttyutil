var TTYUtil = require("../../export")(require("../../build/Release/ttyu"));
var ttyu = new TTYUtil();

process.on("message", function(test) {
    ttyu.start();
    var testa = function(e) {
        ttyu.off(TTYUtil.EVENT.KEY, testa);
        ttyu.stop();
        process.send(e);
    };
    ttyu.on(TTYUtil.EVENT.KEY, testa);
    ttyu.emit(TTYUtil.EVENT.KEY, test.which, test.ctrl);
});
