

#ifndef ___ConfigInterfaceOUT1_cpp__
#define ___ConfigInterfaceOUT1_cpp__

inline void FormatterDate::save_config1(FILE *file){
    fprintf(file, "Date %d", delta_encoding.get_first_value());
    sizeManager.save(file);
    fprintf(file, "\n");
};
inline void FormatterDebug::save_config1(FILE *file){};
inline void FormatterDiscard::save_config1(FILE *file){};
inline void FormatterIFStatement::save_config1(FILE *file){ //TODO
    fprintf(file, "IF\n");
};
inline void FormatterInteger::save_config1(FILE *file){
    if (BigIntFlagAt1 == -1){ //Int
        fprintf(file, "Int");
        sizeManager1.save(file);
    }else{  //BigInt
        fprintf(file, "BigInt %d", BigIntFlagAt1);
    }
    record_min.save(file);
    record_max.save(file);
    fprintf(file, " %c\n", (increasingFuncFlag ? 'T' : 'F'));
};
inline void FormatterIPaddr::save_config1(FILE *file){
    fprintf(file, "IPv4 %u", hashTable.size());
    sizeManager.save(file);
    fprintf(file, "\n");
    for(std::map<unsigned int, unsigned int>::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
        fprintf(file, "%x %x\n", it->second, it->first);
    }
};
inline void FormatterString::save_config1(FILE *file){
    fprintf(file, "String %u", hashTable.size());
    sizeManager.save(file);
    fprintf(file, "\n");
    for(MapChar(unsigned int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
        fprintf(file, "%x %s\n", it->second, it->first);
    }
};

#endif
