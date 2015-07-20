var util = require("./util");

var ENV = {
  TTYU_NCURSES_REBUILD: process.env.TTYU_NCURSES_REBUILD || false,
  TTYU_BUILD_DEBUG: process.env.TTYU_BUILD_DEBUG || false,
  TTYU_CODE_DEBUG: process.env.TTYU_CODE_DEBUG || false,
  WIN_OS: process.platform === "win32",
  PKG: require("../package.json"),
  CONST: require("../lib/const")
};

util.waterfall([
  // startup
  function(cb) {
    console.log("    [preinstall] ENV: [ TTYU_NCURSES_REBUILD='" +
        ENV.TTYU_NCURSES_REBUILD + "', TTYU_BUILD_DEBUG='" +
        ENV.TTYU_BUILD_DEBUG + "', TTYU_CODE_DEBUG='" + ENV.TTYU_CODE_DEBUG +
        "', WIN_OS='" + ENV.WIN_OS + "' ]");
    cb();
  },
  // generate header file
  require("./generate")(ENV),
  // prepare ncurses
  require("./ncurses")(ENV)
], function(err) {
  if(err) throw err;
  console.log("    [preinstall] finished with code -0");
});
