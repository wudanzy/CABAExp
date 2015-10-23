#include "tmap.h"
#include "filter.h"

namespace wudanzy {
namespace measure {

void TimeMap::Accept(TimeVisitor &v) { 
    v.visitTimeMap(this); 
}

void TimeTrie::Accept(TimeVisitor &v) {
   v.visitTimeTrie(this); 
}

std::ostream &operator << (std::ostream& os, const TimeMap &map) {
    int cnt = 0;
    ASPrintVisitor printer;
    for (TimeMap::const_iterator it = map.begin(); it != map.end(); ++it) {
        if (cnt++) os << '|';
        printer.clear();
        // const MOAS* moas = &(it->first);
        // const_cast<MOAS*>(moas)->Accept(printer);      
        static_cast<MOAS>(it->first).Accept(printer);
        os << printer.getStr();
        for (std::set<unsigned>::const_iterator rt = it->second.begin(); rt != it->second.end(); ++rt) {
            os << ' ' << *rt;
        }
        // os << '|';
    }
    // for (TimeMap::MOAS::const_iterator it = map.moas.begin(); it != map.moas.end(); ++it) {
        // if (it->second.size()>2) {
        // os << "\\" << it->first;
        // for (std::vector<std::string>::const_iterator rt = it->second.begin(); rt != it->second.end(); ++rt) {
            // os << ' ' << *rt;
        // }
        // }
    // }
    return os;
}

std::istream &operator >> (std::istream& is, TimeMap &map) {
    is.imbue(std::locale(std::locale(), FilterCharacter::instance()));
    std::string as;
    unsigned time;
    while (is.peek() == '|') {
        is >> as;
        MOAS moas(as);
        while (is.peek() == ' ') {
            is >> time;
            // if (moas.empty()) {
            //     throw std::runtime_error(as + "MOAS is empty");
            // }
            if (is&&moas.size()) map[moas].insert(time);
        }
    } 
    return is;
}

} // namespace measure
} // namespace wudanzy
