

#ifndef ___ConfigInterfaceOUT2_cpp__
#define ___ConfigInterfaceOUT2_cpp__

void FormatterDate::save_config2(FILE *file){
    fprintf(file, "Date\n");
};
void FormatterDebug::save_config2(FILE *file){};
void FormatterDiscard::save_config2(FILE *file){};
void FormatterIFStatement::save_config2(FILE *file){ //TODO
    fprintf(file, "IF\n");
};
void FormatterInteger::save_config2(FILE *file){
    fprintf(file, "Int\n");
};
void FormatterIPaddr::save_config2(FILE *file){
    fprintf(file, "IPv4");
    hashCompressor.save(file);
    fprintf(file, "\n");
};
void FormatterString::save_config2(FILE *file){
    fprintf(file, "String %u", bit_num);
    hashCompressor.save(file);
    fprintf(file, "\n");
};
void FormatterChar::save_config2(FILE *file){
    fprintf(file, "Char\n");
};

#endif
