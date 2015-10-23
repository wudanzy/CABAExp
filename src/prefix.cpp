#include <string>
#include <stdexcept>
#include <cstring>
#include "prefix.h"


Prefix::pair6::pair6(const in6_addr &in6, const size_t& size) {
    first = in6; 
    second = size;
}
Prefix::pair6::pair6() {
    memset(&first, 0, sizeof(first));
    second = 0;
}

unsigned Prefix::ip_to_num(const std::string& ip) {
    unsigned total = 0, cnt = 4, sum = 0;
    for (std::string::const_iterator c = ip.begin(); c != ip.end(); ++c) {
        if (*c == '.' || *c == '/') {
            total += sum << (--cnt*8); 
            sum = 0;
        } else {
            sum *= 10;
            sum += *c - '0';
        }
    }
    total += sum;
    return total;
}

Prefix::pair Prefix::pfx_to_num(const std::string& pfx) {
    unsigned total = 0, sum = 0, cnt = 4;
    for (std::string::const_iterator c = pfx.begin(); c != pfx.end(); ++c) {
        if (*c == '.' || *c == '/') {
            total += sum << (--cnt*8); 
            sum = 0;
        } else {
            sum *= 10;
            sum += *c - '0';
        }
    }
    return std::make_pair(total, sum);
}

std::string Prefix::bits(const std::string& pfx) {
    Prefix::pair p = pfx_to_num(pfx);
    size_t v = p.first, mask = p.second;
    std::string res;
    for (size_t i = 0; i < mask; ++i) {
        if (v & 1U<<31) res.push_back('1');
        else res.push_back('0');
        v <<= 1;
    }
    return res;
}

std::string Prefix::bits6(const std::string& pfx6) {
    Prefix::pair6 p = pfx6_to_pair(pfx6);
    return pair_to_bits6(p);
}

std::string Prefix::num_to_str(unsigned num, const unsigned base = 10) {
    if (num == 0) return std::string("0");
    std::string res;
    unsigned mod = 1;
    while (mod * base <= num) mod *= base;
    while (mod) {
        unsigned left = num/mod;
        if (left <= 9) {
            res.push_back('0'+left);
        } else {
            res.push_back('a'+left-10);
        }
        num %= mod;
        mod /= base;
    }
    return res;
}

std::string Prefix::prefix(const std::string& bits) {
    pair p = bits_to_num(bits);
    unsigned v = p.first, mask = p.second;
    std::string res;
    for (int i = 3; i>=0; --i) {
        unsigned tmp = (v >> i*8)&0xff;
        if (tmp) {
            res += num_to_str(tmp);
        } else res.push_back('0');
        if (i) res.push_back('.');
    }
    res.push_back('/');
    if (mask) res += num_to_str(mask);
        else res.push_back('0');
    return res;
}

std::string Prefix::prefix6(const std::string& bits6) {
    pair6 p = bits6_to_pair(bits6);
    return pair_to_pfx6(p);
}

Prefix::pair Prefix::bits_to_num(const std::string& bits) {
    unsigned mask = 0, sum = 0;
    for (std::string::const_iterator c = bits.begin(); c != bits.end(); ++c) {
        sum <<= 1;
        sum += *c - '0';
        ++mask;
    }
    if (mask<32) sum <<= (32 - mask);
    return std::make_pair(sum, mask);
}

unsigned Prefix::bitsBegin(const std::string& bits) {
    return bits_to_num(bits).first;
}

unsigned Prefix::bitsEnd(const std::string& bits) {
    pair p = bits_to_num(bits);
    return p.first + (1U << (32 - p.second));
}

std::string Prefix::add_one(const std::string& bits) {
    std::string res = std::string(bits);
    int index = res.length() - 1;
    while (index>=0 && res[index] == '1') --index;
    if (index >= 0) {
        for (size_t i = index; i < res.length(); ++i) {
            res[i] = res[i]=='0'?'1':'0';
        }
    } else {
        throw std::runtime_error("the bits string full of 1s");
    } 
    return res;
}

std::string Prefix::subtract_one(const std::string& bits) {
    std::string res = std::string(bits);
    int index = res.length() - 1;
    while (index>=0 && res[index] == '0') --index;
    if (index >= 0) {
        for (size_t i = index; i < res.length(); ++i) {
            res[i] = res[i]=='0'?'1':'0';
        }
    } else {
        throw std::runtime_error("the bits string full of 0s");
    } 
    return res;
}

std::string Prefix::brother_bits(const std::string& bits) {
    std::string res(bits);
    int index = res.length() - 1;
    if (index >= 0) {
        if (res[index] == '0') {
            res[index] = '1';
        } else if (res[index] == '1') {
            res[index] = '0';
        } else {
            throw std::runtime_error("Prefix: the bits contains characters other than 0/1");
        }
    } else {
        throw std::runtime_error("Prefix: empty string");
    }
    return res;
}

in6_addr Prefix::ip6_to_num(const std::string& ip) {
    in6_addr in6;
    memset(&in6, 0, sizeof(in6));
    int s = inet_pton(AF_INET6, ip.c_str(), &in6);
    if (s <= 0) {
        std::string errors = "Prefix::ip6_to_num conversion is failed";
        errors += ip;
        throw std::runtime_error(errors);
    }
    return in6;
}

std::string Prefix::num_to_ip6(const in6_addr& in6) {
    char str[INET6_ADDRSTRLEN]; 
    const char *pChar = inet_ntop(AF_INET6, &in6, str, INET6_ADDRSTRLEN);
    if (pChar != str) {
        throw std::runtime_error("Prefix::num_to_ip6 conversion is failed");
    }
    return std::string(str);
}

Prefix::pair6 Prefix::pfx6_to_pair(const std::string& pfx) {
    Prefix::pair6 pair6;
    std::string::size_type pos = pfx.find('/');
    if (pos == std::string::npos) {
        throw std::runtime_error("Prefix::pfx6_to_pair character '/' is not found");
    }
    pair6.second = static_cast<size_t>(std::stoi(pfx.substr(pos+1)));
    pair6.first = ip6_to_num(pfx.substr(0, pos));
    return pair6;
}

unsigned char Prefix::bits_to_byte(const std::string& bits) {
    unsigned char mask = 0, sum = 0;
    for (std::string::const_iterator c = bits.begin(); c != bits.end(); ++c) {
        sum <<= 1;
        sum += *c - '0';
        ++mask;
    }
    if (mask < 8) sum <<= (8 - mask);
    return sum;   
} 

Prefix::pair6 Prefix::bits6_to_pair(const std::string& bits) {
    Prefix::pair6 pair6;
    for (size_t i = 0; i < bits.length(); i += 8 ) {
        size_t len = (bits.length() - i < 8) ? bits.length() - i : 8; 
        std::string tmpStr = bits.substr(i, len);
        pair6.first.s6_addr[i/8] = bits_to_byte(tmpStr);
    }
    pair6.second = bits.size();
    return pair6;
}

std::string Prefix::pair_to_pfx6(const pair6& pair) {
    std::string res = num_to_ip6(pair.first);
    res.push_back('/');
    res.append(std::to_string(pair.second));
    return res;
}

std::string Prefix::pair_to_bits6(const pair6& pair) {
    std::string res;
    for (size_t i=0; i*8 < pair.second; ++i) {
        std::string tmpStr = num_to_str(pair.first.s6_addr[i], 2);
        size_t numberOfZeros = 8 - tmpStr.size();
        res.append(numberOfZeros, '0');
        res.append(tmpStr);
    }
    while (res.size()> pair.second) res.pop_back();
    return res;
}
