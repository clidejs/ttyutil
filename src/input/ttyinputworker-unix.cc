#ifndef PLATFORM_WINDOWS // so we are on unix

#include "ttyinputworker.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
/*
#include <gpm.h>

TODO implement mouse and key event tracking in c
currently implemented in js for VT102 Terminals only!
*/

#endif // !PLATFORM_WINDOWS
