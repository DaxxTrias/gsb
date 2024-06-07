#ifndef ASTEROIDFILTERING_H
#define ASTEROIDFILTERING_H

#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

// Function declarations
void parseAsteroids(const std::vector<const char*>& asteroids);
bool isInFilter(const char* parsedString);
std::unordered_set<std::string> getActiveFilters();
void updateCache();

// Declare external variables
extern struct FilterState {
    std::unordered_map<std::string, bool> filters;
} filterState;

extern std::unordered_set<std::string> activeFiltersCache;
extern bool cacheValid;

#endif
