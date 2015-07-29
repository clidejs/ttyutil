var gethub = require("gethub");

var build = require("./build");
var util = require("../util");

module.exports = function(ENV, download, cb) {
  if(download) {
    util.rmRecursiveSync(ENV.NCURSES_PATH);
    console.log("    [preinstall/ncurses] cloning from " + ENV.NCURSES_URL);
    gethub("mirror", "ncurses", "master", ENV.NCURSES_PATH, function(err) {
      if(err) throw err;
      build(ENV, cb);
    });
  } else {
    build(ENV, cb);
  }
};
