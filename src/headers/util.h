#ifndef TTYUTIL_UTIL_H_
#define TTYUTIL_UTIL_H_

int util_rgb2term(const char *rgb) {
    //TODO
    return 0;
}

int util_hex2term(const char *hex) {
    //TODO
    return 0;
}

int util_color(const char *c) {
    if(c[0] == '#') {
        return util_hex2term(&c[1]);
    } else if(c[0] == 'r' && c[1] == 'g' && c[2] == 'b' && c[3] == '(') {
        return util_rgb2term(&c[4]);
    } else {
        return util_hex2term(c);
    }
}

#endif // TTYUTIL_UTIL_H_
