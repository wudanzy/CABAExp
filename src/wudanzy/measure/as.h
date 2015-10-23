#ifndef __AS_H__
#define __AS_H__
#include <set>
#include <stdexcept>
#include "asvisitor.h"

namespace wudanzy {
namespace measure {

// forward declarations
class ASVisitor;
class ASPrintVisitor;

// represent a single AS
class AS {
public:
    AS(const std::string &line);
    bool operator == (const AS& other) const {
        return number == other.number;
    }
    bool operator < (const AS& other) const {
        return number < other.number;
    }
    void Accept(ASVisitor& v) const;
    bool isValid() const { return !isPrivate(); }
    // bool isPrivate() const { return (number >= 64512 && number <= 65535) || (number >= 4200000000 && number <= 4294967294); }
    // bool isPrivate() const { return (number >= 64496 && number <= 64511) || (number >= 64512 && number <= 65535) || (number >= 65536 && number <= 65551) || (number >= 4200000000 && number <= 4294967294); }
    bool isPrivate() const { return (number >= 64198 && number <= 131071) || (number >= 4200000000 && number <= 4294967295); }
private:
public:
    static size_t mask;
    static size_t len;
    size_t number;
};

// represent an AS set, which consists of a list of single ASes
class ASset : public std::set<AS> {
public:
    typedef std::set<AS>::iterator iterator;
    ASset(const std::string &line);
    void Accept(ASVisitor& v) const;
};

// represent multi-origin-AS, which consits of a list of ASset of AS
class MOAS : public std::set<ASset> {
public:
    typedef std::set<ASset>::iterator iterator;
    explicit MOAS(const std::string &line);
    explicit MOAS() {}
    void parse(const std::string &line);
    std::string to_string();
    void Accept(ASVisitor& v);
    size_t getASNumber() const;
};

std::ostream &operator << (std::ostream& os, const MOAS &moas);

std::istream &operator >> (std::istream& is, MOAS &moas);

} // namespace measure
} // namespace wudanzy
#endif
