var signal = module.exports = {
    on: function(ttyu, that, listener) {
        if(!that.__signals__) {
            that.__signals__ = {};
            that.__siglisten__ = [];

            // add signal listeners
            for(var sig in ttyu.TTYUtil.SIGNAL) {
                process.on(sig, (that.__siglisten__[sig] =
                        signal.listen(ttyu, sig, that)));
            }
        }
        that.__siglisten__.push(listener);
    },

    off: function(that, listener) {
        if(!that.__signals__) return;
        var i = that.__siglisten__.indexOf(listener);
        if(i !== -1) {
            that.__siglisten__.splice(i,1);
        }

        if(that.__siglisten__.length === 0) {
            // remove signal listeners
            for(var sig in that.__siglisten__) {
                process.removeListener(sig, that.__siglisten__[sig]);
            }

            that.__signals__ = undefined;
            that.__siglisten__ = undefined;
        }
    },

    emit: function(sig) {
        process.emit(sig);
    },

    listen: function(ttyu, sig, that) {
        return function() {
            var ev = {
                type: ttyu.TTYUtil.EVENT.SIGNAL,
                signal: sig
            };
            for(var i = 0; i < that.__siglisten__.length; ++i) {
                that.__siglisten__[i].call(that, ev);
            }
        };
    }
};
