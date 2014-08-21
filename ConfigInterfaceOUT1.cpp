

#ifndef ___ConfigInterfaceOUT1_cpp__
#define ___ConfigInterfaceOUT1_cpp__

inline void FormatterDate::save_config1(FILE *file){
    fprintf(file, "Date %d %d\n", delta_encoding.get_first_value(), Size4FlagAt);
};
inline void FormatterDebug::save_config1(FILE *file){};
inline void FormatterDiscard::save_config1(FILE *file){};
inline void FormatterIFStatement::save_config1(FILE *file){ //TODO
    fprintf(file, "IF\n");
};
inline void FormatterInteger::save_config1(FILE *file){
    if (BigIntFlagAt1 == -1){ //Int
        fprintf(file, "Int %d", Size4FlagAt1);
    }else{  //BigInt
        fprintf(file, "BigInt %d", BigIntFlagAt1);
    }
    record_min.save(file);
    record_max.save(file);
    fprintf(file, " %c\n", (increasingFuncFlag ? 'T' : 'F'));
};
inline void FormatterIPaddr::save_config1(FILE *file){
    fprintf(file, "IPv4\n");
};
inline void FormatterString::save_config1(FILE *file){
    fprintf(file, "String %d %d\n", hashTable.size(), Size4FlagAt);
    for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
        fprintf(file, "%d %s\n", it->second, it->first);
    }
};

#endif
