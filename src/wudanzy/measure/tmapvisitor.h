#ifndef __TIME_VISITOR_H__
#define __TIME_VISITOR_H__
#include "asvisitor.h"
#include "suballocs.h"
#include "tmap.h"

namespace wudanzy {
namespace measure {

// Forward declarations
class TimeMap;
class TimeTrie;
class AllocTrie;

// Visits TimeMap and TimeTrie
class TimeVisitor {
public:
    virtual void visitTimeMap(TimeMap* tmap) = 0;
    virtual void visitTimeTrie(TimeTrie* trie);
    virtual ~TimeVisitor() {}
protected:
    TimeVisitor() {}
};

// Remove the time-record that appears before the allocation of this block
class CleanTimeVisitor : public TimeVisitor {
public:
    virtual void visitTimeMap(TimeMap* tmap) override;
    explicit CleanTimeVisitor(const unsigned t): time(t) {}
private:
    unsigned time;
};

// Remove the time-record that appears before the allocation of this block
// For those prefix whose block is not allocated or assigned, the prefix is removed
class TimeMapCleaner : public TimeVisitor {
public:
    explicit TimeMapCleaner(AllocTrie &trie): allocTrie(trie) {}
    virtual void visitTimeMap(TimeMap* tmap) override;
    virtual void visitTimeTrie(TimeTrie *trie) override;
private:
    void cleanByTime(TimeMap* tmap, const size_t startTime);
    AllocTrie &allocTrie;
    std::string bits;
};

// Parse and simplify the moas
class ParseTimeVisitor : public TimeVisitor {
public:
    virtual void visitTimeMap(TimeMap* tmap) override;
private:
    // ASPrintVisitor printer;
    ASAbsorbVisitor absorber;
};

// Classify MOAS into AS
class ClassifyTimeVisitor : public TimeVisitor {
public:
    virtual void visitTimeMap(TimeMap* tmap) override;
};

// Convert into suballocs
class ConvertSuballocTimeVisitor : public TimeVisitor {
public:
    ConvertSuballocTimeVisitor(SuballocMap &map, size_t t = 201503): time(t), smap(map) {}
    virtual void visitTimeMap(TimeMap *tmap) override;
    virtual void visitTimeTrie(TimeTrie *trie) override;
private:
    size_t time;
    SuballocMap &smap;
    std::string prefix;
};

} // namespace measure
} // namespace wudanzy
#endif
