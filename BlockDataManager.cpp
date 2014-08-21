

#ifndef ___BlockDataManager_cpp__
#define ___BlockDataManager_cpp__
#include<string.h>
#include<stdlib.h>
#include "FileIOManager.cpp"
class BlockConfig{
public:
    unsigned int line_count, block_size;
    BlockConfig(unsigned int _line_count, unsigned int _block_size){
        line_count = _line_count;
        block_size = _block_size;
    }
};

class BlockOutputManager{
private:
    char *path;
    unsigned int path_baselen, line_count, current_block;
    const unsigned int block_size, file_mode;
    OutputManager *outputer;
    void (*onBlockChange)(OutputManager *);
private:
    inline void nextBlock(){ setBlockIndex(current_block + 1); }
    inline void setBlockIndex(unsigned int idx){
        current_block = idx;
        sprintf(path + path_baselen, "%d", current_block);
        delete outputer;
        onBlockChange(outputer = new OutputManager(path, file_mode));
    }
public:
    BlockOutputManager(const char *_path, unsigned int bs, unsigned int fm, void (*callback)(OutputManager *)) :
            line_count(0), block_size(bs), file_mode(fm), outputer(NULL), onBlockChange(callback){
        static const int max_number_len = 11; //10 billion
        const int len = strlen(_path);
        path_baselen = len + 5; //strlen(_path + ".part")
        path = (char *) malloc((path_baselen + 1 + max_number_len) * sizeof(char));
        strcpy(path, _path);
        strcpy(path + len, ".part");
        setBlockIndex(0);
    }
    ~BlockOutputManager(){
        delete outputer;
    }
    inline OutputManager *  getOutputManager(){ return outputer; }
    inline unsigned int         getLineCount(){ return line_count; }
    inline void                     nextLine(){ if (++line_count % block_size == 0) nextBlock(); }
    inline BlockConfig *   createBlockConfig(){ return new BlockConfig(line_count, block_size); }
};

#endif


