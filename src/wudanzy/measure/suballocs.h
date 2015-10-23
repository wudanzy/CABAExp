#ifndef __SUBALLOCS_H__
#define __SUBALLOCS_H__
#include <map>
#include <vector>
#include "as.h"
#include "attribute.h"

namespace wudanzy {
namespace measure {

// forward declarations
class SuballocSet;
class SuballocMap;
class Segment;
class SegmentFactory;

class SuballocVisitor {
public:
    virtual void visitSuballocSet(SuballocSet* set) = 0;
    virtual void visitSuballocMap(SuballocMap* map);
    virtual ~SuballocVisitor() {}
protected:
    SuballocVisitor() {}
};

// key: time when it appears first
// value: the prefix
class Suballoc : public std::pair<unsigned, std::string> 
{
public:
    friend std::istream &operator >> (std::istream& is, Suballoc &alloc);
    Suballoc(unsigned key, std::string value): std::pair<unsigned, std::string>(key, value) {}
    bool operator < (const Suballoc& other) { return first < other.first || (first == other.first && second < other.second); }
    static bool bIPv6;
};

// class Suballoc : public std::map<std::string, unsigned> 
// class SuballocSet : public std::set<Suballoc>
// A set of suballocs with their attributes
class SuballocSet : public std::map<Suballoc, SuballocAttribute>
{
public:
    void Accept(SuballocVisitor& v) {
        v.visitSuballocSet(this);
    }
};

std::ostream &operator << (std::ostream& os, const Suballoc &alloc);

std::ostream &operator << (std::ostream& os, const SuballocSet &set);

std::istream &operator >> (std::istream& is, Suballoc &alloc);

std::istream &operator >> (std::istream& is, SuballocSet &set);

// key: the AS
// value: a set of suballocs
class SuballocMap : public std::map<MOAS, SuballocSet>
{
public:
    void Accept(SuballocVisitor& v) {
        v.visitSuballocMap(this);
    }
};

std::ostream &operator << (std::ostream& os, const SuballocMap &map);

std::istream &operator >> (std::istream& is, SuballocMap &map);

} // namespace measure
} // namespace wudanzy
#endif
