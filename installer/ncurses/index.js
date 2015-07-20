var cp = require("child_process");
var fs = require("fs");
var path = require("path");
var https = require("https");

var util = require("../util");
var execute = require("./download");

module.exports = function(ENV) {
  ENV.NCURSES_PATH = path.join(__dirname, "..", "..", "deps", "ncurses");
  ENV.NCURSES_URL = "https://github.com/mirror/ncurses";
  var rebuild = function(cb) {
    console.log("    [preinstall/ncurses] rebuilding...");
    cb();
  };

  return function(cb) {
    if(ENV.WIN_OS) {
      console.log("    [preinstall/ncurses] step skipped for windows");
      cb();
    } else {
      console.log("    [preinstall/ncurses] checking...");
      var rebuild = ENV.TTYU_NCURSES_REBUILD;
      var download = !fs.existsSync(ENV.NCURSES_PATH);

      if(rebuild) return execute(ENV, true, cb);

      util.spawn("gcc", ["-lncurses"], { },
          function(code, stdout, stderr) {
        if(stderr.search(/\-lncurses/) === -1) {
          console.log("    [preinstall/ncurses] library found");
          cb();
        } else {
          if(fs.existsSync(path.join(ENV.NCURSES_PATH, "lib",
              "libncurses++.a"))) {
            console.log("    [preinstall/ncurses] using latest private build");
            cb();
          } else {
            execute(ENV, download, cb);
          }
        }
      });
    }
  }
};
