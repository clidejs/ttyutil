var StringDecoder = require("string_decoder").StringDecoder;
var keypress = require("keypress");
var Event = require("./event");
var KEY = require("./event/key");
var MOUSE = require("./event/mouse");
var BUTTON = require("./event/button");
var STATE = require("./event/state");

var TTYUtil = module.exports = function(error, event) {
    this.eventCB = event;

    this.input = process.stdin;
    this.output = process.stdout;
    this.running = false;
};

TTYUtil.prototype = {
    start: function() {
        if(this.running) return;

        keypress(this.input);
        this.input.setRawMode(true);
        this.input.on("keypress", this.key.bind(this));
        this.input.on("mousepress", this.mouse.bind(this));

        keypress.enableMouse(this.output);
        this.input.resume();

        this.running = true;
    },

    stop: function() {
        if(!this.running) return;

        keypress.disableMouse(this.output);
        this.input.pause();
        this.running = false;
    },

    key: function(c, key) {
        var ctrl = 0;

        if(key) {
            if(key.ctrl) { ctrl = ctrl | STATE.LEFT_CTRL; }
            if(key.meta) { ctrl = ctrl | STATE.LEFT_ALT; }
            if(key.shift) { ctrl = ctrl | STATE.SHIFT; }

            this.eventCB(Event.createKeyEvent(KEY.DOWN, ctrl, key.name, key.name.charCodeAt(0)));
            this.eventCB(Event.createKeyEvent(KEY.UP, ctrl, key.name, key.name.charCodeAt(0)));
        } else {
            this.eventCB(Event.createKeyEvent(KEY.DOWN, ctrl, c, c.charCodeAt(0)));
            this.eventCB(Event.createKeyEvent(KEY.UP, ctrl, c, c.charCodeAt(0)))
        }
    },

    mouse: function(ev) {
        var ctrl = 0,
            action = 0;

        if(ev.ctrl) { ctrl = ctrl | STATE.LEFT_CTRL | STATE.RIGHT_CTRL; }
        if(ev.meta) { ctrl = ctrl | STATE.LEFT_ALT | STATE.RIGHT_ALT; }
        if(ev.shift) { ctrl = ctrl | STATE.SHIFT; }

        if(ev.scroll === 0 && !ev.release) { action = MOUSE.DOWN; }
        if(ev.scroll === 0 && ev.release) { action = MOUSE.UP; }
        if(ev.scroll !== 0) { action = MOUSE.MOVE; }

        this.eventCB(Event.createMouseEvent(action, ev.x, ev.y, 0, ev.scroll, 0, ctrl));
    }
};
