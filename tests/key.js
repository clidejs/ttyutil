var ttyu = require("../index");
var assert = require("assert");

var unix_required = [
    ttyu.WHICH.BACK,
    ttyu.WHICH.TAB,
    ttyu.WHICH.CLEAR,
    ttyu.WHICH.ENTER,
    ttyu.WHICH.SHIFT,
    ttyu.WHICH.SPACE,
    ttyu.WHICH.PRIOR,
    ttyu.WHICH.NEXT,
    ttyu.WHICH.END,
    ttyu.WHICH.HOME,
    ttyu.WHICH.LEFT,
    ttyu.WHICH.UP,
    ttyu.WHICH.RIGHT,
    ttyu.WHICH.DOWN,
    ttyu.WHICH.PRINT,
    ttyu.WHICH.INSERT,
    ttyu.WHICH.DELETE,
    ttyu.WHICH.HELP,
    ttyu.WHICH.CHAR0,
    ttyu.WHICH.CHAR1,
    ttyu.WHICH.CHAR2,
    ttyu.WHICH.CHAR3,
    ttyu.WHICH.CHAR4,
    ttyu.WHICH.CHAR5,
    ttyu.WHICH.CHAR6,
    ttyu.WHICH.CHAR7,
    ttyu.WHICH.CHAR8,
    ttyu.WHICH.CHAR9,
    ttyu.WHICH.CHARA,
    ttyu.WHICH.CHARB,
    ttyu.WHICH.CHARC,
    ttyu.WHICH.CHARD,
    ttyu.WHICH.CHARE,
    ttyu.WHICH.CHARF,
    ttyu.WHICH.CHARG,
    ttyu.WHICH.CHARH,
    ttyu.WHICH.CHARI,
    ttyu.WHICH.CHARJ,
    ttyu.WHICH.CHARK,
    ttyu.WHICH.CHARL,
    ttyu.WHICH.CHARM,
    ttyu.WHICH.CHARN,
    ttyu.WHICH.CHARO,
    ttyu.WHICH.CHARP,
    ttyu.WHICH.CHARQ,
    ttyu.WHICH.CHARR,
    ttyu.WHICH.CHARS,
    ttyu.WHICH.CHART,
    ttyu.WHICH.CHARU,
    ttyu.WHICH.CHARV,
    ttyu.WHICH.CHARW,
    ttyu.WHICH.CHARX,
    ttyu.WHICH.CHARY,
    ttyu.WHICH.CHARZ,
    ttyu.WHICH.F1,
    ttyu.WHICH.F2,
    ttyu.WHICH.F3,
    ttyu.WHICH.F4,
    ttyu.WHICH.F5,
    ttyu.WHICH.F6,
    ttyu.WHICH.F7,
    ttyu.WHICH.F8,
    ttyu.WHICH.F9,
    ttyu.WHICH.F10,
    ttyu.WHICH.F11,
    ttyu.WHICH.F12,
    ttyu.WHICH.F13,
    ttyu.WHICH.F14,
    ttyu.WHICH.F15,
    ttyu.WHICH.F16,
    ttyu.WHICH.F17,
    ttyu.WHICH.F18,
    ttyu.WHICH.F19,
    ttyu.WHICH.F20,
    ttyu.WHICH.F21,
    ttyu.WHICH.F22,
    ttyu.WHICH.F23,
    ttyu.WHICH.F24,
    ttyu.WHICH.BROWSER_REFRESH
];
console.log("\r\ntest 'tests/key.js'\r");
ttyu.start();

(function check(i, cb) {
    var listener = function(ev) {
        assert.equal(ev.which, unix_required[i], " which should be " +
            unix_required[i]);
        ttyu.off(ttyu.EVENT.KEY, listener);
        if(i <= 0) cb(); else check(i - 1, cb);
    };
    ttyu.on(ttyu.EVENT.KEY, listener);
    ttyu.emit(ttyu.KeyEvent(unix_required[i], 0));
})(unix_required.length - 1, function() {
    ttyu.stop();
    console.log("test 'tests/key.js' passed\r");
});
