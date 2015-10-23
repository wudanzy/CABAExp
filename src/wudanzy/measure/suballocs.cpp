#include "suballocs.h"
#include <sstream>
#include <iostream>
#include "filter.h"
#include "prefix.h"

namespace wudanzy {
namespace measure {

bool Suballoc::bIPv6 = false;

std::ostream &operator << (std::ostream& os, const Suballoc &alloc) {
    os << alloc.first << ' ' << (Suballoc::bIPv6 ? Prefix::prefix6(alloc.second) : Prefix::prefix(alloc.second));
    return os;
}

std::ostream &operator << (std::ostream& os, const SuballocSet &set) {
    for (auto it = set.begin(); it != set.end(); ++it) {
        os << '|' << it->first.first << ' ' 
           << (Suballoc::bIPv6 ? Prefix::prefix6(it->first.second) : Prefix::prefix(it->first.second)) << it->second;
    }
    return os;
}


std::istream &operator >> (std::istream& is, Suballoc &alloc) {
    std::string key, value;
    is >> key >> value;
    alloc.first = std::stoul(key);
    alloc.second = Suballoc::bIPv6 ? Prefix::bits6(value) : Prefix::bits(value);
    return is;
}

std::istream &operator >> (std::istream& is, SuballocSet &set) {
    std::string key, value;
    while (is >> key >> value) {
        bool frag = false, successive = false, absorbed = false;
        while (value.back() < '0' || value.back() > '9') {
            if (value.back()=='*') {
                frag = true;
                value.pop_back();
            } else if (value.back()=='s') {
                successive = true;
                value.pop_back();
            } else if (value.back()=='C') {
                absorbed = true;
                value.pop_back();
            }
        }
        Suballoc suballoc(std::stoul(key), Suballoc::bIPv6 ? Prefix::bits6(value) : Prefix::bits(value));
        set[suballoc].isFragment = frag;
        set[suballoc].isSuccessiveFragment = successive;
        set[suballoc].isAbsorbed = absorbed;
    } 
    return is;
}

std::ostream &operator << (std::ostream& os, const SuballocMap &map) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        os << it->first << it->second << std::endl;
    }
    return os;
}

std::istream &operator >> (std::istream& is, SuballocMap &map) {
    std::string line;
    std::string moas;
    while (getline(is, line)) {
        std::istringstream record(line);
        record.imbue(std::locale(std::locale(), FilterCharacter::instance()));
        record >> moas;
        record >> map[MOAS(moas)];
    }
    return is;
}

} // namespace measure
} // namespace wudanzy
