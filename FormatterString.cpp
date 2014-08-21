
#ifndef ___FormatterString_cpp__
#define ___FormatterString_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
template<unsigned int MAXLEN> class FormatterString : public FormatterController{
public:
    typedef FormatterController super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
    public:
    //-------------------------------------------------------------------------
    //  transform config-format to appropriate format. (for speed up)
    //-------------------------------------------------------------------------
        char *trans_format(const char *_format){
            static int StrlenMAXLEN = MAXLEN < 10 ? 1 : MAXLEN < 100 ? 2 : MAXLEN < 1000 ? 3 : MAXLEN < 10000 ? 4 : MAXLEN < 100000 ? 5 :
                                      MAXLEN < 1000000 ? 6 : MAXLEN < 10000000 ? 7 : MAXLEN < 100000000 ? 8 : MAXLEN < 1000000000 ? 9 : 10;
            char *format = (char *) malloc((strlen(_format) + 1 + 6 + StrlenMAXLEN) * sizeof(char));
            sprintf(format, "%%%d[^%s]%%n", MAXLEN, _format);
            return format; //Ex: format = "%25[^,]%n"
        }
	};
    FormatterString(const char *_format) : super(_format, new VirtualCreator()){

    }
public:
//--------------------------------------
//  execute
//--------------------------------------
    void execute(const char ** inputStream){
        char *str = retrieve(inputStream, format);
        printf("[%s]\n",str); //DEBUG
        free(prev_result);
        prev_result = str;
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    char *retrieve(const char **input, const char *format){
        int scanfLen = 0;
        char *buffer = (char *) malloc((MAXLEN + 1) * sizeof(char));
        sscanf(*input, format, buffer, &scanfLen);
        *input += scanfLen;
        if (**input != '\0') *input += 1; //drop stop-word
        buffer[scanfLen] = '\0'; //avoid error when scanfLen == 0
        return buffer;
    }
};

#endif
