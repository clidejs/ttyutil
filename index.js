var TTYUtil;

if(process.platform === "win32") {
    TTYUtil = require("./build/Release/tty");
} else {
    // TODO implement this in c++ to improve performance? gpm or ncurses?
    TTYUtil = require("./lib/xterm-vt102");
}

TTYUtil.TYPE = require("./lib/event/type");
TTYUtil.MOUSE = require("./lib/event/mouse");
TTYUtil.BUTTON = require("./lib/event/button");
TTYUtil.KEY = require("./lib/event/key");
TTYUtil.STATE = require("./lib/event/state");
TTYUtil.KEYCODE = require("./lib/event/keycode");
TTYUtil.MODE = require("./lib/mode");

if(process.platform === "win32") {
    TTYUtil.mode = TTYUtil.MODE.CMD;
} else {
    // TODO check for VT102
    TTYUtil.mode = TTYUtil.MODE.VT100;
}

module.exports = TTYUtil;
