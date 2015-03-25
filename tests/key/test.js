var cp = require("child_process").fork(path.join(__dirname, "key.js"));

cp.on("message", console.log);
cp.send({which: 1, ctrl: 0});
