

#ifndef ___ConfigInterfaceIN3_cpp__
#define ___ConfigInterfaceIN3_cpp__

#include "FormatterController.cpp"

inline void FormatterDate::load_config2(FILE *file){
    //fprintf(file, "Date %d\n", first_date);
    int first_value;
    char type[5];
    fscanf(file, "%4s", type);
    fscanf(file, "%d", &first_value);
    sizeManager.load(file);
    fscanf(file, "\n");
    delta_encoding.set_first_value(first_value);
};
inline void FormatterDebug::load_config2(FILE *file){};
inline void FormatterDiscard::load_config2(FILE *file){};
inline void FormatterIFStatement::load_config2(FILE *file){
    //fprintf(file, "IF\n");
    char buffer[32];
    fgets(buffer, sizeof(buffer), file);
};
inline void FormatterInteger::load_config2(FILE *file){
    char type[7];
    fscanf(file, "%6s", type);
    if (type[0] == 'I'){ //fprintf(file, "Int");

    }else{ //fprintf(file, "BigInt %d", BigIntFlagAt1);
        fscanf(file, " %d", &BigIntFlagAt1);
    }
    //record_min.save(file);
    //record_max.save(file);
    //sizeManager1.save(file);
    //sizeManager2.save(file);
    record_min = FlexibleInt::load(file);
    record_max = FlexibleInt::load(file);
    sizeManager1.load(file);
    sizeManager2.load(file);
    //if (increasingFuncFlag == true){
    //   fprintf(file, " T");
    //   delta_encoding.get_first_value().save();
    //}else fprintf(file, " F");
    char tmp1;
    fscanf(file, " %c", &tmp1);
    PERROR(tmp1 != 'T' && tmp1 != 'F', printf("syntax error"););
    increasingFuncFlag = (tmp1 == 'T' ? true : false);
    if (increasingFuncFlag) delta_encoding.set_first_value(FlexibleInt::load(file));
    fscanf(file, "\n");





    record_range = (record_max - record_min);
    record_range.try_to_cast_to_int();
    SameFlag = (record_range == 0);
    initialized = true;
};
inline void FormatterIPaddr::load_config2(FILE *file){
    //fprintf(file, "IPv4");
    //sizeManager.save(file);
    //hashCompressor.save(file);
    char type[5];
    fscanf(file, "%4s", type);
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
    fscanf(file, "%6s %u", type, &bit_num_int);
    sizeManager.load(file);
    hashCompressor.load(file);
    fscanf(file, "\n");
    bit_num = bit_num_int; //avoid memory overflow
};
inline void FormatterChar::load_config1(FILE *file){
    //fprintf(file, "Char\n");
    char buffer[32];
    fgets(buffer, sizeof(buffer), file);
};







#endif
