
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
    class VirtualCreator : public super::VirtualCreator{ //�קK�bconstrutor�ɵL�k���T�ϥ�virtual�禡�����D
        char *trans_format(const char *_format){
            char *format = (char *) malloc(strlen(_format) + 1);
            strcpy(format, _format);
            return format;
        }
	};
    FormatterDebug(const char *_format) : super(_format, new VirtualCreator()){

    }
public:
    virtual void save_config1(FILE *file);
    virtual void load_config1(FILE *file);
//--------------------------------------
//  execute
//--------------------------------------
    int execute1(OutputManager *outputer, const char **inputStream){
        printf("%s", format);
        return 0;
    }
    int execute2(InputManager *inputer){
        printf("%s", format);
        return 0;
    }
};

#endif
