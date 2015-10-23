#include "tmapvisitor.h"
#include <climits>
#include <iostream>
#include "tmap.h"
#include "allocation.h"
#include "header.h"
namespace wudanzy {
namespace measure {

void TimeVisitor::visitTimeTrie(TimeTrie *trie) {
    for (TimeTrie::trieptr it = trie->preorder(); !it->isDone(); it->next()) {
        if (it->value()) {
            it->value()->Accept(*this);
        }
    }
}

void TimeMapCleaner::cleanByTime(TimeMap* tmap, const size_t startTime) {
    for (TimeMap::iterator rt = tmap->begin(); rt != tmap->end();) {
        TimeMap::times &v = rt->second;
        while (v.size()>0&&*v.begin() < startTime) v.erase(v.begin());
        if (v.size()==0) {
            tmap->erase(rt++);
        } else { ++rt; }
    }
}

void TimeMapCleaner::visitTimeMap(TimeMap* tmap) {
    wudanzy::measure::AllocTrie::Node* node = allocTrie.search(bits, false);
    wudanzy::measure::AllocTrie::Node* tnode = node;
    while (node&&node->value()==NULL) node = node->parent();
    if (node&&node->value())
    {
        unsigned alloctime = node->value()->time;
        alloctime /= 100;
        cleanByTime(tmap, alloctime);
    } else if (tnode->bits() == bits) {
        size_t time = std::numeric_limits<size_t>::max();
        for (auto rt = tnode->preorder(); !rt->isDone(); rt->next()) {
            if (rt->value()) {
                Alloc *pAlloc = rt->value();
                if (pAlloc->ac == Alloc::ac_assigned || pAlloc->ac == Alloc::ac_allocated) {
                    if (time > pAlloc->time) {
                        time = pAlloc->time;
                    }
                }
            }
        }
        cleanByTime(tmap, time/100);
    } else {
        cleanByTime(tmap, std::numeric_limits<size_t>::max());
    }
}

void TimeMapCleaner::visitTimeTrie(TimeTrie *trie) {
    for (TimeTrie::trieptr it(trie->preorder()); !it->isDone(); it->next()) {
        if (it->value()) {
            bits = it->path();
            it->value()->Accept(*this);
            if (it->value()->size()==0)
            {
                it->currentItem()->setValue(NULL);
            }
        }
    }
}

void CleanTimeVisitor::visitTimeMap(TimeMap* tmap) {
    for (TimeMap::iterator rt = tmap->begin(); rt != tmap->end();) {
        TimeMap::times &v = rt->second;
        // while ( v.size()>0&&v[0]<time) v.erase(v.begin());
        while (v.size()>0&&*v.begin() < time) v.erase(v.begin());
        if (v.size()==0) {
            tmap->erase(rt++);
        } else { ++rt; }
    }
}

void ParseTimeVisitor::visitTimeMap(TimeMap* tmap) {
    for (TimeMap::iterator it = tmap->begin(); it != tmap->end();) {
        MOAS moas(it->first);
        moas.Accept(absorber);
        if (moas != it->first) {
            // std::cout << "orignal " << it->first << " parsed " << parsedAS << std::endl;
            (*tmap)[moas].insert(it->second.begin(), it->second.end());
            tmap->erase(it++);
        } else {
            ++it;
        }
    }
}

void ClassifyTimeVisitor::visitTimeMap(TimeMap* tmap) {
    for (TimeMap::iterator it = tmap->begin(); it != tmap->end();) {
        // if it is a moas
        if (it->first.size() != 1) {
            for (MOAS::iterator jt = it->first.begin(); jt != it->first.end(); ++jt) {
                MOAS tmp;
                tmp.insert(*jt);
                (*tmap)[tmp].insert(it->second.begin(), it->second.end());
            }
            tmap->erase(it++);
        } else {
            ++it;
        }
    }
}

void ConvertSuballocTimeVisitor::visitTimeTrie(TimeTrie *trie) {
    for (TimeTrie::trieptr it(trie->preorder()); !it->isDone(); it->next()) {
        if (it->value()) {
            // prefix = Prefix::prefix(it->path());
            prefix = it->path();
            it->value()->Accept(*this);
        }
    }
}

void ConvertSuballocTimeVisitor::visitTimeMap(TimeMap* tmap) {
    for (TimeMap::iterator it = tmap->begin(); it != tmap->end(); ++it) {
        if (it->second.count(time)) {
            // std::cout << "Get one:" << it->first << ' ' << prefix << ' ' << *it->second.begin() << std::endl;
            // smap[it->first][prefix] = *it->second.begin();
            smap[it->first].insert(make_pair(Suballoc(*it->second.begin(), prefix),SuballocAttribute()));
        }
    }
}

} // namespace measure
} // namespace wudanzy
