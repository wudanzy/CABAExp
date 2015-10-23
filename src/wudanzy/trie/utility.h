#ifndef __UTILITY_H__
#define __UTILITY_H__
#include <arpa/inet.h>
#include <iostream>
#include "prefix.h"
#include "wudanzy/trie/trie.h"

namespace wudanzy {
namespace trie {

template<typename T>
void printTrie(Trie<T> &trie, std::ostream &out, int family = AF_INET, const char sep = '|') {
    for (typename Trie<T>::trieptr it(trie.preorder());!it->isDone();it->next()) {
    // for (typename Trie<T>::iteratorptr it(trie.postorder());!it->isDone();it->next()) {
        AbstractNode<T>* node = it->currentItem();
        // if (node->bits()!=it->path()) std::cout << "bits is " << node->bits() << " and path is " << it->path() << " ." << std::endl;
        if (node&&node->value()) {
            // if (node->bits()!=it->path()) std::cout << "bits is " << node->bits() << " and path is " << it->path() << " ." << std::endl;
            int cnt=0;
            for (IteratorPtr<Iterator<AbstractNode<T>*>> rt(node->ancestors()); !rt->isDone(); rt->next()) {
                if (rt->currentItem()->value()) cnt++;
            }
            if (cnt)
                out << ((family == AF_INET6) ? Prefix::prefix6(it->path()) : Prefix::prefix(it->path())) << sep << *it->value() << std::endl;
        }
    }
}

template<typename T>
void printTrieCovered(Trie<T> &trie, std::ostream &out, const char sep = '|') {
    std::stack<AbstractNode<T>*> s;
    AbstractNode<T> *prev=NULL;
    for (IteratorPtr<AbstractNode<T>*> it(trie.preorder());!it->isDone();it->next()) {
        AbstractNode<T>* node = it->currentItem();
        while (!s.empty()&&s.top()!=node->parent()) { if (s.top()==prev) prev=NULL; s.pop(); } 
        s.push(node);
        if (node->value()) {
            if (prev) {
                out << Prefix::prefix(prev->bits()) << ' ' << *prev->value() << " covers " << Prefix::prefix(node->bits()) << ' ' << *node->value() << std::endl;
            } else {
                prev = node;
            }
        }
    }
}

template<typename T>
void printTrieNodeAncestors(AbstractNode<T> *node, std::ostream &out, const char sep = '|') {
    IteratorPtr<AbstractNode<T>*> it(node->ancestors());
    while (!it->isDone()&&!it->currentItem()->value()) it->next();
    if (!it->isDone()) {
        AbstractNode<T>* tnode = it->currentItem();
        out << Prefix::prefix(tnode->bits()) << sep << *tnode->value();
        it->next();
    }
    for (;!it->isDone();it->next()) {
        AbstractNode<T>* tnode = it->currentItem();
        if (tnode->value()) {
            out << sep <<  Prefix::prefix(tnode->bits()) << sep << *tnode->value();
        }
    }
    out << std::endl;
}

template<typename T>
void printTrieNodeDescendants(AbstractNode<T> *node, std::ostream &out, const char sep = '|') {
    IteratorPtr<AbstractNode<T>*> it(node->preorder());
    while (!it->isDone()&&!it->currentItem()->value()) it->next();
    if (!it->isDone()) {
        AbstractNode<T>* tnode = it->currentItem();
        out << Prefix::prefix(tnode->bits()) << sep << *tnode->value();
        it->next();
    }
    for (;!it->isDone();it->next()) {
        AbstractNode<T>* tnode = it->currentItem();
        if (tnode->value()) {
            out << sep <<  Prefix::prefix(tnode->bits()) << sep << *tnode->value();
        }
    }
    out << std::endl;
}

// void printBits(AllocTrie &trie, std::ostream &out, vector<string> bits) {
    // for (string bit: bits) {
        // AllocTrie::Node* node = trie.search(bit);
    // }
// }

// void printNode(AllocTrie &trie, std::ostream &out, AllocTrie::Node* node) {
    // Alloc* ac = node->value();
    // out << '|' << Prefix::prefix(node->bits()) << '|' << *ac;
// }

template<typename T, typename R>
void matchRibs(Trie<R> &rib, Trie<T> &trie, std::ostream &out, const char sep = '|') {
    for (IteratorPtr<typename Trie<R>::Node*> it(rib.preorder()); !it->isDone(); it->next()) {
        if (it->currentItem()->value()) {
            std::string bits(it->currentItem()->bits());
            out << Prefix::prefix(bits) << sep << *it->currentItem()->value();
            // std::cout << "ribs:" << Prefix::prefix(bits) << std::endl;
            // trie.deaggregate(bits.begin(), bits.end());
            bool hasAncestors = false;
            typename Trie<T>::Node *node = trie.search(bits, false);
            unsigned cnt = 0;
            for (IteratorPtr<typename Trie<T>::Node*> rt(node->ancestors()); !rt->isDone(); rt->next()) {
                typename Trie<T>::Node* tnode = rt->currentItem();
                if (tnode->value()) {
                    hasAncestors = true;
                    // printNode(trie,out,tnode);
                    // out << '|' << Prefix::prefix(tnode->bits()) << '|' << *tnode->value();
                    ++cnt;
                }
            }
            if (hasAncestors) {
                out << sep << 'S';
                for (IteratorPtr<typename Trie<T>::Node*> rt(node->ancestors()); !rt->isDone(); rt->next()) {
                    typename Trie<T>::Node* tnode = rt->currentItem();
                    if (tnode->value()) {
                        // hasAncestors = true;
                        // printNode(trie,out,tnode);
                        out << sep << Prefix::prefix(tnode->bits()) << sep << *tnode->value();
                    }
                }
            }
            else {
                if (node->bits()==bits) {
                    out << sep << 'A';
                    for (IteratorPtr<typename Trie<T>::Node*> rt(node->preorder()); !rt->isDone(); rt->next()) {
                        typename Trie<T>::Node* tnode = rt->currentItem();
                        if (tnode->value()) {
                    out << sep << Prefix::prefix(tnode->bits()) << sep << *tnode->value();
                            // printNode(trie,out,tnode);
                        }
                    }
                } else {
                    out << sep << 'U';
                    out << sep << "unmatched";
                }
            }
            out << std::endl;
        }
    }
}

template<typename T, typename R>
void matchAllocs(Trie<T> &trie, Trie<R> &rib, std::ostream &out, const char sep = '|') {
    for (IteratorPtr<typename Trie<T>::Node*> it(trie.preorder()); !it->isDone(); it->next()) {
        if (it->currentItem()->value()) {
            std::string bits(it->currentItem()->bits());
            out << Prefix::prefix(bits) << sep << *it->currentItem()->value();
            typename Trie<R>::Node *node = rib.search(bits, false);
            unsigned cnt = 0;
            // for (IteratorPtr<typename Trie<R>::Node*> rt(node->ancestors()); !rt->isDone(); rt->next()) {
                // typename Trie<R>::Node* tnode = rt->currentItem();
                // if (tnode->value()&&(tnode!=node||node->bits()!=bits)) {
                // // if (tnode->value()&&tnode==node) {
                // // if (tnode->value()&&tnode!=node) {
                // // if (tnode->value()&&(tnode==node&&node->bits()==bits)) {
                    // out << sep << Prefix::prefix(tnode->bits()) << sep << *tnode->value();
                    // ++cnt;
                // }
            // }
            if (node->bits()==bits) {
                out << "|S";
                for (IteratorPtr<typename Trie<R>::Node*> rt(node->preorder()); !rt->isDone(); rt->next()) {
                    typename Trie<R>::Node* tnode = rt->currentItem();
                    // if (tnode->value()) {
                    if (tnode->value()&&tnode!=node) {
                        out << sep << Prefix::prefix(tnode->bits()) << sep << *tnode->value();
                        ++cnt;
                    }
                }
            } else
                if (!cnt) out << "|U|" << "unmatched";
            out << "|" << cnt << std::endl;
        }
    }
}

} // namespace trie
} // namespace wudanzy
#endif
