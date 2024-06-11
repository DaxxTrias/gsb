#ifndef MEMHELPER_H
#define MEMHELPER_H

#include <vector>
#include <cstdint>

uintptr_t getMultiLevelPointer(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets);
uintptr_t getPointerAddress(uintptr_t baseAddress, uintptr_t initialOffset, const std::vector<uintptr_t>& offsets);

#endif // MEMHELPER_H