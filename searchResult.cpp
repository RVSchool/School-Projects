//
//  searchResult.cpp
//  parser
//
//  Created by Gerard Ryan on 10/25/15.
//  Copyright Â© 2015 Gerard Ryan. All rights reserved.
//

#include "searchResult.h"
#include <fstream>
using namespace std;

namespace SearchTool {
    
    SearchResult::~SearchResult()
    {
        while (results.size()) {
            delete results.front();
            results.pop_front();
        }
    }
    
    bool
    SearchResult::searched() {
        return didSearch;
    }
    
    bool
    SearchResult::dosearch(string& searchfor)
    {
        string line;
        int lnum = 0;
        
        ifstream infile;
        infile.open(file_to_search);
        
        if (!infile.is_open()) {
            return false;
        }
        
        vector<string> found;
        found.push_back(searchfor);
        
        for (;;) {
            ++lnum;
            getline(infile, line);
            if (!infile.good()) {
                break;
            }
            
            if( line.find(searchfor) != string::npos ) {
                // found one! save it in this object and go
                SearchResultEntry *ent = new SearchResultEntry(lnum, found, line);
                if (ent) {
                    results.push_back(ent);
                }
            }
        }
        infile.close();
        
        didSearch = true;
        return true;
    }
    

}