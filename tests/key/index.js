require("it-each")({ testPerIteration: true});
var is = require("node-is");
var path = require("path");
var Const = require("../../const");
var TTYUtil = require("../../export")(require("../../build/Release/ttyu"));

var which = [];

var keys = Object.keys(Const.Which);
for(var i = 0; i < keys.length; ++i) {
    c = Const.Which[keys[i]];
    if(c != 19 && c != -1 && which.indexOf(c) === -1)
        which.push(c);
}

module.exports = function(TTYUtil, expect) {
    describe("TTYUtil `key` event handling", function() {
        describe(".which", function() {
            var ttyu;

            before(function() {
                ttyu = new TTYUtil();
                ttyu.start();
            });

            it.each(which, "should recognize character #%s", ['element'],
                    function(element, next) {
                this.timeout(5000);
                ttyu.on(TTYUtil.EVENT.KEY, createTest(element, next));
                ttyu.emit(TTYUtil.EVENT.KEY, element, 0);
            });

            after(function() {
                ttyu.destroy();
            });

            function createTest(element, callback) {
                var test = function(ev) {
                    is.expect.type.of(ev).to.be.equal("Object");
                    expect(ev.which).to.be.equal(element);
                    ttyu.removeListener(TTYUtil.EVENT.KEY, test);
                    callback();
                };

                return test;
            }
        });
    });
};
