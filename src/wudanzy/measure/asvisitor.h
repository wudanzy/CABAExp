#ifndef __AS_VISITOR_H__
#define __AS_VISITOR_H__
#include <string>
#include "as.h"

namespace wudanzy {
namespace measure {

// forward declarations
class AS;
class ASset;
class MOAS;

// visitor
class ASVisitor {
public:
    virtual void visitAS(const AS* as) {}
    virtual void visitASset(const ASset* set) {}
    virtual void visitMOAS(MOAS* moas) = 0;
    virtual ~ASVisitor() {}
protected:
    ASVisitor() {}
};

class ASPrintVisitor : public ASVisitor {
public:
    ASPrintVisitor(const bool flag = false): bDotNotation(flag) {}
    const std::string& getStr() { return ASstring; }
    virtual void visitAS(const AS* as) override;
    virtual void visitASset(const ASset* set) override;
    virtual void visitMOAS(MOAS* moas) override;
    void clear() { ASstring.clear(); }
private:
    std::string ASstring;
    bool bDotNotation;
};

class ASAbsorbVisitor : public ASVisitor {
public:
    // virtual void visitAS(const AS* as) override {}
    // virtual void visitASset(const ASset* set) override {}
    virtual void visitMOAS(MOAS* moas) override;
private:
    template<typename T>
    bool isIncluded(const std::set<T> &small, const std::set<T> &big);
};

} // namespace measure
} // namespace wudanzy
#endif
