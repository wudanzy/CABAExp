#ifndef __SEGMENT_H__
#define __SEGMENT_H__
#include <cstdlib>
#include <utility>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <memory>
#include "wudanzy/trie/trie.h"
#include "as.h"
#include "suballocs.h"

// forward delcarations
class RelationGraph;

namespace wudanzy {
namespace measure {

// forward declarations in namespace measure
class Segment;
class SegmentMap;
class SegmentVector;
struct SuballocAttribute;
class AllocTrie;

// segment visitors
class SegmentVisitor {
public:    
    virtual void visitSegmentMap(SegmentMap& map);
    virtual void visitSegmentVector(SegmentVector& v);
    virtual void visitSegment(const Segment& segment, SuballocAttribute& attribute) = 0;
    virtual ~SegmentVisitor() {}
};

class SegmentIdentifier : public SegmentVisitor {
public:    
    SegmentIdentifier(wudanzy::trie::BinaryTrie<size_t> &theTrie): trie(theTrie) {}
    virtual void visitSegmentVector(SegmentVector& v) override;
    virtual void visitSegment(const Segment& segment, SuballocAttribute& attribute) override;
private:
    wudanzy::trie::BinaryTrie<size_t> &trie;
};

class BrotherSegmentFinder : public SegmentVisitor {
public:    
    BrotherSegmentFinder(wudanzy::trie::BinaryTrie<const Segment*> &theTrie, RelationGraph &g): trie(theTrie), graph(g) {}
    virtual void visitSegmentVector(SegmentVector& v) override;
    virtual void visitSegment(const Segment& segment, SuballocAttribute& attribute) override;
private:
    wudanzy::trie::BinaryTrie<const Segment*> &trie;
    RelationGraph &graph;
};

class BrotherDistanceCalculator : public SegmentVisitor {
public:
    enum calc_type {
        calc_unweighted_average,
        calc_weighted_average,
        calc_count
    };
    BrotherDistanceCalculator(calc_type c = calc_unweighted_average): calc(c) {}
    virtual void visitSegment(const Segment& segment, SuballocAttribute& attribute) override;
private:
    calc_type calc;
};

class MatchingAllocationFinder : public SegmentVisitor {
public:    
    MatchingAllocationFinder(AllocTrie &theTrie): trie(theTrie) {}
    virtual void visitSegment(const Segment& segment, SuballocAttribute& attribute);
private:
    AllocTrie &trie;
};

// segment related classes
class Segment : public std::pair<MOAS, Suballoc>
{
public:
    Segment(MOAS as = MOAS(), unsigned key = 0, std::string bits = "");
    Segment(const Segment& seg);
    Segment& operator = (const Segment& other);
    bool operator < (const Segment& other);
    std::string to_string(const char sep = ' ') const;
    MOAS& as;
    unsigned& time;
    std::string& bits;
};

class SegmentMap : public std::map<Segment, SuballocAttribute> 
{
public:
    SegmentMap() {} 
    void Accept(SegmentVisitor& v) {
        v.visitSegmentMap(*this);
    }
};

typedef std::pair<Segment, SuballocAttribute> SegmentPair;

class SegmentVectorObserver;

class SegmentVector : public std::vector<SegmentPair> {
public:    
    void Accept(SegmentVisitor& v) {
        v.visitSegmentVector(*this);
    }
    static bool compareByTime(const SegmentPair& p1, const SegmentPair& p2);
    static bool compareByAS(const SegmentPair& p1, const SegmentPair& p2);
    static bool compareByIP(const SegmentPair& p1, const SegmentPair& p2);
    void push_back(SegmentPair& pair);
    virtual void Attach(SegmentVectorObserver *observer);
    virtual void Detach(SegmentVectorObserver *observer);
    virtual void Notify();
private:
    std::list<SegmentVectorObserver*> observers;
    typedef std::vector<SegmentPair> base_type;
};


class SegmentVectorObserver {
public:
    virtual ~SegmentVectorObserver() {}
    virtual void Update(SegmentVector &vector) = 0;
protected:
    SegmentVectorObserver() {}
};

// Store and manage all dangling segments
class SegmentFactory {
public:
    SegmentFactory() {}
    virtual ~SegmentFactory() {}
    virtual Segment* getSegment(MOAS as, unsigned key, std::string bits) = 0;
};

// a factory that create segments whenever a segment is needed
class DynamicAlloatedSegmentFactory : public SegmentFactory {
public:
    virtual Segment* getSegment(MOAS as, unsigned key, std::string bits);
private:
    std::map<std::pair<MOAS, std::string>, std::unique_ptr<Segment>> map;
}; 

// a factory that only returns existed segments in a SegmentVector object
class SegmentVectorSynthesizedFactory : public SegmentFactory, public SegmentVectorObserver {
public:
    SegmentVectorSynthesizedFactory(SegmentVector &v);
    virtual Segment* getSegment(MOAS as, unsigned key, std::string bits);
    virtual void Update(SegmentVector &vector);
private:
    std::map<std::pair<MOAS, std::string>, std::unique_ptr<Segment>> map;
    size_t numberOfValidSegments;
    SegmentVector &vector;
};

std::ostream &operator << (std::ostream& os, const Segment &seg);

std::ostream &operator << (std::ostream& os, const SegmentMap &map);

std::ostream &operator << (std::ostream& os, const SegmentPair &pair);

std::ostream &operator << (std::ostream& os, const SegmentVector &vector);

std::istream &operator >> (std::istream& is, Segment &seg);

std::istream &operator >> (std::istream& is, SegmentMap &map);

std::istream &operator >> (std::istream& is, SegmentVector &vector);

} // namespace measure
} // namespace wudanzy
#endif
