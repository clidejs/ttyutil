var TTYUtil = require("./index");

var tty = new TTYUtil();

console.log(Object.keys(TTYUtil.prototype));

/*
tty.on("key", console.log);
tty.on("error", function(err) {
    try { tty.stop(); } catch(e) {}
    console.error(err);
});

tty.start();
*/
