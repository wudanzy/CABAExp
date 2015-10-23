#include "filter.h"
#include <algorithm>
namespace wudanzy {
namespace measure {

FilterCharacter* FilterCharacter::_instance = NULL;
FilterCharacter* FilterCharacter::instance() {
    if (_instance == NULL) {
        _instance = new FilterCharacter("|");
    }
    return _instance;
}

// FilterCharacter::FilterCharacter(const std::string &chars):std::ctype<char>(getTable(chars),true) {}
// FilterCharacter::FilterCharacter(const std::string &chars):std::ctype<char>(getTable(chars),true,1) {}
FilterCharacter::FilterCharacter(const std::string &chars):std::ctype<char>(getTable(chars),false,1) {}
// FilterCharacter::FilterCharacter(const std::string &chars):std::ctype<char>(getTable(chars),false) {}
std::ctype_base::mask const* FilterCharacter::getTable(const std::string& chars) {
    table = new std::ctype_base::mask[std::ctype<char>::table_size];
    const std::ctype_base::mask *ctable = std::ctype<char>::classic_table();
    std::copy_n(ctable,std::ctype<char>::table_size,table);
    // std::fill_n(table,std::ctype<char>::table_size,std::ctype_base::mask());
    for (size_t i = 0; i < chars.size(); ++i) {
        table[static_cast<unsigned char>(chars[i])] = std::ctype_base::space;
    }
    return table;
}

} // namespace measure
} // namespace wudanzy
