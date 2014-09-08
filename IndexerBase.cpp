
#ifndef ___IndexerBase_cpp__
#define ___IndexerBase_cpp__
#define IndexList std::vector<IndexerBase*>
#define InputIndexList std::vector<InputIndexer*>
class IndexerBase{
public:
    IndexerBase(){
    }
    virtual void load(InputManager *inputer)=0;
};

class InputIndexer{
private:
    IndexList indexList;
public:
    std::vector<InputIndexer*> children;
public:
    InputIndexer(){
    }
    ~InputIndexer(){
        for(     IndexList::iterator iter = indexList.begin(); iter != indexList.end(); ++iter) delete *iter;
        for(InputIndexList::iterator iter =  children.begin(); iter !=  children.end(); ++iter) delete *iter;
        indexList.clear();
        children.clear();
    }
//-------------------------------------------------------------------------
//  CORE
//-------------------------------------------------------------------------
    inline void push(IndexerBase *indexer){
        if (indexer == NULL) return;
        indexList.push_back(indexer);
    }
    inline void load(InputManager *inputer){
        for(IndexList::iterator iter = indexList.begin(); iter != indexList.end(); ++iter) (*iter)->load(inputer);
    }
};

#include "IndexerDate.cpp"
#include "IndexerIpaddr.cpp"

#endif

