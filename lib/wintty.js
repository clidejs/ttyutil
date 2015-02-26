var TTY = require("../build/Release/tty");
var Event = require("./event");
var KEY = require("./event/key");
var MOUSE = require("./event/mouse");
var BUTTON = require("./event/button");

var TTYUtil = module.exports = function(error, event) {
    this.eventCB = event;
    this.tty = new TTY(error, this.handle.bind(this));
    this.running = false;
    this.lastButton = 0;
};

TTYUtil.prototype = {
    start: function() {
        if(this.running) return;
        this.tty.start();
        this.running = true;
    },

    stop: function() {
        if(!this.running) return;
        this.tty.stop();
        this.running = false;
    },

    handle: function(out) {
        out = out.split(" ");
        var action = 0,
            ev;

        if(out[0] === "k") { // key event
            action = out[2] === "1" ? KEY.DOWN : KEY.UP;

            ev = Event.createKeyEvent(action,   // action
                parseInt(out[4],10),            // state
                out[5],                         // name
                parseInt(out[1], 10));          // code
            this.eventCB(ev);
        } else if(out[0] === "m") { // mouse event
            var scrollx = 0,
                scrolly = 0,
                button = parseInt(out[2],10);

            switch(out[1]) {
                case "1":
                    action = MOUSE.MOVE;
                    break;
                case "2":
                    action = MOUSE.DBLCLICK;
                    break;
                case "4":
                    action = MOUSE.WHEEL;
                    scrollx = Math.sign(button);
                    button = BUTTON.WHEEL;
                    break;
                case "8":
                    action = MOUSE.WHEEL;
                    scrolly = Math.sign(button);
                    button = BUTTON.WHEEL;
                    break;
                default: //"0"
                    if(button === 0) {
                        button = this.lastButton;
                        action = MOUSE.UP;
                    } else {
                        this.lastButton = button;
                        action = MOUSE.DOWN;
                    }
                    break;
            }

            ev = Event.createMouseEvent(action, // action
                parseInt(out[4],10),            // x
                parseInt(out[5],10),            // y
                scrollx,                        // scrollx
                scrolly,                        // scrolly
                button,                         // button
                parseInt(out[3], 10));          // state
            this.eventCB(ev);
        }
    }
};
