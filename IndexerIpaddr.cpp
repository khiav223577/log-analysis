
#ifndef ___IndexerIpaddr_cpp__
#define ___IndexerIpaddr_cpp__

class IndexerIpaddr : public IndexerBase{
public:
    typedef std::map<unsigned int, bool> MAP;
    typedef MAP::iterator ITERATOR;
    typedef IndexerBase super;
private:
    MAP hashTable;
public:
    IndexerIpaddr(){
    }
    IndexerIpaddr(std::vector<unsigned int> &hashKeys){
        unsigned int size = hashKeys.size();
        for(unsigned int i = 0 ; i < size; ++i) RMap<MAP>::InsertKeyToMap(hashTable, hashKeys[i], true);
    }
    inline void addIndex(unsigned int value){
        hashTable[value] = true;
    }
//-------------------------------------------------------------------------
//  save / load / clear
//-------------------------------------------------------------------------
    void load(InputManager *inputer){
        unsigned int size = inputer->read_int();
        for(unsigned int i = 0 ; i < size; ++i){
            unsigned int key = inputer->read_int();
            RMap<MAP>::InsertKeyToMap(hashTable, key, true);
        }
    }
    void save(OutputManager *outputer){
        outputer->write(hashTable.size());
        for(ITERATOR it = hashTable.begin(); it != hashTable.end(); ++it) outputer->write(it->first);
    }
    void clear(){
        hashTable.clear();
    }
};

#endif

