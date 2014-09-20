

#ifndef ___ConfigInterfaceOUT2_cpp__
#define ___ConfigInterfaceOUT2_cpp__

void FormatterDate::save_config2(FILE *file){
    fprintf(file, "Date %d", delta_encoding.get_first_value());
    sizeManager.save(file);
    streamingRecorder.save(file);
    fprintf(file, "\n");
};
void FormatterDebug::save_config2(FILE *file){};
void FormatterDiscard::save_config2(FILE *file){};
void FormatterIFStatement::save_config2(FILE *file){ //TODO
    fprintf(file, "IF\n");
};
void FormatterInteger::save_config2(FILE *file){
    if (BigIntFlagAt1 == -1){ //Int
        fprintf(file, "Int");
    }else{  //BigInt
        fprintf(file, "BigInt %d", BigIntFlagAt1);
    }
    sizeManager1.save(file);
    sizeManager2.save(file);
    streamingRecorder.save(file);
    if (streamingRecorder.isInitialized() && streamingRecorder.isAlwaysIncreasing()) delta_encoding.get_first_value().save(file);
    fprintf(file, "\n");
};
void FormatterIPaddr::save_config2(FILE *file){
    fprintf(file, "IPv4");
    sizeManager.save(file);
    hashCompressor.save(file);
    fprintf(file, "\n");
};
void FormatterString::save_config2(FILE *file){
    fprintf(file, "String %u", bit_num);
    sizeManager.save(file);
    hashCompressor.save(file);
    fprintf(file, "\n");
};
void FormatterChar::save_config2(FILE *file){
    fprintf(file, "Char\n");
};

#endif
