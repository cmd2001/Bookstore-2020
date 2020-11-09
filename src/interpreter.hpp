#ifndef BOOKSTORE_2020_EVERYTHING_HPP
#define BOOKSTORE_2020_EVERYTHING_HPP

#include <iostream>
#include <cstdio>
#include <cassert>
#include <algorithm>
#include <vector>
#include <stack>
#include <map>
#include "types.hpp"
#include "core.hpp"
#include "validator.hpp"
#ifndef debug
#define debug cerr
#endif
using namespace std;
using __Amagi::database_cached; // B+ Tree with rb-tree cache.

class Interpreter {
private:
    Bookstore core;
    Validator valid;
    vector<string> split(const string &s) {
        vector<string> ret;
        for(int i = 0, j = 0; i < s.length(); i = j) {
            while(i < s.length() && s[i] == ' ') ++i;
            if(i == s.length()) break;
            for(j = i; j < s.length() && s[j] != ' '; j++);
            ret.push_back(s.substr(i, j - i));
        }
        return ret;
    }
    string getline() {
        string ret;
        char c;
        while((c = getchar()) != '\n' && c != EOF) ret = ret + c;
        return ret;
    }
    void Invalid() {
        puts("Invalid");
    }
    int string2int(const string &x) {
        int ret = 0;
        for(int i = 0; i < x.length(); i++) ret = ret * 10 + x[i] - '0';
        return ret;
    }
    double string2double(const string &x) {
        double ret = 0, mul = 0.1;
        bool flag = 0;
        for(int i = 0; i < x.length(); i++) {
            if(x[i] == '.') flag = 1;
            else {
                if(!flag) ret = ret * 10 + x[i] - '0';
                else ret += mul * (x[i] - '0'), mul *= 0.1;
            }
        }
        return ret;
    }
    bool checks2(const string &s2) {
        if(!s2.length()) return 0;
        if(s2[0] != '"') return 0;
        if(s2[s2.length() - 1] != '"') return 0;
        return 1;
    }
    pair<bool, pair<Key, string> > solve_Core(const string &s) {
        string s1, s2;
        for(int i = 0; i < s.length(); i++) if(s[i] == '=') {
            s1 = s.substr(0, i), s2 = s.substr(i + 1, s.length() - (i + 1));
            break;
        }
        if(s1 == "-ISBN") {
            if(!valid.isbn(s2.substr(1, s2.length() - 2))) return make_pair(0, make_pair(Key(0), ""));
            return make_pair(1, make_pair(iSbN, s2.substr(1, s2.length() - 2)));
        }
        if(s1 == "-price") {
            if(!valid.price(s2.substr(1, s2.length() - 2))) return make_pair(0, make_pair(Key(0), ""));
            return make_pair(1, make_pair(PRICE, s2.substr(1, s2.length() - 2)));
        }

        if(!checks2(s2)) return make_pair(0, make_pair(Key(0), ""));

        if(s1 == "-name") {
            if(!valid.bookname(s2.substr(1, s2.length() - 2))) return make_pair(0, make_pair(Key(0), ""));
            return make_pair(1, make_pair(NAME, s2.substr(1, s2.length() - 2)));
        }
        if(s1 == "-author") {
            if(!valid.author(s2.substr(1, s2.length() - 2))) return make_pair(0, make_pair(Key(0), ""));
            return make_pair(1, make_pair(AUTHOR, s2.substr(1, s2.length() - 2)));
        }
        if(s1 == "-keyword") {
            if(!valid.keyword(s2.substr(1, s2.length() - 2))) return make_pair(0, make_pair(Key(0), ""));
            return make_pair(1, make_pair(KEYWORD, s2.substr(1, s2.length() - 2)));
        }
    }
    pair<bool, map<Key, string> > solve_arg(const vector<string> &v) {
        map<Key, string> ret;
        for(int i = 1; i < v.size(); i++) {
            debug << v[i] << endl;
            auto t = solve_Core(v[i]);
            if(!t.first) return make_pair(0, map<Key, string>());
            ret.insert(t.second);
        }
        return make_pair(1, ret);
    }
    void printBooks(const vector<Book> &v) {
        for(auto &t: v) {
            printf("%s\t%s\t%s\t%s\t%0.2lf\t%d\n", t.isbn, t.name, t.author, t.keyword, t.price, t.quan);
        }
        if(!v.size()) puts("");
    }
public:
    Interpreter() {
        string line;
        while((line = getline()) != "") {
            auto sp = split(line);
            if(!sp.size()) continue; // empty line
            else if(sp[0] == "su") {
                if(sp.size() == 3) {
                    if(!valid.userid(sp[1]) || !valid.password(sp[2])) Invalid();
                    else if(!core.su(sp[1].c_str(), sp[2].c_str())) Invalid();
                } else {
                    if(sp.size() != 2) Invalid();
                    else if(!core.su(sp[1].c_str(), "")) Invalid();
                }
            } else if(sp[0] == "logout") {
                if(sp.size() != 1) Invalid();
                else if(!core.logout()) Invalid();
            } else if(sp[0] == "useradd") {
                if(sp.size() != 5) Invalid();
                else if(!valid.userid(sp[1]) || !valid.password(sp[2]) || !valid.privilege(sp[3]) || !valid.name(sp[4])) Invalid();
                else if(!core.useradd(sp[1].c_str(), sp[2].c_str(), string2int(sp[3]), sp[4].c_str())) Invalid();
            } else if(sp[0] == "register") {
                if(sp.size() != 4) Invalid();
                else if(!valid.userid(sp[1]) || !valid.password(sp[2]) || !valid.name(sp[3])) Invalid();
                else if(!core.reg(sp[1].c_str(), sp[2].c_str(), sp[3].c_str())) Invalid();
            } else if(sp[0] == "delete") {
                if(sp.size() != 2) Invalid();
                else if(!valid.userid(sp[1])) Invalid();
                else if(!core.del(sp[1].c_str())) Invalid();
            } else if(sp[0] == "passwd") {
                if(sp.size() == 4) {
                    if(!valid.userid(sp[1]) || !valid.password(sp[2]) || !valid.password(sp[3])) Invalid();
                    else if(!core.passwd(sp[1].c_str(), sp[2].c_str(), sp[3].c_str())) Invalid();
                } else {
                    if(core.get_cur_pri() != 7 || !valid.userid(sp[1]) || !valid.password(sp[2])) Invalid();
                    else if(!core.passwd(sp[1].c_str(), "", sp[2].c_str())) Invalid();
                }
            } else if(sp[0] == "select") {
                if(sp.size() != 2) Invalid();
                else if(!valid.isbn(sp[1])) Invalid();
                else if(!core.select(sp[1].c_str())) Invalid();
            } else if(sp[0] == "modify") {
                auto t = solve_arg(sp);
                if(!t.first) {
                    debug << "unable to parse arguments" << endl;
                    Invalid();
                }
                else {
                    if(!core.modify(t.second[iSbN].c_str(), t.second[NAME].c_str(), t.second[AUTHOR].c_str(), t.second[KEYWORD].c_str(), t.second[PRICE] == "" ? -1 : string2double(t.second[PRICE]))) Invalid();
                }
            } else if(sp[0] == "import") {
                if(sp.size() != 3) Invalid();
                else if(!valid.quantity(sp[1]) || !valid.price(sp[2])) Invalid();
                else if(!core.import(string2int(sp[1]), string2double(sp[2]))) Invalid();
            } else if(sp[0] == "show") {
                if(sp.size() == 1) { // no arguments
                    auto ans = core.showall();
                    printBooks(ans);
                } if(sp[1] == "finance") {
                    if(sp.size() > 3) Invalid();
                    else {
                        if(sp.size() == 3 && !valid.quantity(sp[2])) Invalid();
                        else {
                            int tim = -1;
                            if(sp.size() == 3) tim = string2int(sp[2]);
                            auto ret = core.show_finance(tim);
                            if(!ret.first) Invalid();
                            else printf("+ %0.2Lf - %0.2Lf\n", ret.second.first, ret.second.second);
                        }
                    }
                } else {
                    if(sp.size() != 2) Invalid();
                    auto t = solve_Core(sp[1]);
                    if(!t.first) Invalid();
                    else {
                        auto ans = core.show(t.second.first, t.second.second.c_str());
                        if(!ans.first) Invalid();
                        else printBooks(ans.second);
                    }
                }
            } else if(sp[0] == "buy") {
                if(sp.size() != 3) Invalid();
                else if(!valid.isbn(sp[1]) || !valid.quantity(sp[2])) Invalid();
                else {
                    auto t = core.buy(sp[1].c_str(), string2int(sp[2]));
                    if(!t.first) Invalid();
                    else printf("%0.2f\n", t.second);
                }
            } else if(sp[0] == "exit" || sp[0] == "quit") break;
            else Invalid();
        }
    }
};

#endif //BOOKSTORE_2020_EVERYTHING_HPP
