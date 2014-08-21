

#ifndef ___BlockDataManager_cpp__
#define ___BlockDataManager_cpp__
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

class BlockOutputManager{
private:
    char *path;
    unsigned int path_baselen;
    unsigned int line_count, current_block;
public:
    BlockOutputManager(const char *_path) : line_count(0), current_block(0){
        const int max_number_len = 11; //10 billion
        const int len = strlen(_path);
        path_baselen = len + 5; //strlen(_path + ".part")
        path = (char *) malloc((path_baselen + 1 + max_number_len) * sizeof(char));
        strcpy(path, _path);
        strcpy(path + len, ".part0");
    }
};

#endif


