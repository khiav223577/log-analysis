

#ifndef ___ConfigInterfaceIN2_cpp__
#define ___ConfigInterfaceIN2_cpp__

#include "FormatterController.cpp"

inline void FormatterDate::load_config1(FILE *file){
    //fprintf(file, "Date\n");
    char buffer[32];
    fgets(buffer, sizeof(buffer), file);
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
    if (type[0] == 'I'){
        //fprintf(file, "Int %d %d\n", record_min.getValue(), record_max.getValue());
        int min, max;
        fscanf(file, "%d %d\n", &min, &max);
        record_min = FlexibleInt(min);
        record_max = FlexibleInt(max);
    }else{
        //fprintf(file, "BigInt %d %s %s\n", BigIntFlagAt, record_min.getValuePtrAsStr().c_str(), record_max.getValuePtrAsStr().c_str());
        fscanf(file, "%d", &BigIntFlagAt);
        std::string bigIntString1 = readBigInt10(file);
        std::string bigIntString2 = readBigInt10(file);
        fscanf(file, "\n");
        record_min = FlexibleInt(new BigInteger(BigUnsignedInABase(bigIntString1, 10)));
        record_max = FlexibleInt(new BigInteger(BigUnsignedInABase(bigIntString2, 10)));
    }
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
    fscanf(file, "%d\n", &hashValueCounter);
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
