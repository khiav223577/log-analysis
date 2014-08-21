
#ifndef ___FormatterDebug_cpp__
#define ___FormatterDebug_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterDebug : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            char *format = (char *) malloc(strlen(_format) + 1);
            strcpy(format, _format);
            return format;
        }
	};
    FormatterDebug(const char *_format) : super(_format, new VirtualCreator()){

    }
public:

//--------------------------------------
//  execute
//--------------------------------------
    inline int execute(const char **inputStream){
        printf("%s", format);
        return 0;
    }
};

#endif
