#pragma once
#include <cstring>

#ifndef NDEBUG
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DEBUG_MSG(str) do { std::cout << __FILENAME__ << ": " << str << std::endl; } while( false )
#else
#define DEBUG_MSG(str) do { } while ( false )
#endif