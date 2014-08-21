

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
    char *path;
    unsigned int path_baselen, line_count, current_block;
    const unsigned int block_size, file_mode;
    XXXXX *manager;
    void (*onBlockChange)(XXXXX *);
private:
    inline void nextBlock(){ setBlockIndex(current_block + 1); }
    inline void setBlockIndex(unsigned int idx){
        current_block = idx;
        sprintf(path + path_baselen, "%d", current_block);
        delete manager;
        onBlockChange(manager = new XXXXX(path, file_mode));
    }
public:
    BlockIOManager(const char *_path, unsigned int bs, unsigned int fm, void (*callback)(XXXXX *)) :
            line_count(0), block_size(bs), file_mode(fm), manager(NULL), onBlockChange(callback){
        static const int max_number_len = 11; //10 billion
        const int len = strlen(_path);
        path_baselen = len + 5; //strlen(_path + ".part")
        path = (char *) malloc((path_baselen + 1 + max_number_len) * sizeof(char));
        strcpy(path, _path);
        strcpy(path + len, ".part");
        setBlockIndex(0);
    }
    ~BlockIOManager(){
        delete manager;
    }
    inline XXXXX *       getIOManager(){ return manager; }
    inline unsigned int  getLineCount(){ return line_count; }
    inline void              nextLine(){ if (++line_count % block_size == 0) nextBlock(); }
    inline BlockConfig * createConfig(){ return new BlockConfig(line_count, block_size); }
};

#endif


