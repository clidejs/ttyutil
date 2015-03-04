# clidejs/ttyutil #

A terminal utility module for node.js serving a cross-platform console api.

### Note ###
currently under development

Implemented:

 - Mousepress support (VT102 Terminals (e.g. iTerm2), Windows cmd.exe)
 - Keypress support (VT100 Terminals (e.g. xterm, iTerm2, Terminal.app,..),
 Windows cmd.exe)
 - Resize events (VT102 Terminals (e.g. iTerm2), Windows cmd.exe (width only))

TODO:

 - fix that annoying malloc error (Abort trap: 6)
 - add documentation
 - add error handling
 - add output functions and expose properties to the js module
 - add .which property on keys with key identifiers (like: `KEY_ENTER`, ...)

Features under consideration:

 - Mousepress support for VT100 Terminals
 - add better color support for cmd.exe

Note: Support for mouseevents in Terminal.app could be added with SIMBL and
 [MouseTerm](https://github.com/brodie/mouseterm) (untested).

### Usage ###
see test.js

### License: MIT ###
