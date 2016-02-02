//
//  p3eval
//

//

#include "p3parser.h"
#include <string>
#include "p4value.h"

#include "searchResult.h"

#include<map>
#include<list>
#include <set>

//this map checks if ID has been assigned in general
//values: 1 corresponds to String, 2 corresponds to SearchResult
map<string,int> declaredIDs;
//Key: ID, Value: String
map<string,string> stringIDs;
//Key: ID, Value: Search Result
map<string,SearchTool::SearchResult*>searchIDs;


Value *
PTree::eval()
{
    return 0;
}

Value *
PTreeStmtList::eval()
{
    //check if a left child exists
    if(this->leftNode()){
        left->eval();
    }
    //check if right child exists
    if(this->rightNode()){
        right->eval();
    }
    return 0;
}

//set the ID to variable
Value *
PTreeSet::eval()
{
    Value *v = 0;
    
    if(leftNode()){
        v = left->eval();
    }
    if(rightNode()){
        v = right->eval();
    }
    
    //if(declaredIDs.find(this->id) == declaredIDs.end()){
        if(v->getType() == T_STRING){
            declaredIDs[this->id] = 1;
            string str = v->getStringValue();
            stringIDs[this->id] = str;

        }
        else if(v->getType() == T_SEARCHRESULT){
            declaredIDs[this->id] = 2;
            SearchTool::SearchResult *sr = v->getSearchValue();
            searchIDs[this->id] = sr;
        }
        else{
            cout << "Expected type" << endl;
        }
   // }
    return 0;
}

//printing the variable
Value *
PTreePrint::eval()
{
    string s;

    
    Value *v = 0;

    if(leftNode()){
        v = left->eval();
    }
    if(rightNode()){
        v = right->eval();
    }
    if(v){
        if(v->getType() == T_STRING){
            s = v->getStringValue();
            cout << s << endl;
        }
    if(v->getType() == T_SEARCHRESULT){
        list<SearchTool::SearchResultEntry *>searchResults;
        searchResults = v->getSearchValue()->getResults();
        while(searchResults.size()){
            cout << v->getSearchValue()->getFile() << ":" << searchResults.front()->getLinenum() << ":" << searchResults.front()->getLine() << endl;
            searchResults.pop_front();
            
        }
    }
    }
    return 0;
}

//create the union
//merge the first search result with a search result based on string
Value *
PTreeUnion::eval()
{
    Value *leftVal;
    Value *rightVal;
    
    if(leftNode()){
    leftVal = left->eval();
    }
    else{
        cout << "Missing value" << endl;
        return 0;
    }
    if(rightNode()){
    rightVal = right->eval();
    }
    else{
        cout << "Missing value" << endl;
        return 0;
    }
    string s;
    
    //expecting two children, error if one or more is missing
    if(leftVal && rightVal){
        //left value string, right value search result
    if(leftVal->getType() == T_STRING){
        if(rightVal->getType() == T_SEARCHRESULT){
            return new ValueSearchResult(rightVal->getSearchValue()->operator+(leftVal->getStringValue()));
        }
        else{
            cout << "Type error performing UNION operator at line " << this->line << endl;
            return 0;
        }
        
    }
    else if(leftVal->getType() == T_SEARCHRESULT){
        if(rightVal->getType() == T_STRING){
          return new ValueSearchResult(leftVal->getSearchValue()->operator+(rightVal->getStringValue()));
        }
        else{
            cout << "Type error performing UNION operator at line " << this->line << endl;
            return 0;
        }
    }
    }
    else{
        cout << "Type error performing UNION operator at line " << this->line << endl;
        return 0;
    }
    return 0;
    
}

//create the intersect
//delete any lines from initial search result that do not include intersect string
Value *
PTreeIntersect::eval()
{
    Value *leftVal;
    Value *rightVal;
    if(leftNode()){
        leftVal = left->eval();
    }
    else{
        cout << "Missing value" << endl;
        return 0;
    }
    if(rightNode()){
        rightVal = right->eval();
    }
    else{
        cout << "Missing value" << endl;
        return 0;
    }
    string s;
    
    if(leftVal && rightVal){
    if(leftVal->getType() == T_STRING){
        if(rightVal->getType() == T_SEARCHRESULT){
            return new ValueSearchResult(rightVal->getSearchValue()->operator^(leftVal->getStringValue()));
        }
        else{
            cout << "Type error performing INTERSECTION operator at line " << this->line << endl;
            return 0;
        }
     
    }
    else if(leftVal->getType() == T_SEARCHRESULT){
     
        if(rightVal->getType() == T_STRING){
         
            return new ValueSearchResult(leftVal->getSearchValue()->operator^(rightVal->getStringValue()));
        }
        else{
            cout << "Type error performing INTERSECTION operator at line " << this->line << endl;
            return 0;
        }
        
    }
    }
    else{
        cout << "Type error performing INTERSECTION operator at line " << this->line << endl;
        return 0;
    }
    return 0;
    
}

Value *
PTreeID::eval()
{
    //if ID is found
    if(declaredIDs.find(this->id) != declaredIDs.end()){
        int type = declaredIDs[this->id];
        //if ID holds a string
        if(type == 1){
            string s = stringIDs[this->id];
            return new ValueString(s);
        }
        //if ID holds a searchresult
        else if(type == 2){
            SearchTool::SearchResult *sr = searchIDs[this->id];
            return new ValueSearchResult(sr);
        }
    }
    else{
        cout << "Variable " << this->id << " not set before use at line " << this->line << endl;
        return 0;
    }
    
    return 0;
}

//evaluate the string
Value *
PTreeSTR::eval()
{
    string s = this->val;
    
    return new ValueString(s);
}

//evaluate the search result
Value *
PTreeSearch::eval()
{
    ValueSearchResult search = new SearchTool::SearchResult(this->filename);
    search.getSearchValue()->dosearch(this->pattern);
    if(search.getSearchValue()->searched()){
        return new ValueSearchResult(search);
    }
    return 0;
}
