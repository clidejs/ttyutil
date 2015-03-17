var TTYUtil = require("../export")(require("../build/Release/ttyu"));
var expect = require("chai").expect;

require("./spec.js")(TTYUtil, expect);
