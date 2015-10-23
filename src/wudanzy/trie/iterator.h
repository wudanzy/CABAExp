#ifndef __ITERATOR_H__
#define __ITERATOR_H__
#include <iostream>
#include <stack>

namespace wudanzy {
namespace trie {

template<typename T> class AbstractNode;
template<typename T, const int N> class TrieNode;

template<typename T>
class Iterator {
public:
    virtual ~Iterator() {}
    virtual void next() = 0;
    virtual bool isDone() const = 0;
    virtual T currentItem() const = 0;
    virtual Iterator* clone() const = 0;
};

// The trie iterator is used to iterates over a trie 
// It supports the function to get the current path
template<typename T>
class TrieIterator : public Iterator<AbstractNode<T>*> {
public:
    virtual const std::string& path() = 0;
    virtual T* value() { return this->currentItem()->value(); }
    virtual TrieIterator* clone() const = 0;
};

template<typename Iterator>
class IteratorPtr {
public:
    IteratorPtr(): _i(0), _use(new std::size_t(1)) {}
    IteratorPtr(const Iterator &i);
    IteratorPtr(const IteratorPtr&);
    IteratorPtr& operator=(const IteratorPtr&);
    ~IteratorPtr() { decr_use(); }
    Iterator* operator->() { return _i; }
    Iterator& operator*() { return *_i; } 
private:
    Iterator *_i;
    // the count of pointers to the pointered objects
    size_t *_use;
    // called in both destructor and assignment operator to free pointers
    void decr_use() {
        if (--*_use == 0) {
            delete _i; 
            delete _use; 
        }
    }
};

template<typename Iterator>
IteratorPtr<Iterator>::IteratorPtr(const Iterator& i): _i(i.clone()), _use(new std::size_t(1)) {} 

template<typename Iterator>
IteratorPtr<Iterator>::IteratorPtr(const IteratorPtr& ptr): _i(ptr._i), _use(ptr._use) { ++*_use; }

template<typename Iterator>
IteratorPtr<Iterator>& IteratorPtr<Iterator>::operator=(const IteratorPtr& ptr){
   ++*ptr._use;
   decr_use();
   _i = ptr._i;
   _use = ptr._use;
   return *this;
}

// the iterator used to iterator the values in the tries
template<typename T, const int N>
class SonsIterator: public Iterator<AbstractNode<T>*> {
private:
    TrieNode<T,N> *node;
    unsigned cur;
public:
    SonsIterator(TrieNode<T,N> *rt): node(rt),cur(0) {}
    virtual void next() override { ++cur; }
    virtual bool isDone() const override { return cur >= N; }
    virtual AbstractNode<T>* currentItem() const override { return node->_sons[cur]; }
    virtual SonsIterator* clone() const override { return new SonsIterator(*this); }
};

// the iterator used to iterator the values in the tries
template<typename T, const int N>
class SonsReverseIterator: public Iterator<AbstractNode<T>*> {
private:
    TrieNode<T,N> *node;
    unsigned cur;
public:
    SonsReverseIterator(TrieNode<T,N> *rt): node(rt),cur(1) {}
    virtual void next() override { ++cur; }
    virtual bool isDone() const override { return cur > N; }
    virtual AbstractNode<T>* currentItem() const override { return node->_sons[N-cur]; }
    virtual SonsReverseIterator* clone() const override { return new SonsReverseIterator(*this); }
};

template<typename T>
class AncestorIterator: public Iterator<AbstractNode<T>*> {
private:
    typedef AbstractNode<T> node_type;
    node_type *cur;
public:
    AncestorIterator(node_type *root): cur(root) {}
    virtual void next() override { cur = cur->parent(); }
    virtual bool isDone() const override {return !cur; }
    virtual node_type* currentItem() const override { return cur; }
    virtual AncestorIterator* clone() const override { return new AncestorIterator(*this); }
};


template<typename T> 
class PreorderNodeIterator: public TrieIterator<T> {
private:
    typedef AbstractNode<T> node_type;
    std::stack<node_type*> s;
    std::stack<node_type*> nodepath;
    std::string cpath;
public:
    PreorderNodeIterator(node_type *root) {if (root) { s.push(root); nodepath.push(root); } }
    virtual void next() override;
    virtual bool isDone() const override { return s.empty(); }
    virtual node_type* currentItem() const override { return s.top(); }
    virtual const std::string& path() override { return cpath; }
    virtual PreorderNodeIterator* clone() const override { return new PreorderNodeIterator(*this); }
};

template<typename T>
void PreorderNodeIterator<T>::next() {
    node_type *head = s.top();
    s.pop();
    for (IteratorPtr<Iterator<node_type*>> it(head->rsons());!it->isDone(); it->next()) {
        if (it->currentItem()) s.push(it->currentItem());
    }
    while (!s.empty()&&s.top()->parent()!=nodepath.top()) {
        nodepath.pop();
        cpath.pop_back();
    }
    if (!s.empty()) { 
        nodepath.push(s.top());
        cpath.push_back(s.top()->getChar());
    }
}

template<typename T>
class PostorderNodeIterator: public TrieIterator<T> {
private:
    typedef AbstractNode<T> node_type;
    std::stack<node_type*> s;
    std::stack<node_type*> nodepath;
    node_type *cur, *prev;
    void expand();
    bool step();
    std::string cpath;
public:
    PostorderNodeIterator(node_type *rt): cur(rt), prev(NULL) {if (rt) { s.push(rt); nodepath.push(rt); } expand(); }
    virtual void next();
    virtual bool isDone() const { return s.empty(); }
    virtual node_type* currentItem() const { return cur; }
    virtual const std::string &path() override { return cpath; }
    virtual PostorderNodeIterator* clone() const override { return new PostorderNodeIterator(*this); }
};

template<typename T>
void PostorderNodeIterator<T>::expand() {
    while (!s.empty()) {
        if (prev&&prev->parent()==cur) return;
        bool fEmpty = true;
        for (IteratorPtr<Iterator<node_type*>> rt(cur->rsons()); !rt->isDone(); rt->next()) {
            if (rt->currentItem()) {
                s.push(rt->currentItem());
                fEmpty = false;
            }
        }
        if (fEmpty) return;
            else {
                cur = s.top();
                nodepath.push(cur);
                cpath.push_back(cur->getChar());
            }
    }
}

template<typename T>
void PostorderNodeIterator<T>::next() {
    prev = s.top();
    s.pop();
    nodepath.pop();
    if (cpath.size()) cpath.pop_back();
    if (!s.empty()) {
        cur = s.top();
        if (prev->parent()!=cur) {
            nodepath.push(cur);
            cpath.push_back(cur->getChar());
        }
    }
    expand();
}

// This iterator garantees that each node that returned by currentItem is valid
template<typename T>
class ValidNodeIterator: public Iterator<AbstractNode<T>*> {
private:
    typedef AbstractNode<T> node_type;
    Iterator<node_type*> *_it;
    void expand();
public:
    ValidNodeIterator(Iterator<node_type*> *it): _it(it) { expand(); }
    virtual ~ValidNodeIterator() { delete _it;}
    virtual void next() { _it->next(); }
    virtual bool isDone() const { return _it->isDone(); }
    virtual node_type* currentItem() const { return _it->currentItem(); }
};

template<typename T>
void ValidNodeIterator<T>::expand() {
    while (!_it->isDone()&&!_it->currentItem()->value()) {
        _it->next();
    }
}

} // namespace trie
} // namespace wudanzy
#endif
