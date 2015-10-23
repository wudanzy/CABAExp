#ifndef __ERROR_H__
#define __ERROR_H__
// #include <exception>
#include <stdexcept>
namespace wudanzy {
namespace trie {
    
class unaccepted_char_error : public std::logic_error {
// char ch;
public:
    explicit unaccepted_char_error(const std::string& arg, const char& c): std::logic_error(arg+c) {}
    // virtual const char* what() const 
};

class uninitialized_tree_error : public std::runtime_error {
public:
    explicit uninitialized_tree_error(const std::string& arg): std::runtime_error(arg) {}
};

} //namespace trie
} //namespace wudanzy
#endif
