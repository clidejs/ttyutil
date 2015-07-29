var path = require("path");
var fs = require("fs");
var cp = require("child_process");

var util = module.exports = {
  // waterfall function (inspired by async.waterfall)
  waterfall: function(fns, end) {
    (function h2o(index) {
      fns[index](function(err, id) {
        if(err) return end(err, id);
        if(++index < fns.length) h2o(index); else end();
      });
    })(0);
  },

  // spawn a new process (child_process.spawn) which will callback when finished
  // with stdout and stderr output as string
  spawn: function(cmd, args, options, cb) {
    var proc = cp.spawn(cmd, args, options);
    var stdout = "";
    var stderr = "";

    proc.stdout.on("data", function(chunk) {
      stdout += chunk;
    });
    proc.stderr.on("data", function(chunk) {
      stderr += chunk;
    });
    proc.on("close", function(code) {
      cb(code, stdout, stderr);
    });
  },

  rmRecursiveSync: function(p) {
    var files = [];
    if(fs.existsSync(p)) {
      files = fs.readdirSync(p);
      for(var index = 0, file; index < files.length; ++index) {
        file = path.join(p, files[index]);
        if(fs.lstatSync(file).isDirectory()) { // recurse
          util.rmRecursiveSync(file);
        } else { // delete file
          fs.unlinkSync(file);
        }
      }
      fs.rmdirSync(p);
    }
  },

  mkdirRecursiveSync: function(p) {
    if(!fs.existsSync(p)) {
      util.mkdirRecursiveSync(path.join(p, ".."));
      fs.mkdirSync(p);
    }
  }
};
