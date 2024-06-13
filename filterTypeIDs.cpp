#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <type_traits>

enum class statics : int {
    Type12 = 645,
    Type11 = 646,
    Type4 = 647,
    Type1 = 648,
    Type2 = 649,
    Type7 = 660,
    Type10 = 663,
    Type9 = 677,
    Type3 = 682,
    Type8 = 706,
    Type13 = 750,
    Type5 = 758,
    Type6 = 786,
    Type1000 = 10000
};

enum class ships : int {
    Type1 = 906,
    Type2 = 1196,
    Type1000 = 10000

};

template<typename E>
std::unordered_set<int> populateSet() {
    std::unordered_set<int> values;
    for (int e = static_cast<int>(E::Type1); e <= static_cast<int>(E::Type1000); ++e) {
        values.insert(e);
    }
    return values;
}

std::vector<int> filterTypeIDs(const std::vector<int>& typeIDs) {
    std::unordered_set<int> excludedTypes = populateSet<statics>();
    std::unordered_set<int> includedTypes = populateSet<ships>();

    std::vector<int> filteredList;

    for (const auto& typeID : typeIDs) {
        if (excludedTypes.find(typeID) == excludedTypes.end() && includedTypes.find(typeID) != includedTypes.end()) {
            filteredList.push_back(typeID);
        }
    }

    return filteredList;
}

