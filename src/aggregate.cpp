#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include "wudanzy/trie/node.h"
#include "wudanzy/trie/trie.h"
#include "prefix.h"
#define Mytrie wudanzy::trie::BinaryTrie<std::string>
#define Mynode wudanzy::trie::AbstractNode<std::string>


void usage() {
    std::cout << "./aggreate input output" << std::endl;
}

void getIn(Mytrie &trie, std::ifstream &in) 
{
    std::string line;
    while (getline(in, line)) {
        std::string pfx, hop, as;
        std::istringstream record(line);
        record >> pfx >> hop >> as;
        trie.insert(Prefix::bits6(pfx), hop);
    }

}

void printNode(Mynode *node) {
    //out << Prefix::prefix6(ptr->path()) << ' ' << *ptr->value() << std::endl; 
}

void print(Mytrie &trie, std::ostream &out)
{
    for (auto ptr = trie.preorder(); !ptr->isDone(); ptr->next()) {
        if (ptr->value()) {
            out << Prefix::prefix6(ptr->path()) << ' ' << *ptr->value() << std::endl; 
        }
    }
}

Mynode* dfs(Mynode *node, Mynode *parent, Mynode *ancestor) 
{
    if (!node) return nullptr;
    //std::cout << node << ' ' << ancestor << std::endl;
    Mynode *cover = node->value()==nullptr? ancestor : node;
    Mynode *left = node->hasSon('0');
    Mynode *right = node->hasSon('1');
    Mynode *sibling = nullptr;
    if (parent) {
        sibling = parent->hasSon('0');
        if (sibling == node) sibling = parent->hasSon('1');
    }
    //both may be nullptr
    Mynode *leftSibling = dfs(left, node, cover);
    Mynode *rightSibling = dfs(right, node, cover);
    // level 1
    if (node->value()&&ancestor&&ancestor->value()&&*ancestor->value()==*node->value()) {
        node->setValue(nullptr);
    }
    // level 2
    if (!node->value()&&left&&right&&left->value()&&right->value()&&*left->value()==*right->value()) {
        node->setValue(new std::string(*left->value()));
        left->setValue(nullptr);
        right->setValue(nullptr);
    }
    // level 3
    if (ancestor==nullptr) {
        if (leftSibling&&rightSibling&&leftSibling->value()&&rightSibling->value()) {
            if (*leftSibling->value()==*rightSibling->value()) {
                // combine two entries
                node->setValue(new std::string(*leftSibling->value()));
                leftSibling->setValue(nullptr);
                rightSibling->setValue(nullptr);
                leftSibling = nullptr;
                rightSibling = nullptr; 
            } 
        } 
        std::cout << node << ' ' << left << ' ' << right << ' ';
        if (node->value()) std::cout << *node->value();
        std::cout << std::endl;
        if (sibling==nullptr&&node->value()) {
            return node;
        }
        if (left&&!right){
            return leftSibling;
        } else if (!left&&right) {
            return rightSibling;
        }
    }
    // level 4
    
    return nullptr;
}

void aggregate(Mytrie &trie)
{
    dfs(trie.root, nullptr, nullptr); 
}

int main(int argc, char** argv)
{
    char ch;
    std::string ifile, ofile;
    
    while ((ch = getopt(argc, argv, "v")) != EOF)
    {
        switch (ch) {
            case 'v':
                usage();
                break;
            default:
                usage();
                return -1;
        }
    }
    argc -= optind;
    argv += optind;
    if (argc == 2)
    {
        ifile = argv[0];
        ofile = argv[1];
    }
    if (!ifile.size()||!ofile.size())
    {
        usage();
        return -1;
    }
    std::ifstream in(ifile);
    std::ofstream out(ofile);
    Mytrie trie;
    getIn(trie, in);
    aggregate(trie);
    print(trie, std::cout);    
    //print(trie, out);    
    return 0;
}

