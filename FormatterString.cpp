
#ifndef ___FormatterString_cpp__
#define ___FormatterString_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
template<unsigned int MAXLEN> class FormatterString{
public:
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    static char *trans_format(const char *_format){
        static int StrlenMAXLEN = MAXLEN < 10 ? 1 : MAXLEN < 100 ? 2 : MAXLEN < 1000 ? 3 : MAXLEN < 10000 ? 4 : MAXLEN < 100000 ? 5 :
                                  MAXLEN < 1000000 ? 6 : MAXLEN < 10000000 ? 7 : MAXLEN < 100000000 ? 8 : MAXLEN < 1000000000 ? 9 : 10;
        char *format = (char *) malloc((strlen(_format) + 1 + 6 + StrlenMAXLEN) * sizeof(char));
        sprintf(format, "%%%d[^%s]%%n", MAXLEN, _format);
        return format; //Ex: format = "%25[^,]%n"
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    static char *retrieve(const char **input, const char *format){
        int scanfLen;
        char *buffer = (char *) malloc((MAXLEN + 1) * sizeof(char));
        sscanf(*input, format, buffer, &scanfLen);
        *input += scanfLen;
        buffer[scanfLen] = '\0'; //avoid error when scanfLen == 0
        return buffer;
    }
};

#endif
