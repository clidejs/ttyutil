module.exports = function(TTYUtil, expect) {
    describe("TTYUtil `mouse` event handling", function() {
        describe("MOUSEDOWN", require("./mouse-down")(TTYUtil, expect));
    });
};
