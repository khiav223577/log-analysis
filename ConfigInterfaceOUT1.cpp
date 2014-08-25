

#ifndef ___ConfigInterfaceOUT1_cpp__
#define ___ConfigInterfaceOUT1_cpp__

inline void FormatterDate::save_config1(FILE *file){
    fprintf(file, "Date %d", delta_encoding.get_first_value());
    sizeManager.save(file);
    streamingRecorder.save(file);
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
    }else{  //BigInt
        fprintf(file, "BigInt %d", BigIntFlagAt1);
    }
    sizeManager1.save(file);
    streamingRecorder.save(file);
};
inline void FormatterIPaddr::save_config1(FILE *file){
    fprintf(file, "IPv4");
    sizeManager.save(file);
    hashCompressor.save(file);
    fprintf(file, "\n");
};
inline void FormatterString::save_config1(FILE *file){
    fprintf(file, "String");
    sizeManager.save(file);
    hashCompressor.save(file);
    fprintf(file, "\n");
};
inline void FormatterChar::save_config1(FILE *file){
    fprintf(file, "Char\n");
};



#endif
