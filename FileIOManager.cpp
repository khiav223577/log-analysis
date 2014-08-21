
#ifndef ___FileIOManager_cpp__
#define ___FileIOManager_cpp__
#define OUTPUT_MANAGER_BUFFER_SIZE 256
#include<stdio.h>
#include<stdlib.h>


FILE *fopen2(const char *filename, const char *mode){
    FILE *f = fopen(filename,mode);
    PERROR(f == NULL, printf("Cannot open %s.",filename); );
    return f;
}

class OutputManager{
private:
    FILE *file;
    int buffer[OUTPUT_MANAGER_BUFFER_SIZE], buffer_counter;
public:
    OutputManager(const char *filename){
        file = fopen2(filename, "wb");
        buffer_counter = 0;
    }
    ~OutputManager(){
        flush();
        fclose(file);
    }
    inline void write(int data){
        buffer[buffer_counter++] = data;
        if (buffer_counter == OUTPUT_MANAGER_BUFFER_SIZE) flush();
    }
    inline void flush(){
        fwrite(buffer, sizeof(int), buffer_counter, file);
        buffer_counter = 0;
    }
};



#undef OUTPUT_MANAGER_BUFFER_SIZE
#endif
