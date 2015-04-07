# ttyutil changelog

## 1.0.0 TODOs

 - mouse selection
 - fullscreen detection
 - improve examples
 - add pixel rendering (e.g. for pngs) functions
 - rework goto/write functionality
 - add X11 implementations to remove curses where possible

## 0.2.0 (WIP)

 - added `.pause()` function
 - renamed `.stop()` to `.destroy()`
 - all functions will throw now when the object was not started or already
 destroyed
 - added `.emit()` function
 - added tests
 - small bug fixes
 - added `.type` property to event objects
 - iojs support

### TODO

 - fix segfault on unix

## 0.1.2

 - fixed infinite loop in helper method `util_rgb2term()`
 - added `EVENT.SIGNAL` events to handle low-level signals like `SIGINT`
 - fixed cleanup after `.stop()`

## 0.1.1

 - fixed bugs when compiled in node <0.11. Is node 0.8+ compatible now
 - windows colored output working now
 - Known Bug: (unix) after `.stop()` the terminal is still recording raw input

## 0.1.0

 - first beta release
 - basic implementation for unix and windows
 - listen to mouse, key and resize events(mouse in VT100 Terminals and CMD only)
 - Getters for width, height, cursor position, color-range, terminal mode
 - Setters for cursor position
 - Methods to move the cursor, write colored output, beep sound, clear screen
 parts, prepare rendering strings and receive color codes
