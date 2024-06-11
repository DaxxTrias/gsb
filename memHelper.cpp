#include <iostream>
#include <vector>
#include <Windows.h>

uintptr_t getMultiLevelPointer(uintptr_t baseAddress, const std::vector<uintptr_t>& offsets) {
    uintptr_t currentAddress = baseAddress;

    for (size_t i = 0; i < offsets.size(); ++i) {
        if (!currentAddress) {
            std::cerr << "Invalid pointer encountered at level " << i << std::endl;
            return 0;
        }

        currentAddress = *reinterpret_cast<uintptr_t*>(currentAddress); // Dereference the pointer
        currentAddress += offsets[i]; // Add the offset
    }

    return currentAddress;
}

uintptr_t getPointerAddress(uintptr_t baseAddress, uintptr_t initialOffset, const std::vector<uintptr_t>& offsets) {
    uintptr_t currentAddress = baseAddress + initialOffset;

    for (size_t i = 0; i < offsets.size(); ++i) {
        if (!currentAddress) {
            std::cerr << "Invalid pointer encountered at level " << i << std::endl;
            return 0;
        }

        currentAddress = *reinterpret_cast<uintptr_t*>(currentAddress); // Dereference the pointer
        currentAddress += offsets[i]; // Add the offset
    }

    return currentAddress;
}