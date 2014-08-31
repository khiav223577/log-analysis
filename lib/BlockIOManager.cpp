

#ifndef ___BlockIOManager_cpp__
#define ___BlockIOManager_cpp__
#include<string.h>
#include<stdlib.h>
class BlockConfig{
public:
    unsigned int line_count, block_size;
    BlockConfig(unsigned int _line_count, unsigned int _block_size){
        line_count = _line_count;
        block_size = _block_size;
    }
};
template<typename XXXXX> class BlockIOManager{
private:
    char *current_path;
    unsigned int path_baselen, line_count, current_block;
    const unsigned int block_size, file_mode;
    XXXXX *manager;
    void (*onBlockChange)(BlockIOManager<XXXXX>&);
private:
    inline void nextBlock(){ setBlockIndex(current_block + 1); }
    inline void setBlockIndex(unsigned int idx){
        current_block = idx;
        sprintf(current_path + path_baselen, "%d", current_block);
        delete manager;
        manager = new XXXXX(current_path, file_mode);
        onBlockChange(*this);
    }
public:
    BlockIOManager(const char *_path, unsigned int bs, unsigned int fm, void (*callback)(BlockIOManager<XXXXX>&)) :
            line_count(0), current_block(-1), block_size(bs), file_mode(fm), manager(NULL), onBlockChange(callback){
        static const int max_number_len = 11; //10 billion
        const int len = strlen(_path);
        path_baselen = len + 5; //strlen(_path + ".part")
        current_path = (char *) malloc((path_baselen + 1 + max_number_len) * sizeof(char));
        strcpy(current_path, _path);
        strcpy(current_path + len, ".part");
    }
    ~BlockIOManager(){
        free(current_path);
        delete manager;
        manager = NULL;
        if (line_count % block_size != 1) onBlockChange(*this); //To output index(Note that current_block doesn't change)
        //remove(current_path);
    }
    inline XXXXX *         getIOManager(){ return manager; }
    inline unsigned int    getLineCount(){ return line_count; }
    inline void                nextLine(){ if (++line_count % block_size == 1) nextBlock(); }
    inline BlockConfig *   createConfig(){ return new BlockConfig(line_count, block_size); }
    inline const char *  getCurrentPath(){ return current_path; }
    inline unsigned int getCurrentBlock(){ return current_block; }
};

#endif


