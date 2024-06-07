#include <unordered_map>
#include <string>
#include <unordered_set>


struct FilterState {
    std::unordered_map<std::string, bool> filters = {
       {"ice", false},
       {"ajatite", false},
       {"valkite", false},
       {"bastium", false},
       {"charodium", true},
       {"vokarium", false},
       {"nhurgite", false},
       {"surtrite", false},
       {"karnite", false},
       {"aegisium", true},
       {"kutonium", true},
       {"targium", true},
       {"arkanium", true},
       {"lukium", true},
       {"ilmatrium", true},
       {"ymirium", true},
       {"xhalium", true},
       {"daltium", true},
       {"haderite", true},
       {"merkerium", true}
    };
};

FilterState filterState;

std::unordered_set<std::string> activeFiltersCache;

std::unordered_set<std::string> getActiveFilters() {
    std::unordered_set<std::string> activeFilters;
    for (const auto& [filter, isActive] : filterState.filters) {
        if (isActive) {
            activeFilters.insert(filter);
        }
    }
    return activeFilters;
}

bool cacheValid = false;

void updateCache() {
    activeFiltersCache = getActiveFilters();
    cacheValid = true;
}

bool isInFilter(const char* parsedString) {
    if (!cacheValid) {
        updateCache();
    }
    for (const auto& filter : activeFiltersCache) {
        if (strstr(parsedString, filter.c_str())) {
            return true;
        }
    }
    return false;
}

void parseAsteroids(const std::vector<const char*>& asteroids) {
    for (const char* asteroid : asteroids) {
        if (isInFilter(asteroid)) {
            // Do something with the matched asteroid
            //printf("Matched asteroid: %s\n", asteroid);
        }
    }
}