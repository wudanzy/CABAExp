#include "attribute.h"
#include "allocation.h"
#include "segments.h"
#include <string>

namespace wudanzy {
namespace measure {

SegmentFactory *SuballocAttribute::segmentFactory = nullptr;
const std::vector<std::string> SuballocAttribute::brotherPrefixTypeNames = 
    {"uninitialized", "unassigned", "exactly_one", "with_hole", "without_hole"};

std::string SuballocAttribute::to_string() {
    std::string res;
    if (isFragment) res.push_back('*');
    if (isSuccessiveFragment) res.push_back('s');
    // if (isSuccessive) res.push_back('S');
    if (isAbsorbed) res.push_back('C');
    if (isCovered) res.push_back('c');
    if (allocs.size()) {
        res.push_back('[');
        size_t cnt = 0;
        for (auto it = allocs.begin(); it != allocs.end(); ++it) {
            if (cnt++) res.push_back('|');
            res.append((*it)->to_string(' '));
        }
        res.push_back(']');
    }
    if (segments.size()) {
        res.push_back('{');
        size_t cnt = 0;
        for (auto pSeg : segments) {
            if (cnt++) res.push_back('|');
            res.append(pSeg->to_string(' '));
            if (distances.size() >= cnt) {
                res.push_back('(');
                res.append(distances[cnt-1]);
                res.push_back(')');
            }
        }
        res.push_back('}');
    }
    return res;
}

void SuballocAttribute::parse(std::string str, SegmentFactory &segmentFactory) {
    for (size_t i = 0 ; i < str.size(); ++i) {
        switch (str[i]) {
            case '*':
                isFragment = true;
                break;
            case 's':
                isSuccessiveFragment = true;
                break;
            case 'S':
                isSuccessive = true;
                break;
            case 'C':
                isAbsorbed = true;
                break;
            case 'c':
                isCovered = true;
                break;
            case '{':
                size_t pos = str.find('}', i);
                if (pos == std::string::npos) {
                    throw std::runtime_error("SuballocAttribute::parse corresponding braces unfound");
                }
                parseSegments(str.substr(i+1, pos - i - 1), segmentFactory);
                i = pos;
                break;
        }
    }
}

void SuballocAttribute::parseSegments(std::string str, SegmentFactory &segmentFactory) {
    std::string::const_iterator prev = str.cbegin();
    for (std::string::const_iterator it = str.cbegin(); it != str.cend(); ++it) {
        if (*it == '|') {
            std::string tmpStr(prev, it);
            std::istringstream record(tmpStr);
            MOAS as;
            size_t key;
            std::string pfx;
            record >> as >> key >> pfx;
            size_t pos_parenthesis = pfx.find('(');
            if (pos_parenthesis != std::string::npos) {
                std::string distance = pfx.substr(pos_parenthesis + 1);
                distance.pop_back();
                distances.push_back(distance);
                pfx = pfx.substr(0, pos_parenthesis);
            }
            segments.push_back(segmentFactory.getSegment(as, key, Prefix::bits(pfx)));
            prev = it + 1;
        }
    }
    std::string tmpStr(prev, str.cend());
    std::istringstream record(tmpStr);
    MOAS as;
    size_t key;
    std::string pfx;
    record >> as >> key >> pfx;
    size_t pos_parenthesis = pfx.find('(');
    if (pos_parenthesis != std::string::npos) {
        std::string distance = pfx.substr(pos_parenthesis + 1);
        distance.pop_back();
        distances.push_back(distance);
        pfx = pfx.substr(0, pos_parenthesis);
    }
    segments.push_back(segmentFactory.getSegment(as, key, Prefix::bits(pfx)));
}

std::ostream &operator << (std::ostream& os, const SuballocAttribute &attribute) {
    if (attribute.isFragment) os << '*';
    if (attribute.isSuccessiveFragment) os << 's';
    // if (attribute.isSuccessive) os << 'S';
    if (attribute.isAbsorbed) os << 'C';
    if (attribute.isCovered) os << 'c';
    if (attribute.allocs.size()) {
        os << '[';
        size_t cnt = 0;
        for (auto it = attribute.allocs.begin(); it != attribute.allocs.end(); ++it) {
            if (cnt++) os << '|';
            os << **it;
        }
        os << ']';
    }
    if (attribute.segments.size()) {
        os << '{';
        // std::cout << "numberOfSegments" << attribute.segments.size();
        size_t cnt = 0;
        for (auto pSeg : attribute.segments) {
            if (cnt++) os << '|';
            if (pSeg == nullptr) {
                throw std::runtime_error("SuballocAttribute::operator << nullptr");
            }
            os << *pSeg;
            if (attribute.distances.size() >= cnt)
               os << '(' << attribute.distances[cnt-1] << ')';
        }
        os << '}';
    }
    return os;
}

bool operator == (const SuballocAttribute &a, const SuballocAttribute &b) {
    return a.isFragment == b.isFragment && a.isSuccessive == b.isSuccessive && a.isSuccessiveFragment == b.isSuccessiveFragment &&
        a.isAbsorbed == b.isAbsorbed && a.isCovered == b.isCovered && a.segments == b.segments && a.allocs == b.allocs && a.distances == b.distances;
}

std::istream &operator >> (std::istream& is, SuballocAttribute &attribute) {
    std::string flagbits;
    char ch;
    while (is.peek() == ' ') is.get(ch);
    if (getline(is, flagbits)) {
        for (size_t i = 0 ; i < flagbits.size(); ++i) {
            switch (flagbits[i]) {
                case '*':
                    attribute.isFragment = true;
                    break;
                case 's':
                    attribute.isSuccessiveFragment = true;
                    break;
                case 'S':
                    attribute.isSuccessive = true;
                    break;
                case 'C':
                    attribute.isAbsorbed = true;
                    break;
                case 'c':
                    attribute.isCovered = true;
                    break;
                case '{':
                    {
                        size_t pos = flagbits.find('}', i);
                        if (pos == std::string::npos) {
                            throw std::runtime_error("SuballocAttribute::parse corresponding braces unfound");
                        }
                        if (attribute.segmentFactory == nullptr) {
                            throw std::logic_error("SuballocAttribute::parse segmentFactory is unspecified");
                        }
                        attribute.parseSegments(flagbits.substr(i+1, pos - i - 1), *SuballocAttribute::segmentFactory);
                        i = pos;
                    }
                    break;
                default:
                    throw std::runtime_error("SuballocAttribute: unidentified attribute character." + flagbits);
            }
        } 
    }
    return is;
}

} // namespace measure
} // namespace wudanzy
