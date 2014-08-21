
#ifndef ___FormatterString_cpp__
#define ___FormatterString_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterString : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
    public:
        const int MaxLen, StringMaxLen;
        VirtualCreator(const int maxlen) : MaxLen(maxlen), StringMaxLen(maxlen < 10 ? 1 : maxlen < 100 ? 2 : maxlen < 1000 ? 3 : maxlen < 10000 ? 4 : maxlen < 100000 ? 5 :
                                                                        maxlen < 1000000 ? 6 : maxlen < 10000000 ? 7 : maxlen < 100000000 ? 8 : maxlen < 1000000000 ? 9 : 10){
        }
        char *trans_format(const char *_format){
            char *format = (char *) malloc((strlen(_format) + 1 + 6 + StringMaxLen) * sizeof(char));
            sprintf(format, "%%%d[^%s]%%n", MaxLen, _format);
            return format; //Ex: format = "%25[^,]%n"
        }
	};
	char *prev_result;
	const int MaxLen;
    FormatterString(const char *_format, int maxlen) : super(_format, new VirtualCreator(maxlen)), MaxLen(maxlen){
        prev_result = NULL;
    }
    ~FormatterString(){
        free(prev_result);
    }
    char *get_prev_result(){ //Will be called by FormatterIFStatement.
        if (prev_result != NULL) return prev_result;
        printf("Error: fails to get_prev_result() in FormatterString.");
        exit(1);
    }
public:
//--------------------------------------
//  execute
//--------------------------------------
    inline int execute(const char ** inputStream){
        char *str = retrieve(inputStream, format);
        SetColor2(); printf("[%s] ",str); SetColor(7);//DEBUG
        free(prev_result);
        prev_result = str;
        return 0;
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline char *retrieve(const char **input, const char *format){
        int scanfLen = 0;
        char *buffer = (char *) malloc((MaxLen + 1) * sizeof(char));
        sscanf(*input, format, buffer, &scanfLen);
        //sscanf(*input, "%511[^\n]%n", buffer, &scanfLen);
        *input += scanfLen;
        if (**input != '\0') *input += 1; //drop stop-word
        buffer[scanfLen] = '\0'; //avoid error when scanfLen == 0
        return buffer;
    }
};

#endif
