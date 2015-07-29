var cp = require("child_process");

module.exports = function(ENV, cb) {
  console.log("    [preinstall/ncurses] './configure'");
  var cfg = cp.spawn("./configure", ["--with-shared", "--enable-pc-files",
      "--enable-widec", "--without-normal"], {
    cwd: ENV.NCURSES_PATH
  });
  if(ENV.TTYU_BUILD_DEBUG) {
    cfg.stdout.pipe(process.stdout);
    cfg.stderr.pipe(process.stderr);
  }
  cfg.on("exit", function(code) {
    if(code !== 0) throw "ncurses './configure' exited with code -" + code;
    console.log("    [preinstall/ncurses] 'make'");
    var make = cp.spawn("make", {
      cwd: ENV.NCURSES_PATH
    });
    if(ENV.TTYU_BUILD_DEBUG) {
      make.stdout.pipe(process.stdout);
      make.stderr.pipe(process.stderr);
    }
    make.on("exit", function(code) {
      if(code !== 0) throw "ncurses 'make' exited with code -" + code;
      cb();
    });
  });
};
