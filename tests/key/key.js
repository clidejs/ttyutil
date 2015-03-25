var TTYUtil = require("../../export")(require("../../build/Release/ttyu"));

process.on("message", function(test) {
    var ttyu = new TTYUtil();
    ttyu.start();
    var testa = function(e) {
        ttyu.off(TTYUtil.EVENT.KEY, testa);
        ttyu.destroy();
        process.send(e);
    };
    ttyu.on(TTYUtil.EVENT.KEY, testa);
    ttyu.emit(TTYUtil.EVENT.KEY, test.which, test.ctrl);
});
