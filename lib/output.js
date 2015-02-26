module.exports = function(TTYUtil) {
    TTYUtil.prototype.write = function(chunk, fg, bg) {
        this.output.write(this.hex(chunk, fg, bg));
        return this;
    };
    TTYUtil.prototype.hex = function(chunk, fg, bg) {
        if(fg) { chunk = "\x1b[38;5;" + hex(fg) + "m" + chunk + "\x1b[39m"; }
        if(bg) { chunk = "\x1b[48;5;" + hex(bg) + "m" + chunk + "\x1b[49m"; }
        return chunk;
    };
    TTYUtil.prototype.beep = function() {
        this.output.write("\x07");
        return this;
    };
    TTYUtil.prototype.clearLine = function() {
        this.output.write("\x1b[K");
        return this;
    };
    TTYUtil.prototype.clear = function() {
        this.output.write("\x1b[0m");
        return this;
    };
    TTYUtil.prototype.to = function(x, y) {
        this.x = Math.round(x);
        this.y = Math.round(y);
        this.output.write("\x1b[" + this.y + ";" + this.x + "H");
        return this;
    };
    TTYUtil.prototype.width = function() {
        return this.output.getWindowSize ? this.output.getWindowSize()[0] : 0;
    };
    TTYUtil.prototype.height = function() {
        return this.output.getWindowSize ? this.output.getWindowSize()[1] : 0;
    };
};

function rgb(r, g, b) {
    return 16 + Math.round(r / 255 * 5 * 36) +
        Math.round(g / 255 * 5 * 6) + Math.round(b / 255 * 5);
}

function hex(color) {
    color = color[0] === '#' ? color.substring(1) : color;
    return rgb(color.substring(0, 2), color.substring(2, 4), color.substring(4, 6));
}
