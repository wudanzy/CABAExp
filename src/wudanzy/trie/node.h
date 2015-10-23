#ifndef __NODE_H__
#define __NODE_H__
#include <vector>
#include <iterator>
#include <cstring>
#include "iterator.h"
// #include "prefix.h"
#include "error.h"

namespace wudanzy {
namespace trie {

template<typename T>
class AbstractNode {
public:
    //typedefs
    typedef Iterator<AbstractNode<T>*> node_iterator;
    typedef TrieIterator<T> trie_iterator;
    typedef IteratorPtr<trie_iterator> trieptr;
    typedef IteratorPtr<node_iterator> nodeptr;
    // typedef std::iterator<std::forward_iterator_tag, AbstractNode<T>> iterator;
private:
    typedef AbstractNode<T> node_type;
    char _c; // character
    T* _v;  // the value
    node_type *_parent; // the parent

    // need by getSon function
    virtual node_type*& son(const char c) = 0;
    virtual node_type* clone() = 0;
    void initialize(char ch, AbstractNode<T> *p) { _c = ch; _parent = p; } 
public:

    //constructors and destructor
    AbstractNode(char ch = '\0', AbstractNode<T> *p = NULL): _c(ch), _v(NULL), _parent(p) {}
    AbstractNode(const AbstractNode<T> &other): _c(other._c), _v(NULL), _parent(other._parent) {}
    virtual ~AbstractNode() { if (_v) { delete _v; } }

    // gets and sets
    char getChar() { return _c; }
    node_type* parent() { return _parent; }
    T* value() { return _v; }
    void setValue(T *value) { if (_v) delete _v; _v = value; }

    // deals with sons
    virtual bool acceptChar(char c) const = 0;
    node_type* hasSon(char c) const { return const_cast<node_type*>(this)->son(c); }
    node_type* getSon(char c); 
    
    // corresponding string
    std::string bits();

    // iterates 
    nodeptr ancestors() { return nodeptr(AncestorIterator<T>(this)); }
    trieptr preorder() { return trieptr(PreorderNodeIterator<T>(this)); }
    trieptr postorder() { return trieptr(PostorderNodeIterator<T>(this)); }
    virtual nodeptr sons() = 0;
    virtual nodeptr rsons() = 0;
};

template<typename T>
AbstractNode<T>* AbstractNode<T>::getSon(char c) {
    if (!acceptChar(c)) throw unaccepted_char_error("getSon(): Unsupported char:", c);
    if (son(c)==NULL) {
        node_type* ptr = clone();   
        ptr->initialize(c,this);
        son(c) = ptr;
    }
    return son(c);
}


template<typename T>
std::string AbstractNode<T>::bits() {
    std::string res;
    std::stack<char> s;
    // for (auto it = ancestorsBegin(); it != ancestorsEnd(); ++it) {
        // s.push(it->getChar());
    // }
    for (IteratorPtr<Iterator<AbstractNode<T>*>> it(ancestors()); !it->isDone(); it->next() ) {
        s.push(it->currentItem()->getChar());
    }
    s.pop();
    while (!s.empty()) {
        res.push_back(s.top());
        s.pop();
    }
    return res;
}


const std::string cset = "0123456789./abcdef: |"; // N==2 binary prefix, N==12 ipv4 prfix, N==19 ipv6 prefix, N==21 ipv6 prefix + as paths
static size_t cidx[128] = {65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 19, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 18, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 12, 13, 14, 15, 16, 17, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536, 20, 65536, 65536, 65536};

template<typename T, const int N>
class TrieNode : public AbstractNode<T> {
    typedef AbstractNode<T> base_node;
private:
    friend class SonsIterator<T,N>;
    friend class SonsReverseIterator<T,N>;
    base_node *_sons[N];
    virtual base_node*& son(const char c) override;
    virtual TrieNode<T,N>* clone() override;
public:
    TrieNode(char ch = '\0', AbstractNode<T> *p = NULL): AbstractNode<T>(ch,p) { memset(_sons,0,sizeof(_sons));}
    TrieNode(const TrieNode<T,N> &other): AbstractNode<T>(other) { memset(_sons,0,sizeof(_sons));}
    virtual ~TrieNode();
    virtual bool acceptChar(char c) const override;
    virtual typename base_node::nodeptr sons() override;
    virtual typename base_node::nodeptr rsons() override;
};

template<typename T, const int N>
typename TrieNode<T,N>::base_node*& TrieNode<T,N>::son(const char c) { 
    if (!acceptChar(c)) 
        throw unaccepted_char_error("TrieNode<T,N>::son: Unsupported char:", c);
    return _sons[cidx[static_cast<int>(c)]]; 
}

template<typename T, const int N>
typename AbstractNode<T>::nodeptr TrieNode<T,N>::sons() {
    return typename base_node::nodeptr(SonsIterator<T,N>(this));
}

template<typename T, const int N>
typename AbstractNode<T>::nodeptr TrieNode<T,N>::rsons() {
    return typename base_node::nodeptr(SonsReverseIterator<T,N>(this));
}

template<typename T, const int N>
TrieNode<T,N>::~TrieNode() {
    for (unsigned i = 0; i < N; ++i) {
        delete _sons[i];
    }
}

template<typename T, const int N>
bool TrieNode<T,N>::acceptChar(char c) const {
    unsigned index = cidx[static_cast<int>(c)];
    return index < N;
}


template<typename T, const int N>
TrieNode<T,N>* TrieNode<T,N>::clone() {
    return new TrieNode<T,N>(*this);
}


template<typename T>
class SegmentNode : public TrieNode<T,2> {
private:
    typedef AbstractNode<T> node_type;
    // unsigned lower, higher;
public:
    node_type* left() { return this->hasSon('0'); }
    node_type* right() { return this->hasSon('1'); }
};

} // namespace trie
} // namespace wudanzy
#endif
