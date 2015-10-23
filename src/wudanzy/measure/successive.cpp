#include "successive.h"
#include <ostream>
#include "suballocs.h"
namespace wudanzy {
namespace measure {

std::ostream &operator << (std::ostream &os, const SuccessiveSuballocs &alloc) {
    os << alloc.first << alloc.second << std::endl;
    return os;
}

std::ostream &operator << (std::ostream &os, const SuccessiveSets &set) {
    for (auto it = set.begin(); it != set.end(); ++it) {
        os << *it;
    }
    return os;
}

} // namespace measure
} // namespace wudanzy
