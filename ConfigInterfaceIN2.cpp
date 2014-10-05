

#ifndef ___ConfigInterfaceIN2_cpp__
#define ___ConfigInterfaceIN2_cpp__

#include "FormatterController.cpp"

void FormatterDate::load_config1(FILE *file){
    char type[5];
    if (fscanf(file, "%4s", type) != 1) PERROR(true, printf("read Date fail"); );
    PERROR(strcmp("Date", type), printf("Config Error: %s != Date", type););
    fscanf(file, "\n");
};
void FormatterDebug::load_config1(FILE *file){};
void FormatterDiscard::load_config1(FILE *file){};
void FormatterIFStatement::load_config1(FILE *file){
    //fprintf(file, "IF\n");
    char type[3];
    if (fscanf(file, "%2s\n", type) != 1) PERROR(true, printf("read IF fail"); );
    PERROR(strcmp("IF", type), printf("Config Error: %s != IF", type););
};
void FormatterInteger::load_config1(FILE *file){
    char type[4];
    if (fscanf(file, "%3s", type) != 1) PERROR(true, printf("read Int fail"); );
    PERROR(strcmp("Int", type), printf("Config Error: %s != Int", type););
    fscanf(file, "\n");
};
void FormatterIPaddr::load_config1(FILE *file){
    //fprintf(file, "IPv4");
    //hashCompressor.save(file);
    char type[5];
    if (fscanf(file, "%4s", type) != 1) PERROR(true, printf("read IPv4 fail"); );
    PERROR(strcmp("IPv4", type), printf("Config Error: %s != IPv4", type););
    hashCompressor.load(file);
    fscanf(file, "\n");
};
void FormatterString::load_config1(FILE *file){
    //fprintf(file, "String");
    //hashCompressor.save(file);
    char type[7];
    if (fscanf(file, "%6s", type) != 1) PERROR(true, printf("read String fail"); );
    PERROR(strcmp("String", type), printf("Config Error: %s != String", type););
    hashCompressor.load(file);
    fscanf(file, "\n");
    unsigned int hashValueCounter = hashCompressor.getHashValueCounter();
    if (hashValueCounter > 1){
        bit_num = 1;
        unsigned int tmp = hashValueCounter - 1;
        while(tmp >>= 1, tmp != 0) bit_num += 1;
    }else bit_num = 0;
    bit_num = ((bit_num + 7)/ 8) * 8;
    //if (bit_num <= 8) sizeManager.setSize2FlagAt(-1); //no need to use partial 1-byte compressor.
};
void FormatterChar::load_config2(FILE *file){
    //fprintf(file, "Char\n");
    char type[5];
    if (fscanf(file, "%4s", type) != 1) PERROR(true, printf("read Char fail"); );
    PERROR(strcmp("Char", type), printf("Config Error: %s != Char", type););
};





#endif
