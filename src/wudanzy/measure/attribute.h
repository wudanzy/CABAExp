#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__
#include <vector>
#include <string>
#include <istream>
#include <ostream>
namespace wudanzy {
namespace measure {
// forward declarations
struct Alloc;
class Segment;
class SegmentFactory;

// contains all attributes, used to label the suballocs
struct SuballocAttribute
{
    enum bp_type {
        bp_uninitialized,
        bp_unassigned,
        bp_exactly_one,
        bp_with_hole,
        bp_without_hole,
        bp_count
    };
    SuballocAttribute(): isFragment(false), isSuccessiveFragment(false), isAbsorbed(false), isSuccessive(false), isCovered(false), brotherDistance(0.0) {}
    std::string to_string();
    void parse(std::string str, SegmentFactory &segmentFactory);
    void parseSegments(std::string str, SegmentFactory &segmentFactory);

    bool isFragment, isSuccessiveFragment;
    bool isAbsorbed;
    bool isSuccessive;
    bool isCovered;
    double brotherDistance;
    bp_type brotherPrefixType;

    std::vector<Alloc*> allocs; 
    std::vector<const Segment*> segments; 
    std::vector<std::string> distances; 
    static SegmentFactory *segmentFactory;
    static const std::vector<std::string> brotherPrefixTypeNames;
};

std::ostream &operator << (std::ostream& os, const SuballocAttribute &attribute);

std::istream &operator >> (std::istream& is, SuballocAttribute &attribute);

bool operator == (const SuballocAttribute &a, const SuballocAttribute &b);

} // namespace measure
} // namespace wudanzy
#endif
