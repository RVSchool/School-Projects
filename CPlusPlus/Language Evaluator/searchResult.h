//
//  searchResult.h
//  parser
//

#ifndef searchResult_h
#define searchResult_h

#include <string>
#include <istream>
#include <vector>
#include <list>
using namespace std;

namespace SearchTool {
    
    class SearchResultEntry {
        long              linenum;    // line number where match was found
        string            line;       // the lines that were matched
        vector<string>    patterns;   // strings that are found in "match"
        
    public:
        SearchResultEntry(long ln, vector<string> s, string l) {
            linenum = ln;
            line = l;
            patterns = s;
        }
        long getLinenum() { return linenum; }
        string getLine() { return line; }
        vector<string>& getPatterns() { return patterns; }
    };
    
    class SearchResult {
        string				file_to_search;
        list<SearchResultEntry *>  results;
        bool                            didSearch;
        
    public:
        SearchResult(string filename) : file_to_search(filename), didSearch(false) {}
        
        ~SearchResult();
        
        string getFile() { return file_to_search; }
        
        bool searched();
        bool dosearch(string& searchfor);
        
        list<SearchResultEntry *>& getResults() { return results; }
        
        SearchResult *operator+(string str){
            SearchResult *unionResult = new SearchResult(file_to_search);
            unionResult->dosearch(str);
   
            
            list<SearchResultEntry *> initResults = this->results;
            list<SearchResultEntry *> mergeResults = unionResult->results;
            list<SearchResultEntry *> newResults;
            
     
            //merge arrays in line number order
            while((initResults.size() != 0) || (mergeResults.size() != 0)){
               
      
                if((initResults.size() == 0)){
                    while((mergeResults.size()) != 0){
                        newResults.push_back(mergeResults.front());
                        mergeResults.pop_front();
                        
                    }
                    break;
                }
                else if((mergeResults.size() == 0)){

                    while((initResults.size()) != 0){
                        newResults.push_back(initResults.front());
                        initResults.pop_front();
                       
                    }
                    break;
                }
          
               if(initResults.front()->getLinenum() == mergeResults.front()->getLinenum()){

                    newResults.push_back(initResults.front());
                    initResults.pop_front();
                    mergeResults.pop_front();
                    continue;
                }
                if(initResults.front()->getLinenum() < mergeResults.front()->getLinenum()){

                    newResults.push_back(initResults.front());
                    initResults.pop_front();
                    continue;
                }
                else{

                    newResults.push_back(mergeResults.front());
                    mergeResults.pop_front();
                    continue;
                }
          
               
            }
            
            newResults.unique();
            unionResult->results = newResults;
            return unionResult;
        }
      
        //intersect operator
        SearchResult *operator^(string str){
            
            SearchResult *intResult = new SearchResult(this->getFile());
            intResult->dosearch(str);
            list<SearchResultEntry *>::iterator entries;
            list<SearchResultEntry *>::iterator ent;
            
            list<SearchResultEntry *> intArray = intResult->getResults();
            list<SearchResultEntry *> compArray = this->getResults();
            list<SearchResultEntry *> intersected;
            //compare arrays for the same line
            for(entries = intArray.begin(); entries != intArray.end(); entries++){
                for(ent = compArray.begin(); ent != compArray.end(); ent++){
                    if((*entries)->getLinenum() == (*ent)->getLinenum()){
                        intersected.push_back((*entries));
                        break;
                    }
                }
            }
            if(intResult->searched()){
                intResult->results = intersected;
                return intResult;
            }
            
            return 0;
        }
    };
    
}
#endif /* searchResult_h */