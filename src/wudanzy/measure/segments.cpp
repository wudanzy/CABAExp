#include <algorithm>
#include <memory>
#include "graph.h"
#include "segments.h"
#include "suballocs.h"
#include "prefix.h"
#include "allocation.h"
#include "wudanzy/trie/trie.h"

namespace wudanzy {
namespace measure {

Segment::Segment(MOAS as, unsigned key, std::string bits): std::pair<MOAS, Suballoc>(as, Suballoc(key, bits)), 
    as(first), time(second.first), bits(second.second) {
}

Segment::Segment(const Segment& seg): std::pair<MOAS, Suballoc>(seg.as, Suballoc(seg.time, seg.bits)),
    as(first), time(second.first), bits(second.second) {
}

Segment& Segment::operator = (const Segment& other) {
    as = other.as;
    time = other.time;
    bits = other.bits;
    return *this;
}

bool Segment::operator < (const Segment& other) {
    return first < other.first || (first == other.first && second < other.second);
}

std::string Segment::to_string(const char sep) const {
    std::string res;
    res.append(as.to_string());
    res.push_back(sep);
    res.append(std::to_string(time));
    res.push_back(sep);
    res.append(wudanzy::measure::Suballoc::bIPv6 ? Prefix::prefix6(bits) : Prefix::prefix(bits));
    return res;
}


// member functions of class SegmentVector
// 
bool SegmentVector::compareByTime(const SegmentPair& p1, const SegmentPair& p2) {
    return p1.first.second.first < p2.first.second.first || 
        (p1.first.second.first == p2.first.second.first && p1.first.second.second < p2.first.second.second) ||
        (p1.first.second == p2.first.second && p1.first.first < p2.first.first);
}

bool SegmentVector::compareByAS(const SegmentPair& p1, const SegmentPair& p2) {
    if (p1.first.first < p2.first.first) 
        return true;
    else if (p1.first.first == p2.first.first) {
        if (p1.first.second.first < p2.first.second.first)
            return true;
        else if (p1.first.second.first == p2.first.second.first) {
            if (p1.first.second.second < p2.first.second.second)
                return true;
        }
    }
    return false;
}

bool SegmentVector::compareByIP(const SegmentPair& p1, const SegmentPair& p2) {
    if (p1.first.bits < p2.first.bits) 
        return true;
    else if (p1.first.bits == p2.first.bits) {
        if (p1.first.as < p2.first.as)
            return true;
        else if (p1.first.as == p2.first.as) {
            if (p1.first.time < p2.first.time)
                return true;
        }
    }
    return false;
}
void SegmentVector::push_back(SegmentPair& pair) {
    base_type::push_back(pair);
    Notify();
}

void SegmentVector::Attach(SegmentVectorObserver *observer) {
    observers.push_back(observer);
    Notify();
}

void SegmentVector::Detach(SegmentVectorObserver *observer) {
    observers.remove(observer);
}

void SegmentVector::Notify() {
    for (auto observer : observers) {
        observer->Update(*this);
    }
}

// member functions of SegmentFactories

Segment* DynamicAlloatedSegmentFactory::getSegment(MOAS as ,unsigned key, std::string bits) {
    auto pair = make_pair(as, bits);
    if (map.count(pair) == 0) {
        map[pair] = std::unique_ptr<Segment>(new Segment(as, key, bits)); 
    }
    return map[pair].get();
}

SegmentVectorSynthesizedFactory::SegmentVectorSynthesizedFactory(SegmentVector &v): numberOfValidSegments(0), vector(v) {
    vector.Attach(this);
}

Segment* SegmentVectorSynthesizedFactory::getSegment(MOAS as, unsigned key, std::string bits) {
    auto pair = make_pair(as, bits);
    if (map.count(pair) == 0) {
        std::string errorString = std::string("SegmentVectorSynthesizedFactory::getSegment prefix not found");
        errorString += wudanzy::measure::Suballoc::bIPv6 ? Prefix::prefix6(bits) : Prefix::prefix(bits);
        throw std::logic_error(errorString);
    }
    // std::cout << "getSegment: " <<  vector[map[bits]].first << std::endl;
    return map[pair].get();
}

void SegmentVectorSynthesizedFactory::Update(SegmentVector &vector) {
    // std::cout << "SegmentVectorSynthesizedFactory::Updating" << std::endl;
    size_t size = vector.size();
    while (numberOfValidSegments < size) {
        auto pair = make_pair(vector[numberOfValidSegments].first.as, vector[numberOfValidSegments].first.bits);
        // std::cout << "Addone" << std::endl;
        // std::cout << "Update: " << vector[numberOfValidSegments].first << std::endl;
        map[pair] = std::unique_ptr<Segment>(new Segment(vector[numberOfValidSegments].first));
        ++numberOfValidSegments;
    }
}

std::ostream &operator << (std::ostream& os, const Segment &seg) {
    // os << &seg << ' ';
    os << seg.first << ' ' << seg.second;
    return os;
}

std::ostream &operator << (std::ostream& os, const SegmentMap &map) {
    for (auto p : map) {
        os << p << std::endl;
        // os << p.first << ' ' << p.second << std::endl;
    }
    return os;    
}

std::ostream &operator << (std::ostream& os, const SegmentPair &pair) {
    os << pair.first << ' ' << pair.second;
    return os;
}

std::ostream &operator << (std::ostream& os, const SegmentVector &vector) {
    for (auto p : vector) {
        os << p << std::endl;
    }
    return os;    
}

std::istream &operator >> (std::istream& is, Segment &seg) {
    // std::cout << seg.first << ' ' << seg.second << std::endl;
    is >> seg.first >> seg.second;
    // std::cout << seg.first << ' ' << seg.second << std::endl;
    return is;
}

std::istream &operator >> (std::istream& is, SegmentMap &map) {
    std::string line;
    while (std::getline(is, line)) {
        std::istringstream record(line);
        Segment seg;
        record >> seg; 
        record >> map[seg];
    }
    return is;
}

std::istream &operator >> (std::istream& is, SegmentVector &vector) {
    std::string line;
    while (std::getline(is, line)) {
        std::istringstream record(line);
        SegmentPair pair;
        record >> pair.first >> pair.second;
        vector.push_back(pair);
    }
    return is;
}

void SegmentVisitor::visitSegmentMap(SegmentMap &map) {
    for (auto it = map.begin(); it != map.end(); ++it) {
        visitSegment(it->first, it->second);
    }
}

void SegmentVisitor::visitSegmentVector(SegmentVector &v) {
    for (auto it = v.begin(); it != v.end(); ++it) {
        visitSegment(it->first, it->second);
    }
}

void SegmentIdentifier::visitSegmentVector(SegmentVector &v) {
    std::sort(v.begin(), v.end(), SegmentVector::compareByIP);
    SegmentVisitor::visitSegmentVector(v);
}

void SegmentIdentifier::visitSegment(const Segment& segment, SuballocAttribute& attribute) {
    // if a general prefix is already inserted, the prefix can be absorbed
    wudanzy::trie::BinaryTrie<size_t>::Node *tnode = trie.search(segment.bits, false);
    attribute.isCovered = false;
    if (tnode) {
        for (wudanzy::trie::AbstractNode<size_t>::nodeptr nt = tnode->ancestors(); !nt->isDone(); nt->next()) {
            if (nt->currentItem()->value()) {
                attribute.isCovered = true; 
            }
        }
    }
    trie.insert(segment.bits, 201503);
    // std::cout << "visiting " << segment.as << ' ' << segment.time << ' ' << segment.bits << ' ' << attribute << std::endl;
    // wudanzy::trie::BinaryTrie<size_t>::Node *node = trie.insertAndAggregate(segment.bits, 201503);
    // if (!node->value()) {
        // attribute.isCovered = true;
    // }
}

void BrotherSegmentFinder::visitSegmentVector(SegmentVector& v) {
    std::sort(v.begin(), v.end(), SegmentVector::compareByIP); 
    // std::sort(v.begin(), v.end(), SegmentVector::compareByTime); 
    SegmentVisitor::visitSegmentVector(v);
}

void BrotherSegmentFinder::visitSegment(const Segment& segment, SuballocAttribute& attribute) {
    std::string bits(Prefix::brother_bits(segment.bits));
    trie.insert(segment.bits, &segment);
    wudanzy::trie::BinaryTrie<const Segment*>::Node *tnode = trie.search(bits);
    if (tnode) {
        for (wudanzy::trie::AbstractNode<const Segment*>::trieptr rt = tnode->preorder(); !rt->isDone(); rt->next()) {
            if (rt->value()) {
                attribute.segments.push_back(*rt->value());
                RelationGraph::path_type path = graph.shortestPathFromDB(segment.as, (*rt->value())->as);  
                // std::cout << graph.getStringFromPath(path) << std::endl;
                attribute.distances.push_back(graph.getStringFromPath(path));
            }
        }
    }
}

static size_t getDistanceFromString(std::string str) {
    for (auto &ch : str) {
        if (!isdigit(ch)) ch = ' ';
    }
    size_t num, cnt=0;
    std::istringstream record(str);
    while (record >> num) {
        ++cnt;
    }
    return cnt - 1;
}

static double getWeight(std::string base_bits, std::string bits) {
    size_t diff = bits.size() - base_bits.size();
    double weight = 1.0;
    while (diff > 0) {
        weight /= 2;
        --diff;
    }
    return weight;
}

void BrotherDistanceCalculator::visitSegment(const Segment& segment, SuballocAttribute& attribute) {
    attribute.brotherDistance = 0.0;
    size_t cnt = 0;
    if (calc == calc_unweighted_average) {
        for (size_t i = 0; i < attribute.segments.size(); ++i ) {
            attribute.brotherDistance += getDistanceFromString(attribute.distances[i]); 
            ++cnt;
        } 
        if (cnt > 0)
            attribute.brotherDistance /= cnt;
    } else if (calc == calc_weighted_average) {
        double weights = 0.0;
        for (size_t i = 0; i < attribute.segments.size(); ++i ) {
            double tmp_weight = getWeight(segment.bits, attribute.segments[i]->bits);
            attribute.brotherDistance += tmp_weight * getDistanceFromString(attribute.distances[i]); 
            weights += tmp_weight;
            ++cnt;
        } 
        if (cnt > 0)
            attribute.brotherDistance /= weights;
    }
    if (attribute.segments.size() == 0) {
        attribute.brotherPrefixType = SuballocAttribute::bp_unassigned;
    } else if (attribute.segments.size() == 1 && attribute.segments[0]->bits == Prefix::brother_bits(segment.bits)) {
            attribute.brotherPrefixType = SuballocAttribute::bp_exactly_one;
    } else {
        wudanzy::trie::BinaryTrie<size_t> trie;
        for (auto pSeg : attribute.segments) {
            trie.insertAndAggregate(pSeg->bits, 201503);
        }
        std::string brother_bits = Prefix::brother_bits(segment.bits);
        auto node = trie.search(brother_bits);
        if (node->value()) {
            attribute.brotherPrefixType = SuballocAttribute::bp_without_hole;
        } else {
            attribute.brotherPrefixType = SuballocAttribute::bp_with_hole;
        }
    }
}

void MatchingAllocationFinder::visitSegment(const Segment& segment, SuballocAttribute& attribute) {
    AllocTrie::Node* node = trie.search(segment.bits, false);
    AllocTrie::Node* tnode = node;
    while (node&&node->value()==NULL) node = node->parent();
    if (node&&node->value()) {
        attribute.allocs.push_back(node->value());
    } else {
        for (AllocTrie::trieptr rt = tnode->preorder(); !rt->isDone(); rt->next()) {
            if (rt->value()) {
                attribute.allocs.push_back(rt->value());
            }
        }
    }
}


} // namespace measure
} // namespace wudanzy
