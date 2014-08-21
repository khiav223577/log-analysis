

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
    //fprintf(file, "IPv4\n");
    char buffer[32];
    fgets(buffer, sizeof(buffer), file);
};
inline void FormatterString::load_config1(FILE *file){
    //fprintf(file, "String %d\n", hashTable.size());
    //for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it) fprintf(file, "%d %s\n", it->second, it->first);
    char type[7];
    fscanf(file, "%6s", type);
    fscanf(file, "%d", &hashValueCounter);
    sizeManager.load(file);
    fscanf(file, "\n");
    hashKeys = std::vector<char *>(hashValueCounter);
    for(int idx, i = 0; i < hashValueCounter; ++i){
        char *input = (char *) malloc((MaxLen + 1) * sizeof(char));
        *input = '\0'; //prevent error when input is empty string
        fscanf(file, "%d%*1[ ]%[^\n]\n", &idx, input);
        hashKeys[idx] = input;
        RMap<MapChar(int)>::InsertKeyToMap(hashTable, input, idx);
    }
    if (hashValueCounter > 1){
        bit_num = 1;
        int tmp = hashValueCounter - 1;
        while(tmp >>= 1, tmp != 0) bit_num += 1;
    }else bit_num = 0;
    if (bit_num <= 8) sizeManager.setSize2FlagAt(-1); //no need to use partial 1-byte compressor.
};








#endif
