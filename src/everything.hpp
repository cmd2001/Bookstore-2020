#ifndef BOOKSTORE_2020_EVERYTHING_HPP
#define BOOKSTORE_2020_EVERYTHING_HPP

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <vector>
#include <stack>
#include "database_cached.hpp"
#define debug cerr
using namespace std;
using __Amagi::database_cached; // B+ Tree with rb-tree cache.

constexpr int len_ISBN = 25;
struct ISBN {
    char data[len_ISBN];
    ISBN() {
        memset(data, 0, len_ISBN);
    }
    ISBN(const char* isbn) {
        memset(data, 0, len_ISBN);
        memcpy(data, isbn, strlen(isbn));
    }
    char& operator [] (const int x) {
        return data[x];
    }
    const char& operator [] (const int x) const {
        return data[x];
    }
    friend bool operator < (const ISBN &a, const ISBN &b) {
        for(int i = 0; i < len_ISBN; i++) if(a[i] != b[i]) return a[i] < b[i];
        return 0;
    }
    bool is_null() {
        return !data[0];
    }
};

constexpr int len_Other_Data = 65;

struct Other_Data {
    char data[len_Other_Data], ISBN[len_ISBN];
    Other_Data() {
        memset(data, 0, len_Other_Data);
        memset(ISBN, 0, len_ISBN);
    }
    Other_Data(const char* dd) {
        memset(data, 0, len_Other_Data);
        memset(ISBN, 0, len_ISBN);
        memcpy(data, dd, strlen(dd));
    }
    char& operator [] (const int x) {
        return data[x];
    }
    const char& operator [] (const int x) const {
        return data[x];
    }
    friend bool operator < (const Other_Data &a, const Other_Data &b) {
        for(int i = 0; i < len_Other_Data; i++) if(a[i] != b[i]) return a[i] < b[i];
        for(int i = 0; i < len_ISBN; i++) if(a.ISBN[i] != b.ISBN[i]) return a.ISBN[i] < b.ISBN[i];
        return 0;
    }
    void construct_Max() {
        memset(ISBN, 255, len_ISBN);
    }
};

struct Book {
    char isbn[len_ISBN], name[len_Other_Data], author[len_Other_Data], keyword[len_Other_Data];
    double price;
    int qual;
    Book() {
        memset(isbn, 0, len_ISBN), memset(name, 0, len_Other_Data), memset(author, 0, len_Other_Data), memset(keyword, 0, len_Other_Data);
        price = qual = 0;
    }
    ISBN get_ISBN() const {
        ISBN ret;
        memcpy(ret.data, isbn, len_ISBN);
        return ret;
    }
    Other_Data get_name() const {
        Other_Data ret;
        memcpy(ret.data, name, len_Other_Data), memcpy(ret.ISBN, isbn, len_ISBN);
        return ret;
    }
    Other_Data get_author() const {
        Other_Data ret;
        memcpy(ret.data, author, len_Other_Data), memcpy(ret.ISBN, isbn, len_ISBN);
        return ret;
    }
    vector<Other_Data> get_keywords() const {
        vector<Other_Data> ret;
        for(int i = 0, j = 0, len = strlen(keyword); i < len; i = j) { // j is the first position after '|'.
            while(i < len && keyword[i] != '|') ++i;
            Other_Data r2;
            for(int k = 0; k < i - j; k++) r2.data[k] = keyword[j + k];
            memcpy(r2.ISBN, isbn, len_ISBN);
            j = i + 1;
        }
        return ret;
    }
    void fill_ISBN(const char* _isbn) {
        memset(isbn, 0, len_ISBN);
        memcpy(isbn, _isbn, strlen(_isbn));
    }
    void fill_name(const char* _name) {
        memset(name, 0, len_Other_Data);
        memcpy(name, _name, strlen(_name));
    }
    void fill_author(const char* _author) {
        memset(author, 0, len_Other_Data);
        memcpy(author, _author, strlen(_author));
    }
    void fill_keyword(const char* _keyword) {
        memset(keyword, 0, len_Other_Data);
        memcpy(keyword, _keyword, strlen(_keyword));
    }
    void fill_price(const double _price) {
        price = _price;
    }
    friend bool operator < (const Book &a, const Book &b) {
        for(int i = 0; i < len_ISBN; i++) if(a.isbn[i] != b.isbn[i]) return a.isbn[i] < b.isbn[i];
        return 0;
    }
};

constexpr int len_User = 35;

struct User_ID {
    char data[len_User];
    User_ID() {
        memset(data, 0, len_User);
    }
    User_ID(const char* cc) {
        memset(data, 0, len_User);
        memcpy(data, cc, strlen(cc));
    }
    char& operator [] (const int x) {
        return data[x];
    }
    const char& operator [] (const int x) const {
        return data[x];
    }
    friend bool operator < (const User_ID &a, const User_ID &b) {
        for(int i = 0; i < len_User; i++) if(a[i] != b[i]) return a[i] < b[i];
        return 0;
    }
    void clean() {
        memset(data, 0, len_User);
    }
    bool is_Null() {
        return !data[0];
    }
};

