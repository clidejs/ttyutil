# ttyutil changelog

## 0.1.0

### Features:

 - first beta release
 - basic implementation for unix and windows
 - listen to mouse, key and resize events(mouse in VT100 Terminals and CMD only)
 - Getters for width, height, cursor position, color-range, terminal mode
 - Setters for cursor position
 - Methods to move the cursor, write colored output, beep sound, clear screen
 parts, prepare rendering strings and receive color codes

### Known Bugs:

 - colored output on windows is broken (do not use!)
