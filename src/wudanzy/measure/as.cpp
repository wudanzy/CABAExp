#include "as.h"
#include <iostream>

namespace wudanzy {
namespace measure {

size_t AS::mask = 0xffff;
size_t AS::len = 16;

AS::AS(const std::string &line) {
    if (!line.size()) throw std::runtime_error("Initial AS with an empty string!");
    size_t pos = 0;
    unsigned temp = std::stoul(line, &pos);
    if (pos!=line.size()) {
        // std::cout << line << ' ' << pos << std::endl;
        std::string substring = line.substr(pos+1);
        number = std::stoul(substring);
        number += temp << len;
    } else {
        // std::cout << line << ' ' << pos << std::endl;
        number = temp;
    }
}

void AS::Accept(ASVisitor& v) const {
    v.visitAS(this);
}

ASset::ASset(const std::string &line) {
    // std::cout << "constructing an ASset " << line << std::endl;
    std::string::const_iterator prev = line.begin();
    for (std::string::const_iterator it = line.begin();it != line.end(); ++it) {
        if (*it==',') {
            std::string tmp(prev,it);
            AS as(tmp);
            if (as.isValid()) insert(as);
            prev = it+1;
        }
    }
    if (prev != line.end()) {
        std::string tmp(prev, line.end());
        AS as(tmp);
        if (as.isValid()) insert(as);
    }
}

void ASset::Accept(ASVisitor& v) const {
    v.visitASset(this);
}

void MOAS::parse(const std::string &line) {
    // std::cout << "constructing a MOAS " << line << std::endl;
    std::string::const_iterator prev = line.begin();
    for (std::string::const_iterator it = line.begin();it != line.end(); ++it) {
        if (*it=='_') {
            std::string tmp(prev,it);
            ASset asset(tmp);
            if (asset.size()) insert(asset);
            // insert(ASset(tmp)); 
            prev = it+1;
        }
    } 
    if (prev != line.end()) {
        std::string tmp(prev, line.end());
        ASset asset(tmp);
        if (asset.size()) insert(asset);
    }
}

MOAS::MOAS(const std::string &line) {
    // std::cout << "constructing a MOAS " << line << std::endl;
    std::string::const_iterator prev = line.begin();
    for (std::string::const_iterator it = line.begin();it != line.end(); ++it) {
        if (*it=='_') {
            std::string tmp(prev,it);
            ASset asset(tmp);
            if (asset.size()) insert(asset);
            // insert(ASset(tmp)); 
            prev = it+1;
        }
    } 
    if (prev != line.end()) {
        std::string tmp(prev, line.end());
        ASset asset(tmp);
        if (asset.size()) insert(asset);
    }
}

void MOAS::Accept(ASVisitor& v) {
    v.visitMOAS(this);
}

size_t MOAS::getASNumber() const {
    if (this->size() > 0 && this->begin()->size() > 0) {
        return this->begin()->begin()->number;
    } else {
        throw std::runtime_error("MOAS: contains no ASes");
    } 
}
std::string MOAS::to_string() {
    ASPrintVisitor printer;
    Accept(printer);
    return printer.getStr();
}

std::ostream &operator << (std::ostream& os, const MOAS &moas) {
    ASPrintVisitor printer;
    static_cast<MOAS>(moas).Accept(printer);
    os << printer.getStr();
    return os;
}

std::istream &operator >> (std::istream& is, MOAS &moas) {
    std::string str;
    is >> str;
    moas.parse(str);
    return is;
}

} // namespace measure
} // namespace wudanzy
