
#ifndef ___FileIOManager_cpp__
#define ___FileIOManager_cpp__
#define OUTPUT_MANAGER_BUFFER_SIZE 4096
#define INPUT_MANAGER_BUFFER_SIZE 4096
#define COPYFILES_MANAGER_BUFFER_SIZE 4096
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
#include<memory.h>
#include "lib/FlexibleInt.cpp"
#include "lib/FlexibleFile.cpp"
#include <ruby.h> // it defines F_OK
#ifndef PERROR
  #include "windows.cpp"
#endif
FILE *fopen2(const char *filename, const char *mode){
    FILE *f = fopen(filename, mode);
    PERROR(f == NULL, printf("Cannot open %s",filename); );
    return f;
}

class OutputManager{
private:
    FlexibleFile file;
    unsigned char buffer[OUTPUT_MANAGER_BUFFER_SIZE], bit_counter;
    unsigned int buffer_counter;
public:
    OutputManager(const char *filename, unsigned int mode = FILE_MODE_RAW){
        switch(mode){
        case FILE_MODE_RAW: { file = fopen2(filename, "wb");                    break; }
        case FILE_MODE_BZ2: { file = BzipManager::openBz2File(filename, "w9");  break; }
        default:{ PERROR(true, printf("Unknown mode: %d", mode);)}
        }
        buffer_counter = 0;
        bit_counter = 0;
        memset(buffer, 0, sizeof(buffer));
    }
    ~OutputManager(){
        flush();
        file.close();
    }
//-------------------------------------------
//  Core
//-------------------------------------------
    inline void plus_buffer_counter(){
        buffer_counter += 1;
        if (buffer_counter == OUTPUT_MANAGER_BUFFER_SIZE) flush();
    }
    inline void flush(){
        file.write(buffer, sizeof(char), buffer_counter);
        buffer_counter = 0;
        memset(buffer, 0, sizeof(buffer));
    }
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
    inline void write_bits(unsigned int value, unsigned char bit_num){
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



//==========================================================================================




class InputManager{
private:
    FlexibleFile file;
    unsigned char buffer[INPUT_MANAGER_BUFFER_SIZE], bit_counter;
    unsigned int buffer_counter, buffer_counter_max;
    bool eof_flag;
public:
    InputManager(const char *filename, unsigned int mode = FILE_MODE_RAW) : eof_flag(false){
        switch(mode){
        case FILE_MODE_RAW: { file = fopen2(filename, "rb");                    break; }
        case FILE_MODE_BZ2: { file = BzipManager::openBz2File(filename, "rb");  break; }
        default:{ PERROR(true, printf("Unknown mode: %d", mode);)}
        }
        buffer_counter = 0;
        bit_counter = 0;
        load_data();
    }
    ~InputManager(){
        file.close();
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
    inline void plus_buffer_counter(){
        buffer_counter += 1;
        if (buffer_counter == buffer_counter_max) load_data();
    }
    inline void load_data(){
        buffer_counter_max = file.read(buffer, sizeof(char), INPUT_MANAGER_BUFFER_SIZE);
        buffer_counter = 0;
        if (buffer_counter_max == 0) eof_flag = true;
    }
//-------------------------------------------
//  ACCESS
//-------------------------------------------
    inline bool is_eof(){ return eof_flag; }
    inline unsigned char read_char(){
        if (eof_flag) return 0;
        if (bit_counter == 0){
            unsigned char data = buffer[buffer_counter];
            plus_buffer_counter();
            return data;
        }else{
            unsigned char data = (buffer[buffer_counter] >> bit_counter);
            plus_buffer_counter();
            data |= (buffer[buffer_counter] << (8 - bit_counter));
            return data;
        }
    }
    inline int read_int(){
        if (eof_flag) return 0;
        unsigned char fourBytes[4];
        fourBytes[0] = read_char();
        fourBytes[1] = read_char();
        fourBytes[2] = read_char();
        fourBytes[3] = read_char();
        return bytes_to_int_Little(fourBytes);
    }
    inline int read_int(unsigned char byte_num){
        if (eof_flag) return 0;
        unsigned char fourBytes[4];
        fourBytes[0] = (byte_num > 0 ? read_char() : 0);
        fourBytes[1] = (byte_num > 1 ? read_char() : 0);
        fourBytes[2] = (byte_num > 2 ? read_char() : 0);
        fourBytes[3] = (byte_num > 3 ? read_char() : 0);
        return bytes_to_int_Little(fourBytes);
    }
    inline long read_long(){
        return (long) read_int(); //TODO: What to do when "long" is not 4-bytes.
    }
    inline char *readLine(char *output, int maxLength){
        if (is_eof()) return NULL;
        int idx = 0;
        maxLength -= 1;
        while(idx < maxLength){
            char c = (char) read_char();
            output[idx++] = c;
            if (c == '\n' || is_eof()) break;
        }
        output[idx] = '\0';
        return output;
    }
//-------------------------------------------
//  Extend
//-------------------------------------------
    inline BigInteger *read_bigInt(){
        if (eof_flag) return new BigInteger(0);
        int sign = read_char() - 1;
        unsigned int array_len = (unsigned int) read_int();
        unsigned long *array = (unsigned long *) malloc(array_len * sizeof(unsigned long));
        for(unsigned int i = 0; i < array_len; ++i) array[i] = read_long();
        BigInteger *bigInt = new BigInteger(array, array_len, BigInteger::Sign(sign));
        free(array);
        return bigInt;
    }
    inline int read_n_byte_int(unsigned char byte_num){
        if (eof_flag) return 0;
        switch(byte_num){
        case 1:{int val = read_int(1); return (val >     127 ? val -      256 : val); }
        case 2:{int val = read_int(2); return (val >   32767 ? val -    65536 : val); }
        case 3:{int val = read_int(3); return (val > 8388607 ? val - 16777216 : val); }
        case 4:{return read_int(4); }
        }
        PERROR((byte_num == 0 || byte_num > 4), printf("byte_num should be 1,2,3,4."););
        return -1;
    }
    inline unsigned int read_bits(unsigned char bit_num){
        if (eof_flag) return 0;
        unsigned char fourBytes[4] = {0};
        unsigned int current_idx = 0;
        while(bit_num >= 8){
            fourBytes[current_idx] = read_char();
            current_idx += 1;
            bit_num -= 8;
        }
        if (bit_num == 0) return bytes_to_int_Little(fourBytes);
        unsigned char data = (buffer[buffer_counter] >> bit_counter);
        bit_counter += bit_num;
        if (bit_counter >= 8){
            bit_counter -= 8;
            plus_buffer_counter();
        }else data = 0;
        unsigned char tmp = (buffer[buffer_counter] << (8 - bit_counter));
        data |= (tmp >> (8 - bit_num));
        fourBytes[current_idx] = data;
        return bytes_to_int_Little(fourBytes);
    }
};

class CopyFilesManager{
private:
    FILE *file;
    unsigned char buffer[COPYFILES_MANAGER_BUFFER_SIZE];
public:
    CopyFilesManager(const char *filename){
        file = fopen2(filename, "wb");
    }
    ~CopyFilesManager(){
        fclose(file);
    }
    inline void copy(const char *filename, bool remove_flag = false){
        FILE *input_file = fopen2(filename, "rb");
        int number;
        while((number = fread(buffer, sizeof(char), COPYFILES_MANAGER_BUFFER_SIZE, input_file)) > 0){
            fwrite(buffer, sizeof(char), number, file);
        }
        fclose(input_file);
        if (remove_flag) remove(filename);
    }

};


#undef SIZE_OF_UINT
#undef CHECK_AND_WRITE_BLOCK
#undef OUTPUT_MANAGER_BUFFER_SIZE
#undef INPUT_MANAGER_BUFFER_SIZE
#endif

