
#ifndef ___FileIOManager_cpp__
#define ___FileIOManager_cpp__
#define OUTPUT_MANAGER_BUFFER_SIZE 256
#define SIZE_OF_UINT sizeof(unsigned int)
//DATA >>= (SIZE_OF_UINT << 3); will trigger compiler warning....
#define CHECK_AND_WRITE_BLOCK(DATA, INDEX) \
    if (sizeof(DATA) > (INDEX * SIZE_OF_UINT)){ \
        write((unsigned int) DATA); \
        DATA >>= (SIZE_OF_UINT << 2);\
        DATA >>= (SIZE_OF_UINT << 2);\
    }
#include<stdio.h>
#include<stdlib.h>
#include "FlexibleInt.cpp"
#include <ruby.h> // it defines F_OK
FILE *fopen2(const char *filename, const char *mode){
    FILE *f = fopen(filename,mode);
    PERROR(f == NULL, printf("Cannot open %s",filename); );
    return f;
}
bool file_exists(const char *filename){
    return (access(filename, F_OK) != -1);
}

class OutputManager{
private:
    FILE *file;
    unsigned int buffer[OUTPUT_MANAGER_BUFFER_SIZE], buffer_counter, char_counter;
public:
    OutputManager(const char *filename){
        file = fopen2(filename, "wb");
        buffer_counter = 0;
        char_counter = 0;
    }
    ~OutputManager(){
        flush();
        fclose(file);
    }
//-------------------------------------------
//  Core
//-------------------------------------------
    inline void write( int data){ write((unsigned  int) data); }
    inline void write(char data){ write((unsigned char) data); }
    inline void write(long data){ write((unsigned long) data); }
    inline void write(unsigned char data){
        unsigned int fourBytes = data;
        buffer[buffer_counter] |= (fourBytes << (char_counter << 3)); //data << (0, 8, 16, or 24)
        if (++char_counter != 4) return;
        clear_char_counter();
    }
    inline void write(unsigned int data){
        clear_char_counter();
        buffer[buffer_counter] = data;
        plus_buffer_counter();
    }
    inline void write(unsigned long data){
        CHECK_AND_WRITE_BLOCK(data, 0);
        CHECK_AND_WRITE_BLOCK(data, 1); //for the compatibility when "long" is not 4-bytes.
        CHECK_AND_WRITE_BLOCK(data, 2);
        CHECK_AND_WRITE_BLOCK(data, 3);
        CHECK_AND_WRITE_BLOCK(data, 4);
        CHECK_AND_WRITE_BLOCK(data, 5);
        CHECK_AND_WRITE_BLOCK(data, 6);
        CHECK_AND_WRITE_BLOCK(data, 7);
        CHECK_AND_WRITE_BLOCK(data, 8);
    }
    inline void clear_char_counter(){
        if (char_counter == 0) return;
        char_counter = 0;
        plus_buffer_counter();
    }
    inline void plus_buffer_counter(){
        buffer_counter += 1;
        if (buffer_counter == OUTPUT_MANAGER_BUFFER_SIZE) flush();
    }
    inline void flush(){
        fwrite(buffer, sizeof(int), buffer_counter, file);
        buffer_counter = 0;
    }
//-------------------------------------------
//  Extend
//-------------------------------------------
    inline void write(FlexibleInt &data){
        if (data.isBigInt() == false){
            write(data.getValue());
            return;
        }
        BigInteger *bigInt = data.getValuePtr();
        unsigned long *buffer = bigInt->getBuffer();
        unsigned int buffer_len = bigInt->getBufferLen();
        write((unsigned int) (bigInt->getSign() + 1)); //0, 1, 2
        write(buffer_len);
        for(unsigned int i = 0; i < buffer_len; ++i) write(buffer[i]);
    }
};

class InputManager{
private:
    FILE *file;
public:
public:
    InputManager(const char *filename){
        file = fopen2(filename, "rb");
    }
    ~InputManager(){
        fclose(file);
    }
//-------------------------------------------
//  Core
//-------------------------------------------
    inline int read_int(){
        int data;
        fread(&data, sizeof(int), 1, file);
        return data;
    }
    inline long read_long(){
        long data;
        fread(&data, sizeof(long), 1, file);
        return data;
    }
//-------------------------------------------
//  Extend
//-------------------------------------------
    inline BigInteger *read_bigInt(){
        int sign = read_int() - 1;
        unsigned int buffer_len = (unsigned int) read_int();
        unsigned long *buffer = (unsigned long *) malloc(buffer_len * sizeof(unsigned long));
        fread(buffer, sizeof(unsigned long), buffer_len, file);
        BigInteger *bigInt = new BigInteger(buffer, buffer_len, BigInteger::Sign(sign));
        free(buffer);
        return bigInt;
    }
};

#undef CHECK_AND_WRITE_BLOCK
#undef OUTPUT_MANAGER_BUFFER_SIZE
#endif
