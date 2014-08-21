#ifndef ___HashCompressor_cpp__
#define ___HashCompressor_cpp__
#include "RMap.cpp"
#define STR_MAXLEN 1023
template<typename KEY_TYPE, typename MAP_TYPE> class HashCompressor{
protected:
    MAP_TYPE hashTable;
    std::vector<KEY_TYPE> hashKeys;
    unsigned int hashValueCounter;
    KEY_TYPE key_type;

public:
    HashCompressor() : hashValueCounter(0){
    }
    ~HashCompressor(){
        free_memory(key_type);
    }
//------------------------------------------------------------
//  ACCESS
//------------------------------------------------------------
public:
    inline unsigned int get_hashValueCounter(){ return hashValueCounter; }
//------------------------------------------------------------
//  compress
//------------------------------------------------------------
public:
    inline unsigned int compress(          int input){ return inner_compress(input);}
    inline unsigned int compress( unsigned int input){ return inner_compress(input);}
    inline unsigned int compress(         char input){ return inner_compress(input);}
    inline unsigned int compress(unsigned char input){ return inner_compress(input);}
    inline unsigned int compress(char *input){
        int mem_size = (strlen(input) + 1) * sizeof(char);
        char *key = (char *) malloc(mem_size);
        memcpy(key, input, mem_size);
        return inner_compress(key);
    }
protected:
    inline unsigned int inner_compress(KEY_TYPE input){
        unsigned int value = RMap<MAP_TYPE>::InsertKeyToMap(hashTable, input, hashValueCounter);
        if (value == hashValueCounter){
            hashKeys.push_back(input);
            hashValueCounter += 1; //input is a new key!
        }
        return value;
    }
//------------------------------------------------------------
//  decompress
//------------------------------------------------------------
public:
    inline KEY_TYPE decompress(unsigned int input){
        return hashKeys[input];
    }
//------------------------------------------------------------
//  free_memory
//------------------------------------------------------------
protected:
    inline void free_memory(          int ele){}
    inline void free_memory( unsigned int ele){}
    inline void free_memory(         char ele){}
    inline void free_memory(unsigned char ele){}
    inline void free_memory(char *ele){

        RMap<MAP_TYPE>::FreeClearMap_1(hashTable);
    }
//------------------------------------------------------------
//  save
//------------------------------------------------------------
public:
    inline void save(FILE *file){
        fprintf(file, " %u\n", hashTable.size());
        inner_save(file, key_type);
    }
protected:
    inline void inner_save(FILE *file, unsigned int unused){
        for(typename MAP_TYPE::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
            fprintf(file, "%x,%x\n", it->second, it->first);
        }
    }
    inline void inner_save(FILE *file, char *unused){
        for(typename MAP_TYPE::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
            fprintf(file, "%x,%s\n", it->second, it->first);
        }
    }
//------------------------------------------------------------
//  load
//------------------------------------------------------------
public:
    inline void load(FILE *file){
        fscanf(file, " %u\n", &hashValueCounter);
        hashKeys = std::vector<KEY_TYPE>(hashValueCounter);
        inner_load(file, key_type);
    }
protected:
    inline void inner_load(FILE *file, unsigned int unused){
        for(unsigned int idx, i = 0; i < hashValueCounter; ++i){
            unsigned int input;
            fscanf(file, "%x,%x\n", &idx, &input);
            hashKeys[idx] = input;
            RMap<MAP_TYPE>::InsertKeyToMap(hashTable, input, idx);
        }
    }
    inline void inner_load(FILE *file, char *unused){
        for(unsigned int idx, i = 0; i < hashValueCounter; ++i){
            fscanf(file, "%x,", &idx);
            const char *input_string = SafeScanf::readHashString(file).c_str();
            fscanf(file, "\n");
            int byte_size = (strlen(input_string) + 1) * sizeof(char);
            char *input = (char *) malloc(byte_size);
            memcpy(input, input_string, byte_size);
            hashKeys[idx] = input;
            RMap<MAP_TYPE>::InsertKeyToMap(hashTable, input, idx);
        }
    }
};



#undef STR_MAXLEN
#endif


