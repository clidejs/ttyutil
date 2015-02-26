var StringDecoder = require("string_decoder").StringDecoder;
var keypress = require("keypress");

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
        if(key) {
            key.type = "key";
            this.eventCB(key);
        } else {
            this.eventCB({
                type: "key",
                name: c
            });
        }
    },

    mouse: function(ev) {
        ev.type = "mouse";
        this.eventCB(ev);
    }
};
