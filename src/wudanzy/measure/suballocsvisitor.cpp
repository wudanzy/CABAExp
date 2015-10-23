#include "suballocsvisitor.h"
#include "wudanzy/trie/trie.h"
#include "prefix.h"
#include "successive.h"
#include "allocation.h"

namespace wudanzy {
namespace measure {


void SuballocVisitor::visitSuballocMap(SuballocMap* map) {
    for (auto it = map->begin(); it != map->end(); ++it) {
        it->second.Accept(*this);
    }
}

// aggregate the prefixes appear at the same time
// caution: prefixes appear at different time will not be aggregated
void AggregateSuballocVisitor::visitSuballocSet(SuballocSet* set) {
    wudanzy::trie::BinaryTrie<unsigned> trie;
    for (auto it = set->begin(); it != set->end(); ++it) {
        trie.insert(it->first.second, it->first.first);
    }
    set->clear();
    trie.aggregate();
    for (wudanzy::trie::BinaryTrie<unsigned>::trieptr it(trie.preorder()); !it->isDone(); it->next()) {
        if (it->value()) {
            Suballoc alloc(*it->value(), it->path());
            SuballocAttribute attr;
            set->insert(make_pair(alloc, attr));
        }
    }
}

// Identify all fragments from all suballocs
void FragmentIdentifyVisitor::visitSuballocSet(SuballocSet* set) {
    wudanzy::trie::BinaryTrie<unsigned> trie;
    for (auto it = set->begin(); it != set->end(); ++it) {
        // if a general prefix is already inserted, the prefix can be absorbed
        wudanzy::trie::BinaryTrie<unsigned>::Node *tnode = trie.search(it->first.second, false);
        it->second.isAbsorbed = false;
        if (tnode) {
            for (wudanzy::trie::AbstractNode<unsigned>::nodeptr nt = tnode->ancestors(); !nt->isDone(); nt->next()) {
                if (nt->currentItem()->value()) {
                    it->second.isAbsorbed = true; 
                }
            }
        }
        // buggy !
        // judge whether the prefix is successive 
        // error: the definition of successive is still unclear
        std::string tmpbits = Prefix::subtract_one(it->first.second);
        tnode = trie.search(tmpbits, false);
        it->second.isSuccessive = false;
        if (tnode) {
            for (wudanzy::trie::AbstractNode<unsigned>::nodeptr nt = tnode->ancestors(); !nt->isDone(); nt->next()) {
                if (nt->currentItem()->value()) {
                    it->second.isSuccessive = true; 
                }
            }
        }
        // whether the prefix can be merged or absorbed
        // whether the prefix is a fragment
        wudanzy::trie::BinaryTrie<unsigned>::Node *node = trie.insertAndAggregate(it->first.second, 201305);
        if (node->value()) {
            it->second.isFragment = true;
        }
    }
    // set->begin()->second.setFragment(false);
}

void SuccessiveFragmentVisitor::visitSuballocSet(SuballocSet* set) {
    SuballocSet::iterator prev = set->begin();
    // static int cnt = 0;
    // std::cout << "visit Suballoc #" << ++cnt << std::endl; 
    for (auto it = set->begin(); it != set->end(); prev = it++) {
        // std::cout << "for clause" << std::endl;
        if (prev != it && prev->second.isFragment && it->second.isFragment && Prefix::bitsBegin(it->first.second)==Prefix::bitsEnd(prev->first.second)) {
            prev->second.isSuccessiveFragment = it->second.isSuccessiveFragment = true;
        }
    }
} 

void SuccessiveFragmentIdentifier::visitSuballocMap(SuballocMap* map) {
    for (auto it = map->begin(); it != map->end(); ++it) {
        moas = it->first;
        it->second.Accept(*this);
    }
}
void SuccessiveFragmentIdentifier::visitSuballocSet(SuballocSet* set) {
    SuballocSet::iterator prev = set->begin();
    SuballocSet one;
    for (auto it = set->begin(); it != set->end(); prev = it++) {
        // std::cout << "for clause" << std::endl;
        if (it->second.isSuccessiveFragment) {
            if (prev != it && prev->second.isSuccessiveFragment && Prefix::bitsBegin(it->first.second)==Prefix::bitsEnd(prev->first.second)) {
                one.insert(*it);    
            } else {
                if (one.size()) {
                    this->set->insert(std::make_pair(moas, one));
                    // (*this->set)[moas] = one;
                    one.clear();
                }
                one.insert(*it);
            }
        }
    }
    if (one.size()) {
        this->set->insert(std::make_pair(moas, one));
    }

} 

void MatchAllocationVisitor::visitSuballocSet(SuballocSet* set) {
    for (auto it = set->begin(); it != set->end(); ++it) {
        AllocTrie::Node* node = trie->search(it->first.second, false);
        AllocTrie::Node* tnode = node;
        while (node&&node->value()==NULL) node = node->parent();
        if (node&&node->value()) {
            it->second.allocs.push_back(node->value());
        } else {
            for (AllocTrie::trieptr rt = tnode->preorder(); !rt->isDone(); rt->next()) {
                if (rt->value()) {
                    it->second.allocs.push_back(rt->value());
                }
            }
        }
    }
} 

void OneSuballocOneLinePrinter::visitSuballocMap(SuballocMap* map) {
    for (auto it = map->begin(); it != map->end(); ++it) {
        pMoas = &it->first;
        it->second.Accept(*this);
    }
}

void OneSuballocOneLinePrinter::visitSuballocSet(SuballocSet* set) {
    for (auto sub : *set) {
        os << *pMoas << ' ' << sub.first << ' ' << sub.second << std::endl; 
    }
}

} // namespace measure
} // namespace wudanzy
