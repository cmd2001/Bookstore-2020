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
private:
    bool valid1[257], valid2[257], valid3[257];
public:
    Validator() {
        for(int i = 'a'; i <= 'z'; i++) valid1[i] = 1;
        for(int i = 'A'; i <= 'Z'; i++) valid1[i] = 1;
        for(int i = '0'; i <= '9'; i++) valid1[i] = 1;
        valid1['_'] = 1;
        for(int i = 33; i < 127; i++) valid2[i] = valid3[i] = 1;
        valid3['"'] = 0;
    }
    bool userid(const string x) const {
        if(!x.length() || x.length() > 30) return 0;
        for(int i = 0; i < x.length(); i++) if(!valid1[x[i]]) return 0;
        return 1;
    }
    bool password(const string x) const {
        if(!x.length() || x.length() > 30) return 0;
        for(int i = 0; i < x.length(); i++) if(!valid1[x[i]]) return 0;
        return 1;
    }
    bool privilege(const string x) const {
        return x == "7" || x == "3" || x == "1";
    }
    bool name(const string x) const {
        if(!x.length() || x.length() > 30) return 0;
        for(int i = 0; i < x.length(); i++) if(!valid2[x[i]]) return 0;
        return 1;
    }
    bool isbn(const string x) const {
        if(!x.length() || x.length() > 20) return 0;
        for(int i = 0; i < x.length(); i++) if(!valid2[x[i]]) return 0;
        return 1;
    }
    bool bookname(const string x) const {
        if(!x.length() || x.length() > 60) return 0;
        for(int i = 0; i < x.length(); i++) if(!valid3[x[i]]) return 0;
        return 1;
    }
    bool author(const string x) const {
        if(!x.length() || x.length() > 60) return 0;
        for(int i = 0; i < x.length(); i++) if(!valid3[x[i]]) return 0;
        return 1;
    }
    bool keyword(const string x) const {
        if(!x.length() || x.length() > 60) return 0;
        for(int i = 0; i < x.length(); i++) if(!valid3[x[i]]) return 0;
        return 1;
    }
    bool quantity(const string x) const {
        if(!x.length() || x[0] == '0') return 0;
        for(int i = 0; i < x.length(); i++) if(x[i] < '0' || x[i] > '9') return  0;
        return 1;
    }
    bool price(const string x) const {
        if(!x.length()) return 0;
        int cnt = 0;
        for(int i = 0; i < x.length(); i++) {
            if(x[i] == '.') ++cnt;
            else if(x[i] < '0' || x[i] > '9') return 0;
        }
        return cnt <= 1;
    }
};

#endif //BOOKSTORE_2020_VALIDATOR_HPP
