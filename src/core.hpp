//
// Created by Sion on 2020/10/7.
//

#ifndef BOOKSTORE_2020_CORE_HPP
#define BOOKSTORE_2020_CORE_HPP

#include <iostream>
#include <cstdio>
#include <cassert>
#include <algorithm>
#include <vector>
#include <stack>
#include <map>
#include "types.hpp"
#include "database_cached.hpp"
#ifndef debug
#define debug cerr
#endif
using namespace std;
using __Amagi::database_cached; // B+ Tree with rb-tree cache.

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
        // debug << "new book = " << book.isbn << " " << book.name << book.author << " " << book.keyword << " " << book.price << endl;
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
        User temp_User(userid, password, name, 1);
        db_user.insert(uid, temp_User);
        return 1;
    }
    bool del(const char* userid) {
        if(get_cur_pri() != 7) return 0; // not root
        User_ID uid(userid);
        if(!db_user.query(uid).first) return 0; // no such user
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
        if(strlen(isbn) && db_book.query(ISBN(isbn)).first) return 0; // same isbn exists.
        auto book = db_book.query(*select_stack.rbegin()).second;

        // debug << "old book = " << book.isbn << " " << book.name << " " << book.author << " " << book.keyword << " " << book.price << endl;

        erase_Book(*select_stack.rbegin());

        if(strlen(isbn)) {
            ISBN old_ISBN = *select_stack.rbegin();
            ISBN new_ISBN = ISBN(isbn);
            for(unsigned i = 0; i < select_stack.size(); i++) {
                if(!(select_stack[i] < old_ISBN) && !(old_ISBN < select_stack[i]))
                    select_stack[i] = new_ISBN;
            }
            book.fill_ISBN(isbn);
        }
        if(strlen(name)) book.fill_name(name);
        if(strlen(author)) book.fill_author(author);
        if(strlen(keyword)) book.fill_keyword(keyword);
        if(price != -1) book.fill_price(price);

        add_book(book);

        return 1;
    }
    bool import(int quan, const double cost) {
        if(get_cur_pri() < 3) return 0; // privilege error.
        if(select_stack.rbegin()->is_null()) return 0; // selected null book.

        auto book = db_book.query(*select_stack.rbegin()).second;
        book.quan += quan;
        db_book.modify(book.get_ISBN(), book);

        ++current_Opt, sum_negative += cost;
        save_Meta();
        return 1;
    }
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

    vector<Book> showall() {
        auto st = ISBN(), ed = ISBN();
        ed.construct_Max();
        auto ls = db_book.range(st, ed);
        vector<Book> ret;
        for(auto t: ls) ret.push_back(t);
        return ret;
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
        if(quantity > book.quan) return make_pair(0, -1); // no enough books.

        book.quan -= quantity;
        db_book.modify(ISBN(isbn), book);
        ++current_Opt, sum_positive += quantity * book.price;
        save_Meta();

        return make_pair(1, quantity * book.price);
    }

    Bookstore(): db_book("storage_db_book.bin"), index_name("storage_index_name.bin"), index_author("storage_index_author.bin"),
                 index_keyword("storage_index_keyword.bin"), db_user("storage_db_user.bin"), db_finance("storage_db_finance.bin"),
                 db_metadata("storage_db_metadata.bin") {
        auto t = db_metadata.query(1);
        if(t.first) { // exists
            current_Opt = t.second;
            sum_positive = db_metadata.query(2).second;
            sum_negative = db_metadata.query(3).second;
        } else {
            sum_positive = sum_negative = current_Opt = 0;
            save_Meta();
            User_ID uid("root");
            User temp_User("root", "sjtu", "root", 7);
            db_user.insert(uid, temp_User);
        }
    }
};

#endif //BOOKSTORE_2020_CORE_HPP
