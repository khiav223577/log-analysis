

#ifndef ___ConfigInterfaceIN2_cpp__
#define ___ConfigInterfaceIN2_cpp__

#include "FormatterController.cpp"

inline void FormatterDate::load_config1(FILE *file){
    //fprintf(file, "Date %d\n", first_date);
    int first_value;
    char type[5];
    if (fscanf(file, "%4s", type) != 1) PERROR(true, printf("read Date fail"); );
    PERROR(strcmp("Date", type), printf("Config Error: %s != Date", type););
    fscanf(file, " %d", &first_value);
    delta_encoding.set_first_value(first_value);
    sizeManager.load(file);
    streamingRecorder.load(file);
    fscanf(file, "\n");
    SameFlag = (streamingRecorder.getMinMaxRange() == 0);
};
inline void FormatterDebug::load_config1(FILE *file){};
inline void FormatterDiscard::load_config1(FILE *file){};
inline void FormatterIFStatement::load_config1(FILE *file){
    //fprintf(file, "IF\n");
    char type[3];
    if (fscanf(file, "%2s\n", type) != 1) PERROR(true, printf("read IF fail"); );
    PERROR(strcmp("IF", type), printf("Config Error: %s != IF", type););
};
inline void FormatterInteger::load_config1(FILE *file){
    char type[7];
    if (fscanf(file, "%6s", type) != 1) PERROR(true, printf("read Int fail"); );
    PERROR(strcmp("Int", type) && strcmp("BigInt", type), printf("Config Error: %s != Int, BigInt", type););
    if (type[0] == 'I'){ //fprintf(file, "Int");

    }else{ //fprintf(file, "BigInt %d", BigIntFlagAt1);
        fscanf(file, " %d", &BigIntFlagAt1);
    }
    //sizeManager1.save(file);
    //streamingRecorder.save(file);
    sizeManager1.load(file);
    streamingRecorder.load(file);
    fscanf(file, "\n");
    record_range = streamingRecorder.getMinMaxRange();
    record_range.try_to_cast_to_int();
    compress_mode = caculate_compress_mode();
};
inline void FormatterIPaddr::load_config1(FILE *file){
    //fprintf(file, "IPv4");
    //sizeManager.save(file);
    //hashCompressor.save(file);
    char type[5];
    if (fscanf(file, "%4s", type) != 1) PERROR(true, printf("read IPv4 fail"); );
    PERROR(strcmp("IPv4", type), printf("Config Error: %s != IPv4", type););
    sizeManager.load(file);
    hashCompressor.load(file);
    fscanf(file, "\n");
};
inline void FormatterString::load_config1(FILE *file){
    //fprintf(file, "String");
    //sizeManager.save(file);
    //hashCompressor.save(file);
    char type[7];
    if (fscanf(file, "%6s", type) != 1) PERROR(true, printf("read String fail"); );
    PERROR(strcmp("String", type), printf("Config Error: %s != String", type););
    sizeManager.load(file);
    hashCompressor.load(file);
    fscanf(file, "\n");
    unsigned int hashValueCounter = hashCompressor.get_hashValueCounter();
    if (hashValueCounter > 1){
        bit_num = 1;
        unsigned int tmp = hashValueCounter - 1;
        while(tmp >>= 1, tmp != 0) bit_num += 1;
    }else bit_num = 0;
    bit_num = ((bit_num + 7)/ 8) * 8;
    if (bit_num <= 8) sizeManager.setSize2FlagAt(-1); //no need to use partial 1-byte compressor.
};
inline void FormatterChar::load_config2(FILE *file){
    //fprintf(file, "Char\n");
    char type[5];
    if (fscanf(file, "%4s", type) != 1) PERROR(true, printf("read Char fail"); );
    PERROR(strcmp("Char", type), printf("Config Error: %s != Char", type););
};





#endif
