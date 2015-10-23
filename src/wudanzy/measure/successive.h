#ifndef __SUCCESSIVE_H__
#define __SUCCESSIVE_H__
#include <map>
namespace wudanzy {
namespace measure {

class MOAS;
class SuballocSet;

typedef std::pair<MOAS, SuballocSet> SuccessiveSuballocs;

class SuccessiveSets : public std::multimap<MOAS, SuballocSet> {

};

std::ostream &operator << (std::ostream &os, const SuccessiveSets &set);

std::ostream &operator << (std::ostream &os, const SuccessiveSuballocs &alloc);

} // namespace measure
} // namespace wudanzy
#endif
