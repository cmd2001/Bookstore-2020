//
// Created by Sion on 2020/10/7.
//

#ifndef BOOKSTORE_2020_TYPES_HPP
#define BOOKSTORE_2020_TYPES_HPP

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
    void construct_Max() {
        memset(data, 127, len_ISBN); // default char type on x86_64 platform is SIGNED CHAR!
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
        memset(ISBN, 127, len_ISBN);
    }
};

struct Book {
    char isbn[len_ISBN], name[len_Other_Data], author[len_Other_Data], keyword[len_Other_Data];
    double price;
    int quan;
    Book() {
        memset(isbn, 0, len_ISBN), memset(name, 0, len_Other_Data), memset(author, 0, len_Other_Data), memset(keyword, 0, len_Other_Data);
        price = quan = 0;
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
        for(int i = 0, j, len = strlen(keyword); i < len;) { // j is the first position after '|'.
            for(j = i; j < len && keyword[j] != '|'; ++j) ;
            Other_Data r2;
            for(int k = 0; k < j - i; k++) r2.data[k] = keyword[i + k];
            memcpy(r2.ISBN, isbn, len_ISBN);
            ret.push_back(r2);
            i = j + 1;
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

enum Key {iSbN, NAME, AUTHOR, KEYWORD, PRICE};

#endif //BOOKSTORE_2020_TYPES_HPP
