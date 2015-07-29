var utils = module.exports = {
    rgb: function(r, g, b) {
        return utils.rgb5(r/51, g/51, b/51);
    },

    rgb5: function(r, g, b) {
        return 16 + Math.round(r)*36 + Math.round(g)*6 + Math.round(b);
    },

    hex: function(c) {
        c = c[0] === "#" ? c.substring(1) : c;
        return utils.rgb(parseInt(c.substring(0, 2), 16),
            parseInt(c.substring(2, 4), 16), parseInt(c.substring(4, 6), 16));
    },

    startsWith: function(str, searchString, position) {
        position = position || 0;
        return str.indexOf(searchString, position) === position;
    }
};
