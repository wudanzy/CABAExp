#ifndef __FILTER_H__
#define __FILTER_H__
#include <locale>

namespace wudanzy {
namespace measure {

class FilterCharacter: public std::ctype<char>
{
public:
    static FilterCharacter* instance();
protected:
    FilterCharacter(const std::string& chars);
    ~FilterCharacter() {delete []table;}
private:
    static FilterCharacter* _instance;
    std::ctype_base::mask const* getTable(const std::string& chars);
    std::ctype_base::mask *table;
};

} // namespace measure
} // namespace wudanzy
#endif
