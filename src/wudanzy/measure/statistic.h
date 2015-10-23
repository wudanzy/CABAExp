#ifndef __STATISTIC_H__
#define __STATISTIC_H__
#include <map>

namespace wudanzy {
namespace measure {

template<typename KEY, typename VALUE>
class Distribution : public std::map<KEY, VALUE> {
public:
    Distribution() {}
    virtual ~Distribution() {}
};

// template<typename KEY, typename VALUE>
// std::ostream &operator << (std::ostream& os, const Distribution<KEY,VALUE> &dist);

template<typename KEY, typename VALUE>
class CumulatedDistribution : public Distribution<KEY,VALUE> {
public:
    CumulatedDistribution() {}
    void load(Distribution<KEY,VALUE> &dist);
};

template<typename KEY, typename VALUE>
std::ostream &operator << (std::ostream& os, const Distribution<KEY, VALUE> &dist) {
    for (auto it = dist.begin(); it != dist.end(); ++it) {
        os << it->first << ' ' << it->second << std::endl;
    }
    return os;
}

template<typename KEY, typename VALUE>
void CumulatedDistribution<KEY,VALUE>::load(Distribution<KEY, VALUE> &dist) {
    if (dist.size()) {
        auto it = dist.begin();
        this->insert(*it++);
        VALUE v = dist.begin()->second;
        for (; it != dist.end(); ++it) {
            v += it->second;
            this->insert(std::make_pair(it->first, v));
        }
    }
}

// template<>
// void CumulatedDistribution<unsigned, unsigned>::load(Distribution<unsigned, unsigned> &dist) {
    // if (dist.size()) {
        // auto it = dist.begin();
        // this->insert(*it++);
        // unsigned v = dist.begin()->second;
        // for (; it != dist.end(); ++it) {
            // v += it->second;
            // this->insert(std::make_pair(it->first, v));
        // }
    // }
// }

} // namespace measure
} // namespace wudanzy
#endif
