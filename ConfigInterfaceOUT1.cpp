

#ifndef ___ConfigInterfaceOUT1_cpp__
#define ___ConfigInterfaceOUT1_cpp__

void FormatterDate::save_config1(FILE *file){
    fprintf(file, "Date %d", delta_encoding.get_first_value());
    streamingRecorder.save(file);
    fprintf(file, "\n");
};
void FormatterDebug::save_config1(FILE *file){};
void FormatterDiscard::save_config1(FILE *file){};
void FormatterIFStatement::save_config1(FILE *file){ //TODO
    fprintf(file, "IF\n");
};
void FormatterInteger::save_config1(FILE *file){
    fprintf(file, "Int");
    streamingRecorder.save(file);
    fprintf(file, "\n");
};
void FormatterIPaddr::save_config1(FILE *file){
    fprintf(file, "IPv4");
    hashCompressor.save(file);
    fprintf(file, "\n");
};
void FormatterString::save_config1(FILE *file){
    fprintf(file, "String");
    hashCompressor.save(file);
    fprintf(file, "\n");
};
void FormatterChar::save_config1(FILE *file){
    fprintf(file, "Char\n");
};



#endif
