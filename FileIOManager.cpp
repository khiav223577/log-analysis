
#ifndef ___FileIOManager_cpp__
#define ___FileIOManager_cpp__
#define OUTPUT_MANAGER_BUFFER_SIZE 4096
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
    unsigned char buffer[OUTPUT_MANAGER_BUFFER_SIZE];
    int buffer_counter, bit_counter;
public:
    OutputManager(const char *filename){
        file = fopen2(filename, "wb");
        buffer_counter = 0;
        bit_counter = 0;
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
        if (bit_counter == 0){
            buffer[buffer_counter] = data;
            plus_buffer_counter();
        }else{
            buffer[buffer_counter] |= (data << bit_counter);
            plus_buffer_counter();
            buffer[buffer_counter] |= (data >> (8 - bit_counter));
        }
    }
    inline void write(unsigned int data){
        write((unsigned char) (data >>  0)); //little endian
        write((unsigned char) (data >>  8));
        write((unsigned char) (data >> 16));
        write((unsigned char) (data >> 24));
    }
    inline void write1(unsigned int data){
        write((unsigned char) (data >>  0)); //little endian
    }
    inline void write2(unsigned int data){
        write((unsigned char) (data >>  0)); //little endian
        write((unsigned char) (data >>  8));
    }
    inline void write3(unsigned int data){
        write((unsigned char) (data >>  0)); //little endian
        write((unsigned char) (data >>  8));
        write((unsigned char) (data >> 16));
    }
    inline void write4(unsigned int data){
        write(data);
    }
    inline void write(unsigned int data, unsigned char byte_num){
        switch(byte_num){
        case 1:{write1(data); break;}
        case 2:{write2(data); break;}
        case 3:{write3(data); break;}
        case 4:{write4(data); break;}
        }
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
    inline void plus_buffer_counter(){
        buffer_counter += 1;
        if (buffer_counter == OUTPUT_MANAGER_BUFFER_SIZE) flush();
    }
    inline void flush(){
        fwrite(buffer, sizeof(char), buffer_counter, file);
        buffer_counter = 0;
    }
//-------------------------------------------
//  Extend
//-------------------------------------------
    inline void write(BigInteger *bigInt){
        unsigned long *buffer = bigInt->getBuffer();
        unsigned int buffer_len = bigInt->getBufferLen();
        write((unsigned char) (bigInt->getSign() + 1)); //0, 1, 2
        write(buffer_len);
        for(unsigned int i = 0; i < buffer_len; ++i) write(buffer[i]);
    }
    inline void write_n_byte_int(int value, unsigned char byte_num){
        switch(byte_num){
        case 1:{write1(value); break;}
        case 2:{write2(value); break;}
        case 3:{write3(value); break;}
        case 4:{write4(value); break;}
        }
    }
    inline void write_bits(unsigned int value, unsigned int bit_num){
        while(bit_num >= 8){
            write((unsigned char) value);
            value >>= 8;
            bit_num -= 8;
        }
        if (bit_num == 0) return;
        buffer[buffer_counter] |= (value << bit_counter);
        bit_counter += bit_num;
        if (bit_counter >= 8){
            bit_counter -= 8;
            plus_buffer_counter();
            buffer[buffer_counter] |= (value >> (bit_num - bit_counter)); //8 - bit_counter
        }
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
//  Utils
//-------------------------------------------
    inline int bytes_to_int_Little(unsigned char fourBytes[4]){
        return (fourBytes[0] <<  0) | (fourBytes[1] <<  8) | (fourBytes[2] << 16) | (fourBytes[3] << 24);
    }
    inline int bytes_to_int_Big(unsigned char fourBytes[4]){
        return (fourBytes[0] << 24) | (fourBytes[1] << 16) | (fourBytes[2] <<  8) | (fourBytes[3] <<  0);
    }
//-------------------------------------------
//  Core
//-------------------------------------------
    inline unsigned char read_char(){
        unsigned char data;
        fread(&data, sizeof(data), 1, file);
        return data;
    }
    inline int read_int(){
        unsigned char fourBytes[4];
        fread(fourBytes, sizeof(fourBytes), 1, file);
        return bytes_to_int_Little(fourBytes);
    }
    inline int read_int(unsigned char byte_num){
        unsigned char fourBytes[4] = {0};
        fread(fourBytes, sizeof(char), byte_num, file);
        return bytes_to_int_Little(fourBytes);
    }
    inline long read_long(){
        return (long) read_int(); //TODO: What to do when "long" is not 4-bytes.
    }
//-------------------------------------------
//  Extend
//-------------------------------------------
    inline BigInteger *read_bigInt(){
        int sign = read_char() - 1;
        unsigned int buffer_len = (unsigned int) read_int();
        unsigned long *buffer = (unsigned long *) malloc(buffer_len * sizeof(unsigned long));
        fread(buffer, sizeof(unsigned long), buffer_len, file);
        BigInteger *bigInt = new BigInteger(buffer, buffer_len, BigInteger::Sign(sign));
        free(buffer);
        return bigInt;
    }
    inline int read_n_byte_int(unsigned char byte_num){
        switch(byte_num){
        case 1:{int val = read_int(1); return (val >     127 ? val -      256 : val); }
        case 2:{int val = read_int(2); return (val >   32767 ? val -    65536 : val); }
        case 3:{int val = read_int(3); return (val > 8388607 ? val - 16777216 : val); }
        case 4:{return read_int(4); }
        }
        PERROR((byte_num == 0 || byte_num > 4), printf("byte_num should be 1,2,3,4."););
        return -1;
    }
};

#undef CHECK_AND_WRITE_BLOCK
#undef OUTPUT_MANAGER_BUFFER_SIZE
#endif
