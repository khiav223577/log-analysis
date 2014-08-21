
#ifndef ___FormatterDiscard_cpp__
#define ___FormatterDiscard_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterDiscard : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            char *format = (char *) malloc(strlen(_format) + 1);
            strcpy(format, _format);
            return format; //Ex: format = ",^"
            /*
            char *format = (char *) malloc(strlen(_format) + 1 + 2);
            sprintf(format, "%s%%n", _format);
            return format; //Ex: format = ",%n"*/
        }
	};
	int format_len;
    FormatterDiscard(const char *_format) : super(_format, new VirtualCreator()){
        format_len = strlen(format);
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
            evalu_discard.start();
        #endif
        retrieve(inputStream, format);
        #ifdef EVALUATE_TIME
            evalu_discard.stop();
        #endif
        return 0;
    }
    int execute2(){
        return 0;
    }
    int execute3(){
        return 0;
    }
//-------------------------------------------------------------------------
//  retrieve data from input according the format.
//-------------------------------------------------------------------------
    inline void retrieve(const char **input, const char *format){
        const char *inputStr = *input;
        for(int i = 0; i < format_len; ++i){
            if (*inputStr != format[i]) break;
            inputStr += 1;
        }
        *input = inputStr;
        /*
        int scanfLen = 0;
        sscanf(*input, format, &scanfLen);
        *input += scanfLen;
        return;*/
    }
};

#endif
