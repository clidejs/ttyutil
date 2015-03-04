var TTYUtil = module.exports = require("./build/Release/tty");
TTYUtil.create = function() {
    return new TTYUtil.TTYUtil();
};
