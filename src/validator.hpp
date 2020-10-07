//
// Created by Sion on 2020/10/7.
//

#ifndef BOOKSTORE_2020_VALIDATOR_HPP
#define BOOKSTORE_2020_VALIDATOR_HPP

#include <iostream>
#include <cstdio>
#include <cassert>
#include <algorithm>
#include <vector>
#include <stack>
#include <map>
#include "types.hpp"
#include "core.hpp"
#include "database_cached.hpp"
#ifndef debug
#define debug cerr
#endif
using namespace std;
using __Amagi::database_cached; // B+ Tree with rb-tree cache.

class Validator { // todo: input Validator
public:
    bool userid(const string x) const {
        return 1;
    }
    bool password(const string x) const {
        return 1;
    }
    bool privilege(const string x) const {
        return 1;
    }
    bool name(const string x) const {
        return 1;
    }
    bool isbn(const string x) const {
        return 1;
    }
    bool bookname(const string x) const {
        return 1;
    }
    bool author(const string x) const {
        return 1;
    }
    bool keyword(const string x) const {
        return 1;
    }
    bool quantity(const string x) const {
        return 1;
    }
    bool price(const string x) const {
        return 1;
    }
};

#endif //BOOKSTORE_2020_VALIDATOR_HPP
