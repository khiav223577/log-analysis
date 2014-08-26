

#ifndef ___ConfigInterfaceIN3_cpp__
#define ___ConfigInterfaceIN3_cpp__

#include "FormatterController.cpp"

inline void FormatterDate::load_config2(FILE *file){
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
inline void FormatterDebug::load_config2(FILE *file){};
inline void FormatterDiscard::load_config2(FILE *file){};
inline void FormatterIFStatement::load_config2(FILE *file){
    //fprintf(file, "IF\n");
    char type[3];
    if (fscanf(file, "%2s\n", type) != 1) PERROR(true, printf("read IF fail"); );
    PERROR(strcmp("IF", type), printf("Config Error: %s != IF", type););
};
inline void FormatterInteger::load_config2(FILE *file){
    char type[7];
    if (fscanf(file, "%6s", type) != 1) PERROR(true, printf("read Int fail"); );
    PERROR(strcmp("Int", type) && strcmp("BigInt", type), printf("Config Error: %s != Int, BigInt", type););
    if (type[0] == 'I'){ //fprintf(file, "Int");

    }else{ //fprintf(file, "BigInt %d", BigIntFlagAt1);
        fscanf(file, " %d", &BigIntFlagAt1);
    }
    //sizeManager1.save(file);
    //sizeManager2.save(file);
    //streamingRecorder.save(file);
    //if (streamingRecorder.isAlwaysIncreasing()) delta_encoding.get_first_value().save(file);
    sizeManager1.load(file);
    sizeManager2.load(file);
    streamingRecorder.load(file);
    if (streamingRecorder.isAlwaysIncreasing()) delta_encoding.set_first_value(FlexibleInt::load(file));
    fscanf(file, "\n");
    record_range = streamingRecorder.getMinMaxRange();
    record_range.try_to_cast_to_int();
    compress_mode = caculate_compress_mode();
};
inline void FormatterIPaddr::load_config2(FILE *file){
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
inline void FormatterString::load_config2(FILE *file){
    //fprintf(file, "String %u", bit_num);
    //sizeManager.save(file);
    //hashCompressor.save(file);
    unsigned int bit_num_int;
    char type[7];
    if (fscanf(file, "%6s", type) != 1) PERROR(true, printf("read String fail"); );
    fscanf(file, " %u", &bit_num_int);
    PERROR(strcmp("String", type), printf("Config Error: %s != String", type););
    sizeManager.load(file);
    hashCompressor.load(file);
    fscanf(file, "\n");
    bit_num = bit_num_int; //avoid memory overflow
};
inline void FormatterChar::load_config1(FILE *file){
    //fprintf(file, "Char\n");
    char type[5];
    if (fscanf(file, "%4s", type) != 1) PERROR(true, printf("read Char fail"); );
    PERROR(strcmp("Char", type), printf("Config Error: %s != Char", type););
};





#endif
