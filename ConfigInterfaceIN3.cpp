

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
    if (type[0] == 'I'){
        //fprintf(file, "Int %d %d %d %d\n", record_min.getValue(), record_max.getValue(), Size4FlagAt1, Size4FlagAt2);
        int min, max;
        fscanf(file, "%d %d %d %d\n", &min, &max, &Size4FlagAt1, &Size4FlagAt2);
        record_min = FlexibleInt(min);
        record_max = FlexibleInt(max);
    }else{
        //fprintf(file, "BigInt %s %s %d %d\n", record_min.getValuePtrAsStr().c_str(), record_max.getValuePtrAsStr().c_str(), BigIntFlagAt, Size4FlagAt2);
        std::string bigIntString1 = readBigInt10(file);
        std::string bigIntString2 = readBigInt10(file);
        fscanf(file, "%d", &BigIntFlagAt);
        fscanf(file, "%d", &Size4FlagAt2);
        fscanf(file, "\n");
        record_min = FlexibleInt(new BigInteger(BigUnsignedInABase(bigIntString1, 10)));
        record_max = FlexibleInt(new BigInteger(BigUnsignedInABase(bigIntString2, 10)));
    }
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
