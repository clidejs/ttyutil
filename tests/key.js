require("it-each")({ testPerIteration: true});
var is = require("node-is");
var Const = require("../const");

var which = [];

var keys = Object.keys(Const.Which);
for(var i = 0; i < keys.length; ++i) {
    c = Const.Which[keys[i]];
    if(c != 19 && c != -1 && which.indexOf(c) === -1) {
        which.push(c);
    }
}

var unix_required = [
    Const.Which.BACK,
    Const.Which.TAB,
    Const.Which.CLEAR,
    Const.Which.ENTER,
    Const.Which.SHIFT,
    Const.Which.SPACE,
    Const.Which.PRIOR,
    Const.Which.NEXT,
    Const.Which.END,
    Const.Which.HOME,
    Const.Which.LEFT,
    Const.Which.UP,
    Const.Which.RIGHT,
    Const.Which.DOWN,
    Const.Which.PRINT,
    Const.Which.INSERT,
    Const.Which.DELETE,
    Const.Which.HELP,
    Const.Which.CHAR0,
    Const.Which.CHAR1,
    Const.Which.CHAR2,
    Const.Which.CHAR3,
    Const.Which.CHAR4,
    Const.Which.CHAR5,
    Const.Which.CHAR6,
    Const.Which.CHAR7,
    Const.Which.CHAR8,
    Const.Which.CHAR9,
    Const.Which.CHARA,
    Const.Which.CHARB,
    Const.Which.CHARC,
    Const.Which.CHARD,
    Const.Which.CHARE,
    Const.Which.CHARF,
    Const.Which.CHARG,
    Const.Which.CHARH,
    Const.Which.CHARI,
    Const.Which.CHARJ,
    Const.Which.CHARK,
    Const.Which.CHARL,
    Const.Which.CHARM,
    Const.Which.CHARN,
    Const.Which.CHARO,
    Const.Which.CHARP,
    Const.Which.CHARQ,
    Const.Which.CHARR,
    Const.Which.CHARS,
    Const.Which.CHART,
    Const.Which.CHARU,
    Const.Which.CHARV,
    Const.Which.CHARW,
    Const.Which.CHARX,
    Const.Which.CHARY,
    Const.Which.CHARZ,
    Const.Which.F1,
    Const.Which.F2,
    Const.Which.F3,
    Const.Which.F4,
    Const.Which.F5,
    Const.Which.F6,
    Const.Which.F7,
    Const.Which.F8,
    Const.Which.F9,
    Const.Which.F10,
    Const.Which.F11,
    Const.Which.F12,
    Const.Which.F13,
    Const.Which.F14,
    Const.Which.F15,
    Const.Which.F16,
    Const.Which.F17,
    Const.Which.F18,
    Const.Which.F19,
    Const.Which.F20,
    Const.Which.F21,
    Const.Which.F22,
    Const.Which.F23,
    Const.Which.F24,
    Const.Which.BROWSER_REFRESH
];

var current = [];

module.exports = function(ttyu, expect) {
    describe("TTYUtil `key` event handling", function() {
        describe(".which", function() {
            before(function() {
                ttyu.start();
            });

            it.each(which, "should recognize character #%s", ['element'],
                    function(element, next) {
                this.timeout(100);
                if(!(process.platform !== "win32" &&
                        unix_required.indexOf(element) === -1)) {
                    ttyu.on(ttyu.EVENT.KEY, createTest(element, next));
                    ttyu.emit(ttyu.KeyEvent(element, 0));
                } else {
                    next();
                }
            });

            afterEach(function() {
                var el;
                while((el = current.pop())) {
                    ttyu.removeListener(ttyu.EVENT.KEY, el);
                }
            });

            after(function(done) {
                var el;
                while((el = current.pop())) {
                    ttyu.removeListener(ttyu.EVENT.KEY, el);
                }
                setTimeout(function() {
                    ttyu.stop();
                    done();
                }, 1000);
            });

            function createTest(element, callback) {
                var test = function(ev) {
                    is.expect.type.of(ev).to.be.equal("Object");

                    if(process.platform !== "win32" &&
                            unix_required.indexOf(element) === -1) {
                        expect([element, Const.Which.UNKNOWN]).to.
                                include(ev.which);
                    } else {
                        expect(ev.which).to.be.equal(element);
                    }
                    callback();
                };
                current.push(test);

                return test;
            }
        });

        // TODO (@bbuecherl) add tests for .ctrl
    });
};
