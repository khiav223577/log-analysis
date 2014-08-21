

#ifndef ___ConfigInterfaceIN3_cpp__
#define ___ConfigInterfaceIN3_cpp__

#include "FormatterController.cpp"

inline void FormatterDate::load_config2(FILE *file){
    //fprintf(file, "Date %d\n", first_date);
    int first_value;
    char type[5];
    fscanf(file, "%4s", type);
    fscanf(file, "%d %d\n", &first_value, &Size4FlagAt);
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
    if (type[0] == 'I'){ //fprintf(file, "Int %d", Size4FlagAt1);
        fscanf(file, " %d", &Size4FlagAt1);
    }else{ //fprintf(file, "BigInt %d", BigIntFlagAt1);
        fscanf(file, " %d", &BigIntFlagAt1);
    }
    //record_min.save(file);
    //record_max.save(file);
    //fprintf(file, " %d", Size4FlagAt2);
    record_min = FlexibleInt::load(file);
    record_max = FlexibleInt::load(file);
    fscanf(file, " %d", &Size4FlagAt2);
    //if (increasingFuncFlag == true){
    //   fprintf(file, " T\n");
    //}else fprintf(file, " F\n");
    char tmp1;
    fscanf(file, " %c\n", &tmp1);
    PERROR(tmp1 != 'T' && tmp1 != 'F', printf("syntax error"););
    increasingFuncFlag = (tmp1 == 'T' ? true : false);

    record_range = (record_max - record_min);
    record_range.try_to_cast_to_int();
    SameFlag = (record_range == 0);
    initialized = true;
};
inline void FormatterIPaddr::load_config2(FILE *file){
    //fprintf(file, "IPv4\n");
    char buffer[32];
    fgets(buffer, sizeof(buffer), file);
};
inline void FormatterString::load_config2(FILE *file){
    //fprintf(file, "String %d\n", hashTable.size());
    //for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it) fprintf(file, "%d %s\n", it->second, it->first);
    unsigned int bit_num_int;
    char type[7];
    fscanf(file, "%6s", type);
    fscanf(file, "%d %d %u\n", &hashValueCounter, &Size4FlagAt, &bit_num_int);
    bit_num = bit_num_int;
    hashKeys = std::vector<char *>(hashValueCounter);
    for(int idx, i = 0; i < hashValueCounter; ++i){
        char *input = (char *) malloc((MaxLen + 1) * sizeof(char));
        *input = '\0'; //prevent error when input is empty string
        fscanf(file, "%d%*1[ ]%[^\n]\n", &idx, input);
        hashKeys[idx] = input;
        RMap<MapChar(int)>::InsertKeyToMap(hashTable, input, idx);
    }
};








#endif
