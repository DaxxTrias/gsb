#pragma once
#include <cstdint>
#include <vector>

uintptr_t getMultiLevelPointer(uintptr_t baseAddress, std::vector<uintptr_t> offsets);