require("it-each")({ testPerIteration: true});
var is = require("node-is");
var path = require("path");

var which = [];

for(var i = 1; i < 255; ++i) {
    which.push(i);
}

module.exports = function(TTYUtil, expect) {
    describe("TTYUtil `key` event handling", function() {
        var cp = require("child_process").fork(path.join(__dirname, "key.js"));

        describe(".which", function() {
            it.each(which, "should recognize character #%s", ['element'],
                    function(element, next) {
                var testa = function(e) {
                    is.expect.type.of(e).to.be.equal("Object");
                    expect(e.which).to.be.equal(element);
                    cp.removeListener("message", testa);
                    next();
                };
                cp.on("message", testa);
                cp.send({ which: element, ctrl: 0 });
            });
        });
    });
};
