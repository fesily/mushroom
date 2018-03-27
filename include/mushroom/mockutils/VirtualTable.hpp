#pragma once

#include <mushroom\mockutils\VTUtils.hpp>
#ifdef _MSC_VER
#include <mushroom\mockutils\mscpp\VirtualTable.hpp>
#else
#include <mushroom\mockutils\gcc\VirtualTable.hpp>
#endif

