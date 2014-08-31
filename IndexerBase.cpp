
#ifndef ___IndexerBase_cpp__
#define ___IndexerBase_cpp__
#define IndexList std::vector<IndexerBase*>
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
    InputIndexer(){
    }
    ~InputIndexer(){
        for(IndexList::iterator iter = indexList.begin(); iter != indexList.end(); ++iter) delete *iter;
        indexList.clear();
    }
//-------------------------------------------------------------------------
//  CORE
//-------------------------------------------------------------------------
    inline void push(IndexerBase *indexer){
        if (indexer == NULL) return;
        indexList.push_back(indexer);
    }
    inline void load(const char *filename, unsigned int mode = FILE_MODE_RAW){
        InputManager *inputer = new InputManager(filename, mode);
        for(IndexList::iterator iter = indexList.begin(); iter != indexList.end(); ++iter) (*iter)->load(inputer);
        delete inputer;

    }
};

#include "IndexerDate.cpp"
#include "IndexerIpaddr.cpp"

#endif

