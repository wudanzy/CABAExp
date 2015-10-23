#ifndef __SUBALLOCS_VISITOR_H__
#define __SUBALLOCS_VISITOR_H__
#include <iostream>
#include <map>
#include "statistic.h"
#include "suballocs.h"

namespace wudanzy {
namespace measure {
class SuccessiveSets;
class AllocTrie;

class AggregateSuballocVisitor : public SuballocVisitor {
public:
    virtual void visitSuballocSet(SuballocSet* set);
    virtual ~AggregateSuballocVisitor() {}
};

class FragmentIdentifyVisitor : public SuballocVisitor {
public:
    virtual void visitSuballocSet(SuballocSet* set);
    virtual ~FragmentIdentifyVisitor() {}
};

template<typename KEY, typename VALUE>
class CountingFragmentMonthVisitor : public SuballocVisitor {
public: 
    CountingFragmentMonthVisitor(Distribution<KEY, VALUE> *d, bool flag = false): dist(d), bFragmentOnly(flag) {}
    virtual void visitSuballocSet(SuballocSet* set);
private:
    Distribution<KEY, VALUE> *dist;
    bool bFragmentOnly;
};

template<typename KEY, typename VALUE>
void CountingFragmentMonthVisitor<KEY, VALUE>::visitSuballocSet(SuballocSet* set) {
    for (auto it = set->begin(); it != set->end(); ++it) {
        if (!bFragmentOnly||it->second.isFragment) {
            (*this->dist)[it->first.first]++;
        }
    }
}

class SuccessiveFragmentVisitor : public SuballocVisitor {
public:
    virtual void visitSuballocSet(SuballocSet* set);
};

class SuccessiveFragmentIdentifier : public SuballocVisitor {
public:
    virtual void visitSuballocSet(SuballocSet* set);
    virtual void visitSuballocMap(SuballocMap* map);
    SuccessiveFragmentIdentifier(SuccessiveSets* s): set(s) {}
private:
    SuccessiveSets *set;
    MOAS moas;
};

class MatchAllocationVisitor : public SuballocVisitor {
public:
    MatchAllocationVisitor(AllocTrie* t): trie(t) {}
    virtual void visitSuballocSet(SuballocSet* set);
private:
    AllocTrie* trie;
};

class OneSuballocOneLinePrinter : public SuballocVisitor {
public:
    OneSuballocOneLinePrinter(std::ostream& out): os(out) {}
    virtual void visitSuballocSet(SuballocSet* set);
    virtual void visitSuballocMap(SuballocMap* map);
private:
    std::ostream& os;
    const MOAS* pMoas;
};

} // namespace measure
} // namespace wudanzy
#endif
