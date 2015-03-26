var TTYUtil = require("../../export")(require("../../build/Release/ttyu"));

process.on("message", function(test) {
    try {
        var ttyu = new TTYUtil();
        var testfn = function(e) {
            ttyu.off(TTYUtil.EVENT.KEY, testfn);
            ttyu.destroy();
            process.send(e);
        };
        ttyu.start();
        setTimeout(function() {
            ttyu.on(TTYUtil.EVENT.KEY, testfn);
            ttyu.emit(TTYUtil.EVENT.KEY, test.which, test.ctrl);
        }, 10);
    } catch(ex) {
        process.send(ex);
    }
});