struct User {
    char id[len_User], password[len_User], name[len_User];
    int pri;
    User() {
        memset(id, 0, len_User), memset(password, 0, len_User), memset(name, 0, len_User), pri = 0;
    }
    User(const char* uid, const char* pswd, const char* nme, const int &prii) {
        memset(id, 0, len_User), memset(password, 0, len_User), memset(name, 0, len_User), pri = prii;
        memcpy(id, uid, strlen(uid)), memcpy(password, pswd, strlen(pswd)), memcpy(name, nme, strlen(nme));
    }
    User_ID get_id() {
        User_ID ret;
        memcpy(ret.data, id, len_User);
        return ret;
    }
    bool check_Password(const char* p) {
        const int sp = strlen(p);
        if(sp != strlen(password)) return 0;
        for(int i = 0; i < sp; i++) if(p[i] != password[i]) return 0;
        return 1;
    }
    void fill_Password(const char* p) {
        memset(password, 0, len_User);
        memcpy(password, p, strlen(p));
    }
};

class Bookstore {
private:
    database_cached<ISBN, Book> db_book;
    database_cached<Other_Data, ISBN> index_name, index_author, index_keyword;
    database_cached<User_ID, User> db_user;
    database_cached<int, pair<long double, long double> > db_finance;
    database_cached<int, long double> db_metadata;
    int current_Opt;
    long double sum_positive, sum_negative;
    vector<pair<User_ID, int> > login_stack;
    vector<ISBN> select_stack;

    void erase_Book(const ISBN isbn) {
        const auto info = db_book.query(isbn).second;
        db_book.erase(isbn);

        index_name.erase(info.get_name());
        index_author.erase(info.get_author());
        for(auto &t: info.get_keywords()) {
            index_keyword.erase(t);
        }
    }
    void add_book(const Book &book) {
        db_book.insert(book.get_ISBN(), book);

        index_name.insert(book.get_name(), book.get_ISBN());
        index_author.insert(book.get_author(), book.get_ISBN());
        for(auto &t: book.get_keywords()) {
            index_keyword.insert(t, book.get_ISBN());
        }
    }
    void save_Meta() {
        if(!db_metadata.query(1).first) {
            db_metadata.insert(1, current_Opt);
            db_metadata.insert(2, sum_positive);
            db_metadata.insert(3, sum_negative);
        } else {
            db_metadata.modify(1, current_Opt);
            db_metadata.modify(2, sum_positive);
            db_metadata.modify(3, sum_negative);
        }
        db_finance.insert(current_Opt, make_pair(sum_positive, sum_negative));
    }

public:
    int get_cur_pri() {
        if(login_stack.empty()) return 0;
        return login_stack.rbegin()->second;
    }
    bool su(const char* userid, const char* password) {
        User_ID temp(userid);
        auto info = db_user.query(userid);
        if(!info.first) return 0; // no such user.
        if(get_cur_pri() <= info.second.pri && !info.second.check_Password(password)) return 0; // wrong password.
        login_stack.push_back(make_pair(userid, info.second.pri));
        select_stack.push_back(ISBN());
        return 1;
    }
    bool logout() {
        if(login_stack.empty()) return 0;
        login_stack.pop_back();
        select_stack.pop_back();
        return 1;
    }
    bool useradd(const char* userid, const char* password, int pri, const char* name) {
        if(pri >= get_cur_pri()) return 0; // privilege error.
        if(db_user.query(User_ID(userid)).first) return 0; // same userid.
        User_ID uid(userid);
        User temp_User(userid, password, name, pri);
        db_user.insert(uid, temp_User);
        return 1;
    }
    bool reg(const char* userid, const char* password, const char* name) {
        if(db_user.query(User_ID(userid)).first) return 0; // same userid.
        User_ID uid(userid);
        User temp_User(userid, password, name, 0);
        db_user.insert(uid, temp_User);
        return 1;
    }
    bool del(const char* userid) {
        if(get_cur_pri() != 7) return 0; // not root
        User_ID uid(userid);
        for(auto &t: login_stack) {
            if(!(uid < t.first) && !(t.first < uid)) return 0; // cannot delete user in login stack!
        }
        db_user.erase(uid);
        return 1;
    }
    bool passwd(const char* userid, const char* pswd_old, const char* pswd_new) {
        User_ID uid(userid);
        auto t = db_user.query(uid);
        if(!t.first) return 0; // no such user.
        if(get_cur_pri() != 7 && !t.second.check_Password(pswd_old)) return 0; // not root and wrong password.
        auto t2 = t.second;
        t2.fill_Password(pswd_new);
        db_user.modify(uid, t2);
        return 1;
    }
    bool select(const char* isbn) {
        if(get_cur_pri() < 3) return 0; // privilege error.
        ISBN i(isbn);
        Book t;
        t.fill_ISBN(isbn);
        if(!db_book.query(isbn).first) add_book(t);
        *select_stack.rbegin() = i;
        return 1;
    }
    bool modify(const char* isbn, const char* name, const char* author, const char* keyword, const double price) {
        if(get_cur_pri() < 3) return 0; // privilege error.
        if(select_stack.rbegin()->is_null()) return 0; // selected null book.
        auto book = db_book.query(*select_stack.rbegin()).second;
        erase_Book(*select_stack.rbegin());
        *select_stack.rbegin() = ISBN(isbn);

        if(strlen(isbn)) book.fill_ISBN(isbn);
        if(strlen(name)) book.fill_name(name);
        if(strlen(author)) book.fill_author(author);
        if(strlen(keyword)) book.fill_keyword(keyword);
        if(price != -1) book.fill_price(price);

        return 1;
    }
    bool import(int qual, const double cost) {
        if(get_cur_pri() < 3) return 0; // privilege error.
        if(select_stack.rbegin()->is_null()) return 0; // selected null book.

        auto book = db_book.query(*select_stack.rbegin()).second;
        book.qual += qual;
        db_book.modify(book.get_ISBN(), book);

        ++current_Opt, sum_negative += cost;
        save_Meta();
    }
    enum Key {iSbN, NAME, AUTHOR, KEYWORD};
    pair<bool, vector<Book> > show(Key type, const char* data) {
        if(get_cur_pri() < 1) return make_pair(0, vector<Book>());
        vector<Book> ret;
        if(type == iSbN) {
            auto t = db_book.query(ISBN(data));
            if(t.first) ret.push_back(t.second);
        } else {
            auto st = Other_Data(data), ed = Other_Data(data);
            ed.construct_Max();
            auto ls = (type == NAME ? index_name : (type == AUTHOR ? index_author : index_keyword)).range(st, ed); // ???
            for(auto &t: ls) {
                auto book = db_book.query(t);
                assert(book.first);
                ret.push_back(book.second);
            }
        }
        return make_pair(1, ret);
    }

