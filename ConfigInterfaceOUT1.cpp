

#ifndef ___ConfigInterfaceOUT1_cpp__
#define ___ConfigInterfaceOUT1_cpp__

inline void FormatterDate::save_config1(FILE *file){
    fprintf(file, "Date\n");
};
inline void FormatterDebug::save_config1(FILE *file){};
inline void FormatterDiscard::save_config1(FILE *file){};
inline void FormatterIFStatement::save_config1(FILE *file){ //TODO
    fprintf(file, "IF\n");
};
inline void FormatterInteger::save_config1(FILE *file){
    if (BigIntFlagAt == -1){ //Int
        fprintf(file, "Int %d %d\n", record_min.getValue(), record_max.getValue());
    }else{  //BigInt
        fprintf(file, "BigInt %d %s %s\n", BigIntFlagAt, record_min.getValuePtrAsStr().c_str(), record_max.getValuePtrAsStr().c_str());
    }

};
inline void FormatterIPaddr::save_config1(FILE *file){
    fprintf(file, "IPv4\n");
};
inline void FormatterString::save_config1(FILE *file){
    fprintf(file, "String %d\n", hashTable.size());
    for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
        fprintf(file, "%d %s\n", it->second, it->first);
    }
};

#endif
