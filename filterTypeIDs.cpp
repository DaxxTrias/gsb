#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <type_traits>

enum statics {
    ROCK = 11201,
    ROCK2 = 11189,
    ROCK3 = 11082

};

enum ships {
    SHIP_PART1 = 134664

};

//template<typename E>
//std::unordered_set<int> getEnumValues() {
//    std::unordered_set<int> values;
//    for (int e = static_cast<int>(E::Type1); e <= static_cast<int>(E::Type6); ++e) {
//        if (std::is_same<E, ExcludedTypeIDs>::value && e > static_cast<int>(ExcludedTypeIDs::Type3)) break;
//        values.insert(e);
//    }
//    return values;
//}
//
//std::vector<int> filterTypeIDs(const std::vector<int>& typeIDs) {
//    auto excludedTypes = getEnumValues<statics>();
//    auto includedTypes = getEnumValues<ships>();
//
//    std::vector<int> filteredList;
//
//    for (const auto& typeID : typeIDs) {
//        if (excludedTypes.find(typeID) == excludedTypes.end() && includedTypes.find(typeID) != includedTypes.end()) {
//            filteredList.push_back(typeID);
//        }
//    }
//
//    return filteredList;
//}
//
//int main() {
//    // Example array of TypeIDs
//    std::vector<int> typeIDs = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
//
//    std::vector<int> filteredTypeIDs = filterTypeIDs(typeIDs);
//
//    std::cout << "Filtered TypeIDs:" << std::endl;
//    for (const auto& id : filteredTypeIDs) {
//        std::cout << id << std::endl;
//    }
//
//    return 0;
//}
