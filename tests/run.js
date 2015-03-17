var TTYUtil = require("../export")(require("../build/Debug/ttyu"));
var expect = require("chai").expect;

require("./spec.js")(TTYUtil, expect);
