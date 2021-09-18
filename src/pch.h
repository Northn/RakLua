#pragma once

#include <string>

#include "rtdhook.hpp"

#pragma warning(disable : 26495) // Variable '%variable%' is uninitialized. Always initialize a member variable (type.6).
#pragma warning(disable : 6387)  // <argument> may be <value>: this does not adhere to the specification for the function <function name>
#pragma warning(disable : 6308)  // 'realloc' may return null pointer: assigning a null pointer to <variable>, which is passed as an argument to 'realloc', will cause the original memory block to be leaked
#pragma warning(disable : 26819) // Unannotated fallthrough between switch labels (es.78).
#pragma warning(disable : 26439) // This kind of function may not throw. Declare it 'noexcept'.

#define SOL_ALL_SAFETIES_ON 1
#include "sol.hpp"

#include "BitStream.h"
#include "RakClient.h"

#pragma warning(default : 26439)
#pragma warning(default : 26819)
#pragma warning(default : 6308)
#pragma warning(default : 6387)
#pragma warning(default : 26495)
