#ifndef DBG_H_
#define DBG_H_
#include <iostream>

#ifdef DEBUG
#   define DBGA std::cout << "A" << std::endl
#   define DBGB std::cout << "B" << std::endl
#   define DBGC std::cout << "C" << std::endl
#   define DBGD std::cout << "D" << std::endl
#   define DBGE std::cout << "E" << std::endl
#   define DBGF std::cout << "F" << std::endl
#   define DBGG std::cout << "G" << std::endl
#   define DBGH std::cout << "H" << std::endl
#   define DBGI std::cout << "I" << std::endl
#   define DBGJ std::cout << "J" << std::endl
#   define DBGK std::cout << "K" << std::endl
#   define DBGL std::cout << "L" << std::endl
#   define DBGM std::cout << "M" << std::endl
#   define DBGN std::cout << "N" << std::endl
#   define DBGO std::cout << "O" << std::endl
#   define DBGP std::cout << "P" << std::endl
#   define DBGQ std::cout << "Q" << std::endl
#   define DBGR std::cout << "R" << std::endl
#   define DBGS std::cout << "S" << std::endl
#   define DBGT std::cout << "T" << std::endl
#   define DBGU std::cout << "U" << std::endl
#   define DBGV std::cout << "V" << std::endl
#   define DBGW std::cout << "W" << std::endl
#   define DBGX std::cout << "X" << std::endl
#   define DBGY std::cout << "Y" << std::endl
#   define DBGZ std::cout << "Z" << std::endl
#else // DEBUG
#   define DBGA
#   define DBGB
#   define DBGC
#   define DBGD
#   define DBGE
#   define DBGF
#   define DBGG
#   define DBGH
#   define DBGI
#   define DBGJ
#   define DBGK
#   define DBGL
#   define DBGM
#   define DBGN
#   define DBGO
#   define DBGP
#   define DBGQ
#   define DBGR
#   define DBGS
#   define DBGT
#   define DBGU
#   define DBGV
#   define DBGW
#   define DBGX
#   define DBGY
#   define DBGZ
#endif // DEBUG

#define DBG(a) std::cout << a << std::endl

#endif // DBG_H_
