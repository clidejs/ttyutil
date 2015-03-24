var TTYUtil = require("../export")(require("../build/Release/ttyu"));
var expect = require("chai").expect;

// test specs
require("./spec")(TTYUtil, expect);

// test input listeners
require("./key")(TTYUtil, expect);
//require("./mouse")(TTYUtil, expect);
//require("./signal")(TTYUtil, expect);

// test output functions
//require("./output")(TTYUtil, expect);
