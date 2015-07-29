var util = require("../installer/util");

util.waterfall([
    require("./key"),
    require("./mouse")
], function() {
    console.log("finished without errors");
    process.exit(0);
});
