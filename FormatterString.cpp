
#ifndef ___FormatterString_cpp__
#define ___FormatterString_cpp__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INT_TO_STRING(X) #X
template<unsigned int MAXLEN> class FormatterString{
public:
    static char *retrieve(const char ** input, const char *_format){
        static int StrlenMAXLEN = MAXLEN < 10 ? 1 : MAXLEN < 100 ? 2 : MAXLEN < 1000 ? 3 : MAXLEN < 10000 ? 4 : MAXLEN < 100000 ? 5 :
                                  MAXLEN < 1000000 ? 6 : MAXLEN < 10000000 ? 7 : MAXLEN < 100000000 ? 8 : MAXLEN < 1000000000 ? 9 : 10;
        int scanfLen;
        char *format = (char *) malloc((strlen(*input) + 1 + 6 + StrlenMAXLEN) * sizeof(char));
        char *buffer = (char *) malloc((MAXLEN + 1) * sizeof(char));
        sprintf(format, "%%" INT_TO_STRING(MAXLEN) "^[%s]%%n", _format);
        sscanf(*input, format, buffer, scanfLen);
        *input += scanfLen;
        free(format);
        return buffer;
    }
};




#undef INT_TO_STRING

#endif
