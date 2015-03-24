var TTYUtil = require("../index");

var ttyu = new TTYUtil();

ttyu.on(TTYUtil.EVENT.KEY, function(ev) {
    ttyu.write(JSON.stringify(ev) + "\r\n");
});

ttyu.start();

ttyu.emit(TTYUtil.EVENT.KEY, 65, 4);
