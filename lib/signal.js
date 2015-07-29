var signal = module.exports = {
    on: function(ttyu, listener) {
        if(!ttyu.__signals__) {
            ttyu.__signals__ = {};
            ttyu.__siglisten__ = [];

            // add signal listeners
            for(var sig in ttyu.SIGNAL) {
                process.on(sig, (ttyu.__siglisten__[sig] =
                        signal.listen(ttyu, sig)));
            }
        }
        ttyu.__siglisten__.push(listener);
    },

    off: function(ttyu, listener) {
        if(!ttyu.__signals__) return;
        var i = ttyu.__siglisten__.indexOf(listener);
        if(i !== -1) {
            ttyu.__siglisten__.splice(i,1);
        }

        if(ttyu.__siglisten__.length === 0) {
            // remove signal listeners
            for(var sig in ttyu.__siglisten__) {
                process.removeListener(sig, ttyu.__siglisten__[sig]);
            }

            ttyu.__signals__ = undefined;
            ttyu.__siglisten__ = undefined;
        }
    },

    emit: function(sig) {
        process.emit(sig);
    },

    listen: function(ttyu, sig) {
        return function() {
            var ev = {
                type: ttyu.EVENT.SIGNAL,
                signal: sig
            };
            for(var i = 0; i < ttyu.__siglisten__.length; ++i) {
                ttyu.__siglisten__[i].call(ttyu, ev);
            }
        };
    }
};
