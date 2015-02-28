#ifndef TTYUTIL_TTYOUTPUTUTIL_H_
#define TTYUTIL_TTYOUTPUTUTIL_H_

#include <node.h>
#include <nan.h>

/**
 * Set cursor state to one of the following:
 * - 0: invisible
 * - 1: normal
 * - 2: very visible
 */
void SetCursorState(int type);

/**
 * Check if the terminal got color support
 */
bool HasColorSupport();



#endif // TTYUTIL_TTYOUTPUTUTIL_H_
