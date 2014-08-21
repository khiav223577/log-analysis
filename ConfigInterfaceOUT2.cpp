

#ifndef ___ConfigInterfaceOUT2_cpp__
#define ___ConfigInterfaceOUT2_cpp__

inline void FormatterDate::save_config2(FILE *file){
    fprintf(file, "Date %d %d\n", delta_encoding.get_first_value(), Size4FlagAt);
};
inline void FormatterDebug::save_config2(FILE *file){};
inline void FormatterDiscard::save_config2(FILE *file){};
inline void FormatterIFStatement::save_config2(FILE *file){ //TODO
    fprintf(file, "IF\n");
};
inline void FormatterInteger::save_config2(FILE *file){
    if (BigIntFlagAt == -1){ //Int
        fprintf(file, "Int %d %d %d %d\n", record_min.getValue(), record_max.getValue(), Size4FlagAt1, Size4FlagAt2);
    }else{  //BigInt
        fprintf(file, "BigInt %s %s %d %d\n", record_min.getValuePtrAsStr().c_str(), record_max.getValuePtrAsStr().c_str(), BigIntFlagAt, Size4FlagAt2);
    }

};
inline void FormatterIPaddr::save_config2(FILE *file){
    fprintf(file, "IPv4\n");
};
inline void FormatterString::save_config2(FILE *file){
    fprintf(file, "String %d %d %u\n", hashTable.size(), Size4FlagAt, bit_num);
    for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
        fprintf(file, "%d %s\n", it->second, it->first);
    }
};

#endif
