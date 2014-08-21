

#ifndef ___ConfigInterfaceIN2_cpp__
#define ___ConfigInterfaceIN2_cpp__

#include "FormatterController.cpp"

inline void FormatterDate::load_config1(FILE *file){
    //fprintf(file, "Date\n");
};
inline void FormatterDebug::load_config1(FILE *file){};
inline void FormatterDiscard::load_config1(FILE *file){};
inline void FormatterIFStatement::load_config1(FILE *file){ //TODO
    //fprintf(file, "IF\n");
};
inline void FormatterInteger::load_config1(FILE *file){
    /*
    if (BigIntFlagAt == -1){ //Int
        fprintf(file, "Int %d %d\n", record_min.getValue(), record_max.getValue());
    }else{  //BigInt
        fprintf(file, "BigInt %d %s %s\n", BigIntFlagAt, record_min.getValuePtrAsStr().c_str(), record_max.getValuePtrAsStr().c_str());
    }*/

};
inline void FormatterIPaddr::load_config1(FILE *file){
    //fprintf(file, "IPv4\n");
};
inline void FormatterString::load_config1(FILE *file){
    /*
    fprintf(file, "String %d\n", hashTable.size());
    for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
        fprintf(file, "%d %s\n", it->second, it->first);
    }*/
};

#endif
