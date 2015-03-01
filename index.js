// TODO
var TTYUtil = require("./build/Release/tty");

var tty = new TTYUtil(console.log);
tty.start();

/* TODO segment fault 11!
setTimeout(function() {
    tty.stop();
}, 3000);*/
