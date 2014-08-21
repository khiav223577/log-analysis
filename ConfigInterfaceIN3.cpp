

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
    if (type[0] == 'I'){ //fprintf(file, "Int %d", Size4FlagAt1);
        sizeManager1.load(file);
    }else{ //fprintf(file, "BigInt %d", BigIntFlagAt1);
        fscanf(file, " %d", &BigIntFlagAt1);
    }
    //record_min.save(file);
    //record_max.save(file);
    //fprintf(file, " %d", Size4FlagAt2);
    record_min = FlexibleInt::load(file);
    record_max = FlexibleInt::load(file);
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
    //fprintf(file, "IPv4 %u", hashTable.size());
    char type[5];
    fscanf(file, "%4s", type);
    fscanf(file, " %u", &hashValueCounter);
    sizeManager.load(file);
    fscanf(file, "\n");
    hashKeys = std::vector<unsigned int>(hashValueCounter);
    for(unsigned int idx, i = 0; i < hashValueCounter; ++i){
        unsigned int input;
        fscanf(file, "%x %x\n", &idx, &input);
        hashKeys[idx] = input;
        RMap< std::map<unsigned int, unsigned int> >::InsertKeyToMap(hashTable, input, idx);
    }
};
inline void FormatterString::load_config2(FILE *file){
    //fprintf(file, "String %d\n", hashTable.size());
    //for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it) fprintf(file, "%d %s\n", it->second, it->first);
    unsigned int bit_num_int;
    char type[7];
    fscanf(file, "%6s", type);
    fscanf(file, " %u %u", &hashValueCounter, &bit_num_int);
    sizeManager.load(file);
    fscanf(file, "\n");
    bit_num = bit_num_int;
    hashKeys = std::vector<char *>(hashValueCounter);
    for(unsigned int idx, i = 0; i < hashValueCounter; ++i){
        char *input = (char *) malloc((MaxLen + 1) * sizeof(char));
        *input = '\0'; //prevent error when input is empty string
        fscanf(file, "%x%*1[ ]%[^\n]\n", &idx, input);
        hashKeys[idx] = input;
        RMap<MapChar(unsigned int)>::InsertKeyToMap(hashTable, input, idx);
    }
};








#endif
