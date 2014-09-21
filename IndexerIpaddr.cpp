
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
//-------------------------------------------------------------------------
//  ACCESS
//-------------------------------------------------------------------------
    bool hasValueEqualTo(int val){
        unsigned int uval = val;
        //printf("hasValueEqualTo: %u %s\n", uval, (hashTable.count(uval) == 1) ? "true" : "false");
        return (hashTable.count(uval) == 1);
    }
    bool hasValueBetween(int val1, int val2){ //val1 <= val2
        unsigned int uval1 = val1, uval2 = val2;
        if (uval1 > uval2) return false;
        PERROR(true, printf("hasValueBetween has not been implemented in IndexerIpaddr.");); //TODO
        return true;
    }
};

#endif

