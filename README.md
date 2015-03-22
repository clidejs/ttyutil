# clidejs/ttyutil [![NPM](https://img.shields.io/npm/v/ttyutil.svg)](https://npmjs.com/package/ttyutil) [![AppVeyor](https://img.shields.io/appveyor/ci/bbuecherl/ttyutil.svg)](https://ci.appveyor.com/project/bbuecherl/ttyutil) [![Build Status](https://img.shields.io/travis/clidejs/ttyutil.svg)](https://travis-ci.org/clidejs/ttyutil) [![GitHub issues](https://img.shields.io/github/issues/clidejs/ttyutil.svg)](https://github.com/clidejs/ttyutil/issues) [![Gitter](https://img.shields.io/badge/gitter-chat-blue.svg)](https://gitter.im/clidejs/ttyutil)

[![NPM](https://nodei.co/npm/ttyutil.png?downloads=true&downloadRank=true)](https://nodei.co/npm/ttyutil/)
[![NPM](https://nodei.co/npm-dl/ttyutil.png?months=6&height=3)](https://nodei.co/npm/ttyutil/)

## About

A terminal utility module for node.js serving a cross-platform console api.
Works with Node.js v0.8 and above.

## [Changelog](https://github.com/clidejs/ttyutil/tree/master/CHANGELOG.md)

## Note

This project is still under development and thus the API may change, please
report any bugs [here](https://github.com/clidejs/ttyutil/issues), and feel free
to make feature requests.

## Installation

Since this is a native c++ node.js addon, you will need these tools to build the
packages (see [node-gyp](https://github.com/TooTallNate/node-gyp#installation)):

 - python (v2.7+, but not 3.x)
 - on unix:
    - make
    - c++ compiler (e.g. gcc)
 - on mac:
    - xcode command line tools
 - on windows:
    - Visual Studio 2010 or 2013

Simply add **ttyutil** to the dependencies of your project:

``` bash
npm install --save ttyutil
```

And require it anywhere in your codebase:

``` js
var TTYUtil = require("ttyutil");
```

## Usage

### `TTYUtil`

The `TTYUtil` exposes the `TTYUtil` constructor (`var ttyu = new TTYUtil()`).
This creates the `ttyu` object which is used to interact with the terminal.

#### `ttyu.on(event, listener)`

Adds an event `listener` callback for the `event`(see TTYUtil.EVENT).

#### `ttyu.removeListener(event, listener)`

Remove the event `listener` callback from the `event` (see TTYUtil.EVENT).

#### `ttyu.off(event, listener)`

Shortcut for `ttyu.removeListener(event, listener)`.

#### `ttyu.start()`

Start emitting input events (mouse, key and resize events) to the listeners
attached by `tty.on(event, listener)`.

#### `ttyu.stop()`

Stops emitting input events.

#### `ttyu.running`

Boolean flag indicating if input events are emitted (`true` when `ttyu.start()`
was executed). *read-only*

#### `ttyu.width`

Integer amount of horizontal character space in the terminal. *read-only*

#### `ttyu.height`

Integer amount of vertical character space in the terminal. *read-only*

#### `ttyu.mode`

Integer identifier for current terminal mode (can be one of TTYUtil.MODE).
*read-only*

#### `ttyu.colors`

Integer amount of colors available in the terminal (16-256). *read-only*

#### `ttyu.x`

Current x-position of the terminal cursor.

#### `ttyu.y`

Current y-position of the terminal cursor.

#### `ttyu.goto(x, y)`

Function to set the current cursor position to (`x`, `y`).

#### `ttyu.write(chunk, fg, bg)`

Function to write a string `chunk` at the current cursor position (`ttyu.x`,
`ttyu.y`) with the foreground color `fg` and the background color `bg` to the
terminal buffer. `fg` and `bg` can be terminal color codes, hexadecimal strings
(e.g. `"#FF0000"`) or rgb strings (e.g. `"rgb(255,0,0)"`).

#### `ttyu.prepare(chunk, fg, bg)`

Function to prepare a string `chunk` with the foreground color `fg` and
background color `bg`. Works like `ttyu.write(chunk, fg, bg)`, but returns the
formatted string without printing it to the terminal.

#### `ttyu.beep()`

Function to create a warning sound or visual warning effect.

#### `ttyu.clear(x, y, width, height)`

Function to clear a part of the terminal buffer. `x` and `y` describe the
top-left corner of the rectangle and `width` and `height` define the size of the
rectangle to clear. When called without arguments, this function will clear the
complete (visible) terminal buffer.

#### `ttyu.color(str)`

Function to get the terminal color code from `str`, a hexadecimal string
(e.g. `"#FF0000"`) or rgb string (e.g. `"rgb(255,0,0)"`).

### `TTYUtil.EVENT`

This object wraps all input events as constants:

- `TTYUtil.EVENT.ERROR` {`String`}: `error` identifier for error events
- `TTYUtil.EVENT.SIGNAL` {`String`}: `signal` identifier for process signal
events
- `TTYUtil.EVENT.KEY` {`String`}: `key` identifier for key events
- `TTYUtil.EVENT.RESIZE` {`String`}: `resize` identifier for resize events
- `TTYUtil.EVENT.MOUSEDOWN` {`String`}: `mousedown` identifier for mousedown
events (mouse button pressed)
- `TTYUtil.EVENT.MOUSEUP` {`String`}: `mouseup` identifier for mouse up events
(mouse button released)
- `TTYUtil.EVENT.MOUSEMOVE` {`String`}: `mousemove` identifier for mouse move
events
- `TTYUtil.EVENT.MOUSEWHEEL` {`String`}: `mousewheel` identifier for mouse
wheel events (vertical scroll events)
- `TTYUtil.EVENT.MOUSEHWHEEL` {`String`}: `mousehwheel` identifier for the
mouse hwheel events (horizontal scroll events)

### `TTYUtil.MODE`

This object wraps all terminal modes as constants:

- `TTYUtil.MODE.CMD` {`Integer`}: `0` identifier for the windows cmd.exe
- `TTYUtil.MODE.VT102` {`Integer`}: `1` identifier for VT102 compilant terminals
(e.g. iTerm2 for OSX or xTerm for X11)
- `TTYUtil.MODE.VT100` {`Integer`}: `2` identifier for VT100 compilant terminals
(e.g. Terminal.app for OSX)

### `TTYUtil.MOUSE`

This object wraps all mouse buttons identifier as constants:

- `TTYUtil.MOUSE.LEFT` {`Integer`}: `1` left-most mouse button
- `TTYUtil.MOUSE.LEFT2` {`Integer`}: `4` second left-most mouse button
- `TTYUtil.MOUSE.LEFT3` {`Integer`}: `8` thrid left-most mouse button
- `TTYUtil.MOUSE.LEFT4` {`Integer`}: `16` fourth left-most mouse button
- `TTYUtil.MOUSE.RIGHT` {`Integer`}: `2` right-most mouse button

### `TTYUtil.CTRL`

This object wraps all control key identifier as constants:

- `TTYUtil.CTRL.NULL` {`Integer`}: `0` no control key is active
- `TTYUtil.CTRL.ALT` {`Integer`}: `1` the alt key is pressed
- `TTYUtil.CTRL.CTRL` {`Integer`}: `2` the ctrl key is pressed
- `TTYUtil.CTRL.SHIFT` {`Integer`}: `4` the shift key is pressed
- `TTYUtil.CTRL.ENHANCED` {`Integer`}: `8` the enhanced key is pressed
- `TTYUtil.CTRL.CMD` {`Integer`}: `16` the cmd key is pressed (OSX)
- `TTYUtil.CTRL.NUMLOCK` {`Integer`}: `32` numlock is activated
- `TTYUtil.CTRL.SCROLLLOCK` {`Integer`}: `64` scrolllock is activated
- `TTYUtil.CTRL.CAPSLOCK` {`Integer`}: `128` capslock is activated

### `TTYUtil.SIGNAL`

This object contains a list of all signals that can be catched by ttyutil:

- `TTYUtil.SIGNAL.SIGINT` {`String`}: `SIGINT` interrupt event (e.g. CTRL+C)
- `TTYUtil.SIGNAL.SIGTERM` {`String`}: `SIGTERM` termination event
- `TTYUtil.SIGNAL.SIGPIPE` {`String`}: `SIGPIPE` pipe error event
- `TTYUtil.SIGNAL.SIGHUP` {`String`}: `SIGHUP` terminal close event

### `TTYUtil.WHICH`

This object wraps all key codes as constants.
*(see /const.js for the complete list)*

## Test

run the tests using `npm test`. check out /tests for test cases.

## Example

see the /examples folder

## License: MIT

Copyright (c) 2015 [Bernhard Bücherl](https://github.com/bbuecherl)

--------------------------------------------------------------------------------

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
