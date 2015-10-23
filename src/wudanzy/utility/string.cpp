#include "string.h"

namespace wudanzy {
namespace utility {

std::vector<string> string::split(const char ch) {
    std::vector<string> res;
    auto prev = cbegin(); 
    for (auto it = cbegin(); it != cend(); ++it) {
        if (ch == *it) {
            res.push_back(string(prev, it));
            prev = it + 1;
        }
    }
    res.push_back(string(prev, cend()));
    return res;
}

} // namespace utility
} // namespace wudanzy
