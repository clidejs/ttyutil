var is = require("node-is");

module.exports = function(TTYUtil, expect) {
    describe("TTYUtil Specification", function() {

        describe("TTYUtil.prototype", function() {
            var keys = Object.keys(TTYUtil.prototype);

            it("should have 12 enumerable properties", function() {
                expect(keys.length).to.be.equal(12);
            });

            it("#on() should be an enumerable function", function() {
                expect(keys.indexOf("on")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.on)
                        .to.be.equal("Function");
            });

            it("#off() should be an enumerable function", function() {
                expect(keys.indexOf("off")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.off)
                        .to.be.equal("Function");
            });

            it("#removeListener() should be an enumerable function",
                    function() {
                expect(keys.indexOf("removeListener")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.removeListener)
                        .to.be.equal("Function");
            });

            it("#emit() should be an enumerable function", function() {
                expect(keys.indexOf("emit")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.emit)
                        .to.be.equal("Function");
            });

            it("#start() should be an enumerable function", function() {
                expect(keys.indexOf("start")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.start)
                        .to.be.equal("Function");
            });

            it("#stop() should be an enumerable function", function() {
                expect(keys.indexOf("stop")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.stop)
                        .to.be.equal("Function");
            });

            it("#goto() should be an enumerable function", function() {
                expect(keys.indexOf("goto")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.goto)
                        .to.be.equal("Function");
            });

            it("#write() should be an enumerable function", function() {
                expect(keys.indexOf("write")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.write)
                        .to.be.equal("Function");
            });

            it("#prepare() should be an enumerable function", function() {
                expect(keys.indexOf("prepare")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.prepare)
                        .to.be.equal("Function");
            });

            it("#beep() should be an enumerable function", function() {
                expect(keys.indexOf("beep")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.beep)
                        .to.be.equal("Function");
            });

            it("#clear() should be an enumerable function", function() {
                expect(keys.indexOf("clear")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.clear)
                        .to.be.equal("Function");
            });

            it("#color() should be an enumerable function", function() {
                expect(keys.indexOf("color")).to.be.not.equal(-1);
                is.expect.type.of(TTYUtil.prototype.color)
                        .to.be.equal("Function");
            });
        });
    });
};
