var ttyu = require("../index");
var assert = require("assert");
var mouseEvent = [];

// mouse up & mouse down
for(var type in ttyu.MOUSE) {
    mouseEvent.push([ttyu.EVENT.MOUSEUP, ttyu.MOUSE[type]]);
    mouseEvent.push([ttyu.EVENT.MOUSEDOWN, ttyu.MOUSE[type]]);
}

module.exports = function(cb) {
    console.log("\r\ntest 'tests/mouse.js'\r");
    ttyu.start();

    (function check(i, cb) {
        var event = ttyu.MouseEvent.call(null, mouseEvent[i][0],
            mouseEvent[i][1], Math.round(Math.random() * 10),
            Math.round(Math.random() * 10), 0);
        var listener = function(ev) {
            console.log(JSON.stringify(ev));
            assert.equal(ev.type, event.type);
            assert.equal(ev.button, event.button);
            assert.equal(ev.x, event.x);
            assert.equal(ev.y, event.y);
            assert.equal(ev.ctrl, event.ctrl);
            ttyu.off(event.type, listener);
            console.log(" done!\r");
            if(i <= 0) cb(); else check(i - 1, cb);
        };
        ttyu.on(event.type, listener);
        ttyu.emit(event);
        console.log("testing " + JSON.stringify(event) + " ... ");
    })(mouseEvent.length - 1, function() {
        ttyu.stop();
        console.log("test 'tests/mouse.js' passed\r");
        cb();
    });
};
