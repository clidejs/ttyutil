# clidejs/ttyutil #

A terminal utility module for node.js serving a cross-platform console api.

### Note ###
currently under development

Implemented:
 - Mousepress support (VT102 Terminals (e.g. iTerm2), Windows cmd.exe)
 - Keypress support (VT100 Terminals (e.g. xterm, iTerm2, Terminal.app,..), Windows cmd.exe)

TODO:
 - Mousepress support for VT100 Terminals (via ncurses or gpm?)
 - Replace keypress module with c++ binding to improve performance (posix systems)
 - add output functions, cursor movement functions and coloring
 - add documentation
 - support other tty-streams than process.stdin and process.stdout ?
 - add configuration options
 - add listeners for resize and other events

### Events ###
Description of the event object properties which the callback is called with:

 - `type`: {String} "key" when a key event has occured, "mouse" when a mouse event has occured (see TTYUtil.TYPE)

Mouse events:
 - `action`: {int} mouse event action id (see TTYUtil.MOUSE)
 - `x`: {int} x-coordinate of the terminal stream, where the mouse event occured
 - `y`: {int} y-coordinate of the terminal stream, where the mouse event occured
 - `scrollx`: {int} scroll amount of the mouse event in x-axis
 - `scrolly`: {int} scroll amount of the mouse event in y-axis
 - `button`: {int} button id (see TTYUtil.BUTTON)
 - `state`: {int} control keys that where active when the event occured (see TTYUtil.STATE)

Key events:
 - `action`: {int} key event action id (see TTYUtil.KEY)
 - `state`: {int} control keys that where active when the event occured (see TTYUtil.STATE)
 - `name`: {String} key name
 - `code`: {int} key code (see TTYUtil.KEYCODE)

### Usage ###
see test.js

### License: MIT ###
