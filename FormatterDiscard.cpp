
#ifndef ___FormatterDiscard_cpp__
#define ___FormatterDiscard_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterDiscard : public FormatterController{
public:
    typedef FormatterController super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
    public:
    //-------------------------------------------------------------------------
    //  transform config-format to appropriate format. (for speed up)
    //-------------------------------------------------------------------------
        char *trans_format(const char *_format){
            char *format = (char *) malloc(strlen(_format) + 1 + 2);
            sprintf(format, "%s%%n", _format);
            return format; //Ex: format = ",%n"
        }
	};
    FormatterDiscard(const char *_format) : super(_format, new VirtualCreator()){

    }
public:

//--------------------------------------
//  execute
//--------------------------------------
    void execute(const char ** inputStream){
        retrieve(inputStream, format);
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    void retrieve(const char **input, const char *format){
        int scanfLen = 0;
        sscanf(*input, format, &scanfLen);
        *input += scanfLen;
        return;
    }
};

#endif
