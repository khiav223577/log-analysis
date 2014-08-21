
#ifndef ___FormatterIPaddr_cpp__
#define ___FormatterIPaddr_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterIPaddr : public FormatterController{
public:
    typedef FormatterController super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
    public:
    //-------------------------------------------------------------------------
    //  transform config-format to appropriate format. (for speed up)
    //-------------------------------------------------------------------------
        char *trans_format(const char *_format){
            char *format = (char *) malloc(18);
            sprintf(format, "%%3d.%%3d.%%3d.%%3d%%n");
            return format; //Ex: format = "%3d.%3d.%3d.%3d%n"
        }
	};
    FormatterIPaddr(const char *_format) : super(_format, new VirtualCreator()){

    }
public:

//--------------------------------------
//  execute
//--------------------------------------
    void execute(const char ** inputStream){
        unsigned int resule = retrieve(inputStream, format);
        printf("%d.%d.%d.%d\n",(resule >> 24) & 255,(resule >> 16) & 255,(resule >> 8) & 255,(resule >> 0) & 255);
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    unsigned int retrieve(const char **input, const char *format){ //EX: input = 140.109.23.120
        int scanfLen = 0;
        int a,b,c,d;
        sscanf(*input, format, &a, &b, &c, &d, &scanfLen);
        *input += scanfLen;
        return (a << 24) | (b << 16) | (c << 8) | (d << 0);
    }
};

#endif
