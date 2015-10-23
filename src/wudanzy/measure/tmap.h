#ifndef __TIME_MAP_H__
#define __TIME_MAP_H__
#include <iostream>
#include <set>
#include <locale>
#include "tmapvisitor.h"
#include "wudanzy/trie/trie.h"

namespace wudanzy {
namespace measure {

class TimeVisitor;

// typedef std::map<std::string, std::vector<unsigned>> TimeMap;
// class TimeMap : public std::map<std::string, std::set<unsigned>, Compare> {
class TimeMap : public std::map<MOAS, std::set<unsigned>> {
// class TimeMap : public std::map<std::string, std::vector<unsigned>> {
public:
    typedef std::set<unsigned> times;
    friend std::ostream &operator << (std::ostream& os, const TimeMap &map);
    friend class CleanTimeVisitor;
    friend class ParseTimeVisitor;
    void Accept(TimeVisitor &v);
private:
    // typedef std::map<std::string, times> base_type;
    typedef std::map<MOAS, std::set<unsigned>> base_type;
    //typedef std::map<std::string, std::vector<std::string>> moas_type;
    //moas_type moas;
    //void resetMoas();
    //std::string parseAS(const std::string&);
    // ASPrintVisitor printer;
    ASAbsorbVisitor absorber;
};

class TimeTrie: public wudanzy::trie::BinaryTrie<TimeMap> {
public:
    void Accept(TimeVisitor &v);
private:
    typedef wudanzy::trie::BinaryTrie<TimeMap> base_type;
};


std::ostream &operator << (std::ostream& os, const TimeMap &map);

std::istream &operator >> (std::istream& is, TimeMap &map);

} // namespace measure
} // namespace wudanzy

#endif
