#ifndef __WUDANZY_UTILITY_STRING
#define __WUDANZY_UTILITY_STRING
#include <string>
#include <vector>

namespace wudanzy {
namespace utility {

class string : public std::string {
private:
    typedef std::string base_type;
public:
    using base_type::basic_string;
    std::vector<string> split(const char ch = ' ');
};

} // namespace utility
} // namespace wudanzy
#endif
