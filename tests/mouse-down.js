require("it-each")({ testPerIteration: true});
var is = require("node-is");
var Const = require("../const");
var mouse = [Const.Mouse.LEFT, Const.Mouse.LEFT2,
        Const.Mouse.LEFT3, Const.Mouse.RIGHT];

function makeEvent() {
    return [mouse[Math.floor(Math.random() * mouse.length)],
            Math.round(Math.random() * 60), Math.round(Math.random() * 15), 0];
}

var current = [];
var mevents = [];

for(var i = 0; i < 100; ++i) {
    mevents.push(makeEvent());
}

module.exports = function(TTYUtil, expect) {
    return function() {
        var ttyu;

        before(function() {
            ttyu = new TTYUtil();
            ttyu.start();
        });

        it.each(mevents, "should recognize mouse input: %s", ['element'],
                function(element, next) {
            this.timeout(500);
            ttyu.on(TTYUtil.EVENT.MOUSEDOWN, createTest(element, next));
            ttyu.emit(TTYUtil.MouseEvent(TTYUtil.EVENT.MOUSEDOWN, element[0],
                    element[1], element[2], element[3]));
        });

        afterEach(function() {
            var el;
            while((el = current.pop())) {
                ttyu.removeListener(TTYUtil.EVENT.MOUSEDOWN, el);
            }
        })

        after(function(done) {
            var el;
            while((el = current.pop())) {
                ttyu.removeListener(TTYUtil.EVENT.MOUSEDOWN, el);
            }
            setTimeout(function() {
                ttyu.destroy();
                done();
            }, 100);
        });

        function createTest(element, callback) {
            var test = function(ev) {
                is.expect.type.of(ev).to.be.equal("Object");
                expect(ev.button).to.be.equal(element[0]);
                expect(ev.x).to.be.equal(element[1]);
                expect(ev.y).to.be.equal(element[2]);
                expect(ev.ctrl).to.be.equal(element[3]);
                ttyu.removeListener(TTYUtil.EVENT.MOUSEDOWN, test);
                callback();
            };
            current.push(test);

            return test;
        }

        // TODO (@bbuecherl) add tests for .ctrl
    };
};
