
#ifndef ___FormatterIPaddr_cpp__
#define ___FormatterIPaddr_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterIPaddr : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            char *format = (char *) malloc(18);
            sprintf(format, "%%3d.%%3d.%%3d.%%3d%%n");
            return format; //Ex: format = "%3d.%3d.%3d.%3d%n"
        }
	};
    FormatterIPaddr(const char *_format) : super(_format, new VirtualCreator()){

    }
public:
    virtual void save_config1(FILE *file);
    virtual void save_config2(FILE *file);
    virtual void load_config1(FILE *file);
    virtual void load_config2(FILE *file);
//--------------------------------------
//  execute
//--------------------------------------
    int execute1(const char **inputStream){
        #ifdef EVALUATE_TIME
            evalu_ip.start();
        #endif
        unsigned int result = retrieve(inputStream, format);
        outputer->write((int) result);
        debug(result);
        #ifdef EVALUATE_TIME
            evalu_ip.stop();
        #endif
        return 0;
    }
    int execute2(){
        unsigned int result = (unsigned int) inputer->read_int();
        outputer->write((int) result);
        debug(result);
        return 0;
    }
    int execute3(){
        unsigned int result = (unsigned int) inputer->read_int();
        debug(result);
        return 0;
    }
    inline void debug(int result){
        #ifdef DEBUG
            printf("%d.%d.%d.%d ",(result >> 24) & 255,(result >> 16) & 255,(result >> 8) & 255,(result >> 0) & 255);
        #endif
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline unsigned int retrieve(const char **input, const char *format){ //EX: input = 140.109.23.120
        int scanfLen = 0;
        int a = -1,b = -1,c = -1,d = -1;
        sscanf(*input, format, &a, &b, &c, &d, &scanfLen);
        PERROR(a == -1 || b == -1 || c == -1 || d == -1, printf("retrieve IP failed. input = %s", *input););
        *input += scanfLen;
        return (a << 24) | (b << 16) | (c << 8) | (d << 0);
    }
};

#endif
