var is = require("node-is");

module.exports = function(TTYUtil, expect) {
    describe("TTYUtil Specification", function() {

        describe("TTYUtil.prototype", function() {
            var keys = Object.keys(TTYUtil.prototype);
            it("should have 13 enumerable properties", function() {
                expect(keys.length).to.be.equal(13);
            });

            describe("#start()", function() {
                it("should be enumerable", function() {
                    expect(keys.indexOf("start")).to.be.not.equal(-1);
                });
                it("should be a function", function() {
                    is.expect.type.of(TTYUtil.prototype.start)
                            .to.be.equal("Function");
                });
            });
        });
    });
};
