


#ifndef ___ConfigInterfaceOUT1_cpp__
#define ___ConfigInterfaceOUT1_cpp__
    inline void FormatterDate::output_config1(FILE *file){
        fprintf(file, "Date\n");
    };
    inline void FormatterDebug::output_config1(FILE *file){};
    inline void FormatterDiscard::output_config1(FILE *file){};
    inline void FormatterIFStatement::output_config1(FILE *file){ //TODO
        fprintf(file, "IF\n");
    };
    inline void FormatterInteger::output_config1(FILE *file){
        fprintf(file, "Int %d %d\n", record_min, record_max);
    };
    inline void FormatterIPaddr::output_config1(FILE *file){
        fprintf(file, "IPv4\n");
    };
    inline void FormatterString::output_config1(FILE *file){
        fprintf(file, "String %d\n", hashTable.size());
        for(MapChar(int)::iterator it = hashTable.begin(); it != hashTable.end(); ++it){
            fprintf(file, "%d %s\n", it->second, it->first);
        }
    };

#endif
