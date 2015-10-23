#include "allocation.h"

namespace wudanzy {
namespace measure {

Alloc::Alloc() :rir(static_cast<rir_type>(0)),ac(static_cast<ac_type>(0)),ipType(static_cast<ip_type>(0)),range(0),time(0) {
    memset(country,0,sizeof(country)); 
}

Alloc::Alloc(const Alloc& al):rir(al.rir),ac(al.ac),ipType(al.ipType),ip(al.ip),range(al.range),time(al.time) { 
    country[0]=al.country[0]; country[1]=al.country[1]; 
}

Alloc::Alloc(std::string &line)
    :rir(static_cast<rir_type>(0)),ac(static_cast<ac_type>(0)),ipType(static_cast<ip_type>(0)),range(0),time(0) {
    parse(line);
}

void Alloc::parse(std::string &line) {
    memset(country,0,sizeof(country)); 
    as.clear();
    unsigned cnt = 0;
    std::string::const_iterator prev = line.begin(), end = line.end();
    for (std::string::const_iterator ci = prev; ci != end; ++ci) {
        if (*ci == '|' or *ci == ' ') {
            ++cnt;
            std::string temp(prev, ci);
            assign(temp, cnt);
            // cout << '*' << temp << '*';
            prev = ci + 1;
        } 
    }
    if (prev < end) {
        ++cnt;
        std::string temp(prev, end);
        assign(temp, cnt);
    }
}

std::string Alloc::to_string(const char sep) const {
    std::string res;
    res.append(Alloc::rirs[rir]);
    res.push_back(sep);
    res.push_back(Alloc::country[0]);
    res.push_back(Alloc::country[1]);
    res.push_back(sep);
    res.append(Alloc::iptypes[ipType]);
    res.push_back(sep);
    if (as.size()) {
        res.append(as);
        res.push_back(sep);
    }
    res.append(ip);
    res.push_back(sep);
    res.append(std::to_string(range));
    res.push_back(sep);
    res.append(std::to_string(time));
    res.push_back(sep);
    res.append(Alloc::allocs[ac]);
    return res;
}

void Alloc::assign(std::string &val, unsigned index) {
    switch (index) {
        case 1: // rir_type;
            if (rmap.count(val)) {
                rir = rmap[val];
            } else {
                std::string errorString = "Alloc::assign error rir_type:";
                errorString += val;
                throw std::runtime_error(errorString);
            }
            break;
        case 2: // country_code;
            if (val.size()==2) {
                country[0]=val[0];
                country[1]=val[1];
            }
            break;
        case 3: // ipv4, ipv6 or asn
            if (pmap.count(val)) {
                ipType = pmap[val];
            } else {
                std::string errorString = "Alloc::assign error ip_type:";
                errorString += val;
                throw std::runtime_error(errorString);
            }
            break;
        case 4: // prefix
            ip = val;
            break;
        case 5: // value
            try {
                if (val.size()) range = std::stoul(val);
            } catch (const std::exception& e) {
                std::string errorString = "Alloc::assign error range:";
                errorString += val;
                throw std::runtime_error(errorString);
            }
            break;
        case 6: // year
            try {
                if (val.size()) time = std::stoul(val);
            } catch (const std::exception& e) {
                std::string errorString = "Alloc::assign error time:";
                errorString += val;
                throw std::runtime_error(errorString);
            }
            break;
        case 7: // alloc_type;
            if (amap.count(val)) {
                ac = amap[val];
            } else {
                std::string errorString = "Alloc::assign error ac_type:";
                errorString += val;
                throw std::runtime_error(errorString);
            }
            break;
        default:
            break;
    }
}

const std::vector<std::string> Alloc::rirs = {"uninitialized","afrinic", "apnic", "arin", "lacnic", "ripencc"};
const std::vector<std::string> Alloc::allocs = {"uninitialized","allocated", "assigned", "available", "reserved"};
const std::vector<std::string> Alloc::iptypes = {"uninitialized","ipv4", "ipv6", "asn"};
std::map<std::string, Alloc::rir_type> Alloc::rmap;
std::map<std::string, Alloc::ac_type> Alloc::amap;
std::map<std::string, Alloc::ip_type> Alloc::pmap;
bool Alloc::init_ = Alloc::init();

std::ostream &operator << (std::ostream& os, const Alloc &alloc) {
    // os << alloc.time << '|' << alloc.as << '|' << alloc.ip << '|' << alloc.range  << '|' << Alloc::rirs[alloc.rir] << '|' << Alloc::allocs[alloc.ac];
    // os << alloc.time << '|' << alloc.as << '|' << alloc.ip << '|' << alloc.range;
    os << alloc.to_string('|');
    return os;
}

bool operator == (const Alloc& one, const Alloc& other) {
    return one.rir == other.rir && one.ac == other.ac && one.country[0] == other.country[0] && 
        one.country[1] == other.country[1] && one.ip == other.ip && one.ipType == other.ipType &&
        one.range == other.range && one.time == other.time && one.as == other.as;  
}

void AllocTrie::load(std::istream &in) {
    std::string line;
    while (getline(in, line)) {
        Alloc alloc(line);
        if (alloc.ipType == Alloc::ip_v4 ) {
            insert(Prefix::ip_to_num(alloc.ip), alloc.range, alloc);
        } else if (alloc.ipType == Alloc::ip_v6) {
            insert(Prefix::bits6(alloc.ip + "/" + std::to_string(alloc.range)), alloc);
        } else {
            throw std::logic_error("unsupported ipType");
        }
    }
}

bool operator == (const Rib &r, const Rib &b) {
    return r.as==b.as&&r.time==b.time;
}

std::ostream &operator << (std::ostream& os, const Rib &rib) {
    os << /*Prefix::prefix(rib.bits) << '|' <<*/ rib.as << '|' << rib.time;
    return os;
}

void RibTrie::load(std::ifstream &in) {
    std::string line;
    while (getline(in, line)) {
        // std::cout << line << std::endl;
        Rib rib(line);
        insert(rib.bits,rib);
    }
}

void assignTime(RibTrie &rib, AllocTrie &alloc) {
    for (RibTrie::trieptr it(rib.preorder()); !it->isDone(); it->next()) {
        if (it->value()) {
            std::string bits(it->currentItem()->bits());
            // out << Prefix::prefix(bits) << '|' << *it->currentItem()->value();
            // std::cout << "ribs:" << Prefix::prefix(bits) << std::endl;
            // rib.deaggregate(bits.begin(), bits.end());
            bool hasAncestors = false;
            // RibTrie::Node *node = trie.search(bits);
            AllocTrie::Node *node = alloc.search(bits, false);
            // for (AllocTrie::Node::AncestorIterator rt = node->ancestorsBegin(); rt != node->ancestorsEnd(); ++rt) {
            for (AllocTrie::Node::nodeptr rt(node->ancestors()); !rt->isDone(); rt->next()) {
                AllocTrie::Node* tnode = rt->currentItem();
                // Allo
                if (tnode->value()) {
                // if (rt->value()) {
                    hasAncestors = true;
                    Rib *value = it->currentItem()->value();
                    value->time = rt->currentItem()->value()->time;
                    // printNode(trie,out,tnode);
                    // out << '|' << Prefix::prefix(tnode->bits()) << '|' << *tnode->value();
                    // ++cnt;
                }
            }
            if (!hasAncestors) {
                if (node->bits()==bits) {
                    for (AllocTrie::Node::trieptr rt(node->preorder()); !rt->isDone(); rt->next()) {
                        AllocTrie::Node* tnode = rt->currentItem();
                        if (tnode->value()) {
                            std::string temp_bits = rt->currentItem()->bits();
                            rib.deaggregate(temp_bits.begin(), temp_bits.end());
                            // out << '|' << Prefix::prefix(tnode->bits()) << '|' << *tnode->value();
                            // printNode(trie,out,tnode);
                        }
                    }
                } //else
                    // out << '|' << "unmatched";
            }
            // out << std::endl;
        }
    }
}

} // namespace measure
} // namespace wudanzy
