// this module defines all constants which are used in c++ and js code
// they are included in js via require and in c++ using "node build"
module.exports = {
    Event: {
        error: 0,
        resize: 1,
        key: 2,
        mouseup: 3,
        mousedown: 4,
        mousemove: 5,
        mousewheel: 6,
        mousehwheel: 7
    },

    Mouse: {
        LEFT: 1,
        LEFT2: 4,
        LEFT3: 8,
        LEFT4: 16,
        RIGHT: 2
    },

    Which: {
        UNKNOWN: -1
    },

    Ctrl: {
        NULL: 0x00,
        ALT: 0x01,
        CTRL: 0x02,
        SHIFT: 0x04,
        ENHANCED: 0x08,
        CMD: 0x10,
        NUMLOCK: 0x20,
        SCROLLLOCK: 0x40,
        CAPSLOCK: 0x80
    },

    Mode: {
        CMD: 0x00,
        VT102: 0x01,
        VT100: 0x02
    }
};
