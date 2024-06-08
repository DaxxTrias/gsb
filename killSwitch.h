#pragma once
#ifndef KILLSWITCH_H
#define KILLSWITCH_H

#include <atomic>

// Declare the kill switch
extern std::atomic<bool> killSwitch;

#endif // KILLSWITCH_H
