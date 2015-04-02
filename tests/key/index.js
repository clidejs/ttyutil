require("it-each")({ testPerIteration: true});
var is = require("node-is");
var path = require("path");
var fork = require("child_process").fork;
var Const = require("../../const");

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
            var cp = fork(path.join(__dirname, "key.js"));

            it.each(which, "should recognize character #%s", ['element'],
                    function(element, next) {
                this.timeout(500);
                var testa = function(x,e) {
                    is.expect.type.of(e).to.be.equal("Object");
                    expect(e.which).to.be.equal(element);
                    cp.removeListener("message", testa);
                    next();
                    cp.kill();
                };
                cp.on("message", testa);
                cp.send({ which: element, ctrl: 0 });
            });
        });
    });
};
