#ifndef __PREFIX_H__
#define __PREFIX_H__
#include <string>
#include <utility>
#include <arpa/inet.h>
class Prefix{
public:
    typedef std::pair<unsigned, unsigned> pair;
    // typedef std::pair<in6_addr, size_t> pair6;
    struct pair6 : public std::pair<in6_addr, size_t> {
        pair6(const in6_addr &in6, const size_t& size = 0);
        pair6();
    };
    static std::string bits(const std::string& pfx);
    static std::string bits6(const std::string& pfx);
    // static std::string bits(const std::string& pfx, const unsigned mask);
    static std::string prefix(const std::string& bits);
    static std::string prefix6(const std::string& bits);
    static std::string add_one(const std::string& bits);
    static std::string subtract_one(const std::string& bits);
    static std::string brother_bits(const std::string& bits);
    static unsigned bitsBegin(const std::string& bits);
    static unsigned bitsEnd(const std::string& bits);
    static unsigned ip_to_num(const std::string& ip);
// private:
    static pair pfx_to_num(const std::string& pfx);
    static pair bits_to_num(const std::string& bits);
    static std::string num_to_str(unsigned num, const unsigned base);
    static unsigned char bits_to_byte(const std::string& bits);
    static in6_addr ip6_to_num(const std::string& ip);
    static std::string num_to_ip6(const in6_addr& in6);
    static pair6 pfx6_to_pair(const std::string& pfx);
    static pair6 bits6_to_pair(const std::string& bits);
    static std::string pair_to_pfx6(const pair6& pair);
    static std::string pair_to_bits6(const pair6& pair);
};
#endif
