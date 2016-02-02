//
//  p3parser.h
//  parser
//
//  Created by Gerard Ryan on 11/15/15.
//  Copyright Â© 2015 Gerard Ryan. All rights reserved.
//

#ifndef p3parser_h
#define p3parser_h

#include "p2lex.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class Value;

extern int errcnt;


class PTree {
protected:
    int     line;
    PTree   *left;
    PTree   *right;
    
public:
    PTree(int line, PTree *left = 0, PTree *right = 0) {
        this->line = line;
        this->left = left;
        this->right = right;
    }
    
    
    virtual void findEmptyStrings() {
        if (left) {
            left->findEmptyStrings();
        }
        if (right) {
            right->findEmptyStrings();
        }
    }
    virtual void findInvalidOps() {
        if (left) {
            left->findInvalidOps();
        }
        if (right) {
            right->findInvalidOps();
        }
    }
    
    enum Type { T_STRING, T_SEARCHRESULT, T_NONE };
    
    //check if left child exists
    bool leftNode(){
        if(left){
            return true;
        }
        return false;
    }
    //check if right child exists
    bool rightNode(){
        if(right){
            return true;
        }
        return false;
    }
    
    virtual Type getType() { return T_NONE; }
    
    virtual Value   *eval(); // not used in p3
};

class PTreeStmtList : public PTree {
public:
    PTreeStmtList(int line, PTree *s1, PTree *s2 = 0) : PTree(line, s1, s2) {}
    
    virtual Value   *eval(); // not used in p3
};

class PTreePrint : public PTree {
public:
    PTreePrint(int line, PTree *expr) : PTree(line, expr) {}
    
    Value   *eval(); // not used in p3
};

class PTreeSet : public PTree {
    string id;
public:
    PTreeSet(int line, string id, PTree *expr) : PTree(line, expr) { this->id = id; }
    
    Value   *eval(); // not used in p3
};

class PTreeUnion : public PTree {
public:
    PTreeUnion(int line, PTree *s1, PTree *s2) : PTree(line, s1, s2) {}
    
    Type getType() {
        Type tl = left->getType();
        Type tr = right->getType();
        
        if( tl == T_NONE || tr == T_NONE || tl == tr )
            return T_NONE;
        return T_SEARCHRESULT;
    }
    
    void findInvalidOps() {
        left->findInvalidOps();
        right->findInvalidOps();
        if (getType() != T_SEARCHRESULT) {
            cerr << "Invalid union operator for given types at line " << line << endl;
            errcnt++;
        }
    }
    
    Value   *eval(); // not used in p3
};

class PTreeIntersect : public PTree {
public:
    PTreeIntersect(int line, PTree *s1, PTree *s2) : PTree(line,s1, s2) {}
    
    virtual Type getType() {
        Type tl = left->getType();
        Type tr = right->getType();
        
        if( tl == T_NONE || tr == T_NONE || tl == tr )
            return T_NONE;
        return T_SEARCHRESULT;
    }
    
    void findInvalidOps() {
        left->findInvalidOps();
        right->findInvalidOps();
        if (getType() != T_SEARCHRESULT) {
            cerr << "Invalid intersect operator for given types at line " << line << endl;
            errcnt++;
        }
    }
    
    Value   *eval(); // not used in p3
};

class PTreeID : public PTree {
    string id;
public:
    PTreeID(int line, string id) : PTree(line) { this->id = id; }
    
    Value   *eval(); // not used in p3
};

class PTreeSTR : public PTree {
    string val;
public:
    PTreeSTR(int line, string val) : PTree(line) {
        this->val = val;
    }
    
    void findEmptyStrings() {
        if (val.length() == 0) {
            cerr << "Empty string at line " << line << endl;
            errcnt++;
        }
    }
    
    virtual Type getType() { return T_STRING; }
    
    Value   *eval(); // not used in p3
};

class PTreeSearch : public PTree {
    string filename;
    string pattern;
public:
    PTreeSearch(int line, string f, string p) : PTree(line), filename(f), pattern(p) {}
    
    void findEmptyStrings() {
        if (filename.length() == 0 || pattern.length() == 0) {
            cout << "Empty string at line " << line << endl;
        }
    }
    
    virtual Type getType() { return T_SEARCHRESULT; }
    
    Value   *eval(); // not used in p3
};


extern PTree *Program(istream *br);

#endif /* p3parser_h */