var ttyu = require("../lib/export")(
        require("../build/Release/ttyu"));

ttyu.start().bold().italic().underline();
ttyu.on(ttyu.EVENT.KEY, out);
ttyu.on(ttyu.EVENT.MOUSEDOWN, out);

function col() {
    return Math.round(Math.random()*255);
}

function out(ev) {
    ttyu.fg(col()).bg(col())
        .write(JSON.stringify(ev) + "\r\n");
    if(ev.which == ttyu.WHICH.CHARX) {
        ttyu.stop();
    }
};
