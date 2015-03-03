var TTYUtil = require("./index");

var tty = new TTYUtil();

tty.on("key", console.log);
tty.on("error", function(err) {
    try { tty.stop(); } catch(e) {}
    console.error(err);
});

tty.start();