    pair<bool, pair<long double, long double> > show_finance(int time) {
        if(get_cur_pri() != 7 || current_Opt - time < 0) return make_pair(0, make_pair(0, 0)); // privilege error or time > opt.
        auto ref = time != -1 ? db_finance.query(current_Opt - time).second : make_pair((long double)0, (long double)0);
        return make_pair(1, make_pair(sum_positive - ref.first, sum_negative - ref.second));
    }

    pair<bool, double> buy(const char* isbn, const int quantity) {
        if(get_cur_pri() < 1) return make_pair(0, -1); // privilege error.
        auto info = db_book.query(ISBN(isbn));
        if(!info.first) return make_pair(0, -1); // no such book.
        auto book = info.second;
        if(quantity > book.qual) return make_pair(0, -1); // no enough books.

        book.qual -= quantity;
        db_book.modify(ISBN(isbn), book);
        ++current_Opt, sum_positive += quantity * book.price;
        save_Meta();

        return make_pair(1, quantity * book.price);
    }

    Bookstore(): db_book("db/db_book.bin"), index_name("db/index_name.bin"), index_author("db/index_author.bin"),
                 index_keyword("db/index_keyword.bin"), db_user("db/db_user.bin"), db_finance("db/db_finance.bin"),
                 db_metadata("db/db_metadata.bin") {
        auto t = db_metadata.query(1);
        if(t.first) { // exists
            current_Opt = t.second;
            sum_positive = db_metadata.query(2).second;
            sum_negative = db_metadata.query(2).second;
        } else {
            sum_positive = sum_negative = current_Opt = 0;
            save_Meta();
        }
    }
    void test() {}
};

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

class Interpreter { // todo: interactive interpreter
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
public:
    Interpreter() {
        string line;
        while((line = getline()) != "") {
            auto sp = split(line);
            if(sp[0] == "su") {
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
            } else if(sp[0] == "import") {
                if(sp.size() != 3) Invalid();
                else if(!valid.quantity(sp[1]) || !valid.price(sp[2])) Invalid();
                else if(!core.import(string2int(sp[1]), string2double(sp[2]))) Invalid();
            } else if(sp[0] == "show") {
                if(sp[1] == "finance") {
                    if(sp.size() > 3) Invalid();
                    else {
                        if(sp.size() == 3 && !valid.quantity(sp[2])) Invalid();
                        else {
                            int tim = -1;
                            if(sp.size() == 3) tim = string2int(sp[2]);
                            auto ret = core.show_finance(tim);
                            if(!ret.first) Invalid();
                            else printf("+ %0.2lf - %0.2lf\n", ret.second.first, ret.second.second);
                        }
                    }
                } else {
                }
            } else if(sp[0] == "buy") {
                if(sp.size() != 3) Invalid();
                else if(!valid.isbn(sp[1]) || !valid.quantity(sp[2])) Invalid();
                else {
                    auto t = core.buy(sp[1].c_str(), string2int(sp[2]));
                    if(!t.first) Invalid();
                    else printf("%0.2f\n", t.second);
                }
            } else Invalid();
        }
    }
};

#endif //BOOKSTORE_2020_EVERYTHING_HPP
