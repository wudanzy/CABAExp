#ifndef __ALLOCATION_H__
#define __ALLOCATION_H__
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "wudanzy/trie/trie.h"
#include "wudanzy/trie/node.h"
#include "wudanzy/trie/iterator.h"
#include "wudanzy/trie/utility.h"
#include "prefix.h"

namespace wudanzy {
namespace measure {

struct Alloc {
    // non-static members
    enum rir_type {
        rir_uninitialized,
        rir_afrinic,
        rir_apnic,
        rir_arin,
        rir_lacnic,
        rir_ripencc,
        rir_count
    } rir;
    enum ac_type {
        ac_uninitialized,
        ac_allocated,
        ac_assigned,
        ac_available,
        ac_reserved,
        ac_count
    } ac;
    enum ip_type {
        ip_uninitialized,
        ip_v4,
        ip_v6,
        ip_asn,
        ip_count
    } ipType;
    char country[2];
    std::string ip;
    unsigned range;
    unsigned time;
    std::string as;
    // constructors
    explicit Alloc();
    explicit Alloc(const Alloc& al);
    explicit Alloc(std::string &line);
    // non-static member functions
    void parse(std::string &line);
    std::string to_string(const char sep = '|') const;
    // static member functions
    static const std::vector<std::string> rirs;
    static const std::vector<std::string> allocs;
    static const std::vector<std::string> iptypes;
    static std::map<std::string, rir_type> rmap;
    static std::map<std::string, ac_type> amap;
    static std::map<std::string, ip_type> pmap;
    static bool init_;
    static bool init() {
        int cnt = 0;
        for (std::vector<std::string>::const_iterator cit = rirs.begin(); cit != rirs.end(); ++cit) {
             rmap[*cit] = static_cast<rir_type>(cnt++);
        }
        cnt = 0;
        for (std::vector<std::string>::const_iterator cit = allocs.begin(); cit != allocs.end(); ++cit) {
             amap[*cit] = static_cast<ac_type>(cnt++);
        }
        cnt = 0;
        for (std::vector<std::string>::const_iterator cit = iptypes.begin(); cit != iptypes.end(); ++cit) {
             pmap[*cit] = static_cast<ip_type>(cnt++);
        }
        return true;
    }
private:
    void assign(std::string &val, unsigned index);
};

std::ostream &operator << (std::ostream& os, const Alloc &alloc);
bool operator == (const Alloc& one, const Alloc& other);

class AllocTrie: public wudanzy::trie::BinaryTrie<Alloc> {
public:
    AllocTrie(): BinaryTrie<Alloc>() {}
    void load(std::istream &in);
};

struct Rib {
    std::string as;
    std::string bits;
    unsigned time;
    Rib():time(0) {/*std::cout << "default constructor" << std::endl;*/}
    Rib(const Rib &r):as(r.as), bits(r.bits), time(r.time) {/*std::cout << "copy constructor" << std::endl;*/}
    explicit Rib(std::string &line):time(0) { 
        // std::cout << "string constructor" << std::endl;
        // std::cout << line << std::endl;
        for (std::string::iterator it = line.begin(); it != line.end(); ++it) {
            if (*it == '|' || *it == '/') *it = ' ';
        }
        // std::cout << line << std::endl;
        std::string pfx;
        unsigned len;
        std::istringstream record(line); 
        record >> pfx >> len >> as >> time;
        pfx+="/"+std::to_string(len);
        bits = Prefix::bits(pfx);
        // std::cout << line << std::endl;
        // std::cout << line << '|' <<  pfx << ' ' << as << ' ' << time << std::endl;
    }
};

bool operator == (const Rib &r, const Rib &b);

std::ostream &operator << (std::ostream& os, const Rib &rib);

class RibTrie: public wudanzy::trie::BinaryTrie<Rib> {
public:
    RibTrie(): BinaryTrie<Rib>() {}
    void load(std::ifstream &in);
};

} // namespace measure
} // namespace wudanzy
#endif
