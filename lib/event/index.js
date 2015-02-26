var TYPE = require("./type");

module.exports = {
    createMouseEvent: function(action, x, y, scrollx, scrolly, button, state) {
        return {
            type: TYPE.MOUSE,
            action: action,
            x: x,
            y: y,
            scrollx: scrollx,
            scrolly: scrolly,
            button: button,
            state: state
        };
    },
    createKeyEvent: function(action, state, name, code) {
        return {
            type: TYPE.KEY,
            action: action,
            state: state,
            name: name,
            code: code
        };
    }
};
