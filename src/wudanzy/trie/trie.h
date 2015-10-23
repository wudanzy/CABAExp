#ifndef __TRIE_H__
#define __TRIE_H__
#include <string>
#include <stdexcept>
#include "node.h"
#include "error.h"
#include "iterator.h"

namespace wudanzy {
namespace trie {

template<typename T>
class Trie {
public:
    typedef AbstractNode<T> Node;
    // typedef typename AbstractNode<T>::node_iterator node_iterator;
    // typedef typename AbstractNode<T>::trie_iterator trie_iterator;
    typedef typename AbstractNode<T>::trieptr trieptr;
protected:
    Node *root; // root of trie
    // void freeTrie() { if (root) { delete root; root = NULL; }}
public:
    Trie(Node* rt): root(rt) { if (!root) throw uninitialized_tree_error("NULL TrieRoot"); }
    // ~Trie() { if (root) freeTrie(root); }
    // ~Trie() { freeTrie(); }
    virtual ~Trie() { delete root; }
    void reset() { if (root) root->freeSons(); }

    // search for a string
    Node* search(const std::string& path, const bool fExactly = true);
    Node* search(const std::string::const_iterator b, const std::string::const_iterator e, const bool fExactly = true);
    
    // insert the value  
    Node* insert(const std::string& path, const T& value);
    Node* insert(const std::string::const_iterator b, const std::string::const_iterator e, const T& value);

    // iterators
    trieptr preorder() { return root->preorder(); }
    trieptr postorder() { return root->postorder(); }
};

template<typename T>
typename Trie<T>::Node* Trie<T>::search(const std::string::const_iterator b, const std::string::const_iterator e, const bool fExactly) {
    Node *rt, *nxt;
    std::string::const_iterator it;
    for (rt = root, it = b; rt; ++it, rt = nxt) {
        if (it == e) return rt;
        if (rt->hasSon(*it)) {
            nxt = rt->getSon(*it);
        } else {
            nxt = NULL;
        }
        if (!fExactly&&!nxt) return rt;
    }
    return NULL;
}

template<typename T>
typename Trie<T>::Node* Trie<T>::search(const std::string& s, const bool fExactly) {
    return search(s.begin(), s.end(), fExactly);
}

template<typename T>
typename Trie<T>::Node* Trie<T>::insert(const std::string::const_iterator b,const std::string::const_iterator e, const T& t) {
    Node *rt, *nxt;
    std::string::const_iterator it;
    // if (root==NULL) std::cout << "No root" << std::endl;
    for (rt = root, it = b; it != e && rt; ++it, rt = nxt) {
        // std::cout << "for it = " << *it << " rt = " << rt << std::endl;
        nxt = rt->getSon(*it);
        // std::cout << "nxt = " << nxt << std::endl;
    }
    if (rt) {
        rt->setValue(new T(t));
        return rt;
    } else {
        // std::cout << "error characters" << std::endl; 
        throw std::runtime_error("Unaccepted chars:");
        return NULL; 
    } // the string contains other characters 
}

template<typename T>
typename Trie<T>::Node* Trie<T>::insert(const std::string& s, const T& t) {
    // std::cout << "inserting " << s << std::endl;
    return insert(s.begin(), s.end(), t);
}


// a special kind of trie that can insert a range of numbers
template<typename T>
class BinaryTrie : public Trie<T> {
private:
    void insertOne(const unsigned from, const unsigned range, const T& value);
public:
    typedef typename Trie<T>::Node Node;
    typedef Trie<T> Base;
    typedef typename Base::trieptr trieptr;
    using Base::insert;
    using Base::postorder;
    using Base::root;
    BinaryTrie(Node* rt = new SegmentNode<T>()): Trie<T>(rt) {}
    void insert(unsigned from, unsigned range, const T& value);
    void aggregate();
    void  deaggregate(const std::string::const_iterator &from, const std::string::const_iterator &to);
    Node* insertAndAggregate(const std::string &str, const T &t);
private:
    bool aggregateSons(Node* node);
    bool absorbNode(Node* node);
};

// insert a node and aggregate if possible
template<typename T>
typename BinaryTrie<T>::Node *BinaryTrie<T>::insertAndAggregate(const std::string &path, const T &t)
{
    Node *node = insert(path, t);
    Node *tnode = node;
    if (absorbNode(tnode)) 
        return node;
    while (tnode->parent()) {
        if (aggregateSons(tnode->parent()))
            tnode = tnode->parent();
        else
            break;
    }
    return node;
}

template<typename T>
void BinaryTrie<T>::deaggregate(const std::string::const_iterator &b, const std::string::const_iterator &e) {
    Node *rt, *nxt;
    std::string::const_iterator it;
    for (rt = root, it = b; rt && it!=e; ++it, rt = nxt) {
        // if (it == e) return;
        // it != e means that rt is not the longest prefix
        if (rt->value()) {
            Node *left = rt->getSon('0');
            left->setValue(new T(*rt->value()));
            Node *right = rt->getSon('1');
            right->setValue(new T(*rt->value()));
            rt->setValue(NULL);
        }
        nxt = rt->getSon(*it);
    }
}

template<typename T>
bool BinaryTrie<T>::aggregateSons(Node* node) {
    if (node->hasSon('0')&&node->hasSon('1')) {
        AbstractNode<T> *left = node->hasSon('0'), *right = node->hasSon('1');
        if (!node->value()&&left->value()&&right->value()&&*left->value()==*right->value()) {
            node->setValue(new T(*left->value()));
            left->setValue(NULL);
            right->setValue(NULL);
            return true;
        }
    }
    return false;
}

// absorb a node if its value is the same with its nearest ancestor
template<typename T>
bool BinaryTrie<T>::absorbNode(Node* node) {
    if (node->value()) {
        Node* p = node->parent();
        while (p&&!p->value()) p = p->parent();
        if (p&&*p->value()==*node->value()) {
            node->setValue(NULL);
            return true;
        }
    }
    return false;
}

template<typename T>
void BinaryTrie<T>::aggregate() {
    for (trieptr it(postorder()); !it->isDone(); it->next()) {
        Node* node = it->currentItem();
        absorbNode(node);
        aggregateSons(node);
    }
}

template<typename T>
void BinaryTrie<T>::insertOne(const unsigned from, const unsigned range, const T& value) {
    std::string bits;
    for (unsigned index = 1 << 31; index >= range; index >>= 1) {
        char bit = (from & index)==index ? '1': '0';
        bits.push_back(bit);
    }
    // std::cout << "insert One" << bits << ' ' << value << std::endl;
    Base::insert(bits, value);
}

// insert a range of IP addresses
template<typename T>
void BinaryTrie<T>::insert(unsigned from, unsigned range, const T& value) {
    // std::cout << "BinaryTrie inserting begin" << std::endl;
    // int tmp_range = range;
    // if (tmp_range == 16777216) std::cout << "inserting " << from << ' ' << range << std::endl;
    while (range > 0) {
        unsigned room = (from & -from);
        unsigned block = range;
        while (block&(block-1)) block &= block - 1; 
        unsigned allocation = room < block? room: block;
        // std::cout << "inserting one " << from << ' ' << allocation << std::endl;
        insertOne(from, allocation, value);
        range -= allocation;
        from += allocation;
    }
    // std::cout << "BinaryTrie inserting end" << std::endl;
}

} // namespace trie
} // namespace wudanzy
#endif
