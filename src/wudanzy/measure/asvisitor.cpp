#include "asvisitor.h"

namespace wudanzy {
namespace measure {

void ASPrintVisitor::visitAS(const AS* as) {
    std::string tmpStr;
    if (bDotNotation&&as->number >> AS::len) {
        tmpStr = std::to_string(as->number>>AS::len)+"."+std::to_string(as->number&AS::mask);
    } else {
        tmpStr = std::to_string(as->number);
    }
    ASstring += tmpStr;
}

void ASPrintVisitor::visitASset(const ASset* set) {
    int cnt = 0;
    for (ASset::iterator it = set->begin(); it != set->end(); ++it) {
        if (cnt++) ASstring.push_back(',');
        it->Accept(*this);
    }
}

void ASPrintVisitor::visitMOAS(MOAS* moas) {
    int cnt = 0;
    for (MOAS::iterator it = moas->begin(); it != moas->end(); ++it) {
        if (cnt++) ASstring.push_back('_');
        it->Accept(*this);
    }
}

void ASAbsorbVisitor::visitMOAS(MOAS* moas) {
    for (MOAS::iterator it = moas->begin(); it != moas->end(); ++it) {
        MOAS::iterator itb = it;
        ++itb;
        while (itb!=moas->end()) {
            if (isIncluded(*it,*itb)){
                moas->erase(itb++);
            } else {
                ++itb;
            }
        }
    }
}

template<typename T>
bool ASAbsorbVisitor::isIncluded(const std::set<T> &small, const std::set<T> &big) {
    for (typename std::set<T>::iterator it = small.begin(); it != small.end(); ++it) {
        if (big.count(*it)==0) {
            return false;
        } 
    } 
    return true;
}

} // namespace measure
} // namespace wudanzy
