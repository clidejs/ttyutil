var ttyu = require("../lib/export")(require("../build/Release/ttyu"));
var expect = require("chai").expect;

// test specs
// require("./spec")(ttyu, expect);

// test input listeners
require("./key")(ttyu, expect);
//require("./mouse")(ttyu, expect);
//require("./signal")(ttyu, expect);

// test output functions
//require("./output")(ttyu, expect);
