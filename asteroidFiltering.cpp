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
       {"exorium", true},
       {"surtrite", false},
       {"karnite", false},
       {"aegisium", true},
       {"kutonium", true},
       {"targium", true},
       {"arkanium", true},
       {"lukium", true},
       {"ilmatrium", true},
       {"ymrium", true},
       {"xhalium", true},
       {"daltium", true},
       {"haderite", true},
       {"merkerium", true},
       {"corazium", true},
       {"ukonium", true},
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

bool parseAsteroids(const std::vector<const char*>& asteroids) {
    for (const char* asteroid : asteroids) {
        if (isInFilter(asteroid)) {

            //printf("Matched asteroid: %s\n", asteroid);
            return true;
        }
    }
    return false;
}
