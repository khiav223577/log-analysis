

#ifndef ___ConfigInterfaceIN2_cpp__
#define ___ConfigInterfaceIN2_cpp__

#include "FormatterController.cpp"

inline void FormatterDate::load_config1(FILE *file){
    //fprintf(file, "Date %d\n", first_date);
    int first_value;
    char type[5];
    fscanf(file, "%4s", type);
    fscanf(file, "%d", &first_value);
    sizeManager.load(file);
    fscanf(file, "\n");
    delta_encoding.set_first_value(first_value);
};
inline void FormatterDebug::load_config1(FILE *file){};
inline void FormatterDiscard::load_config1(FILE *file){};
inline void FormatterIFStatement::load_config1(FILE *file){
    //fprintf(file, "IF\n");
    char buffer[32];
    fgets(buffer, sizeof(buffer), file);
};
inline void FormatterInteger::load_config1(FILE *file){
    char type[7];
    fscanf(file, "%6s", type);
    if (type[0] == 'I'){ //fprintf(file, "Int %d", Size4FlagAt1);
        sizeManager1.load(file);
    }else{ //fprintf(file, "BigInt %d", BigIntFlagAt1);
        fscanf(file, " %d", &BigIntFlagAt1);
    }
    //record_min.save(file);
    //record_max.save(file);
    record_min = FlexibleInt::load(file);
    record_max = FlexibleInt::load(file);
    //fprintf(file, " %c\n", (increasingFuncFlag ? 'T' : 'F'));
    char tmp1;
    fscanf(file, " %c\n", &tmp1);
    PERROR(tmp1 != 'T' && tmp1 != 'F', printf("syntax error"););
    increasingFuncFlag = (tmp1 == 'T' ? true : false);

    record_range = (record_max - record_min);
    record_range.try_to_cast_to_int();
    SameFlag = (record_range == 0);
    initialized = true;
};
inline void FormatterIPaddr::load_config1(FILE *file){
    //fprintf(file, "IPv4");
    //sizeManager.save(file);
    //hashCompressor.save(file);
    char type[5];
    fscanf(file, "%4s", type);
    sizeManager.load(file);
    hashCompressor.load(file);
    fscanf(file, "\n");
};
inline void FormatterString::load_config1(FILE *file){
    //fprintf(file, "String");
    //sizeManager.save(file);
    //hashCompressor.save(file);
    char type[7];
    fscanf(file, "%6s", type);
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
    char buffer[32];
    fgets(buffer, sizeof(buffer), file);
};





#endif
