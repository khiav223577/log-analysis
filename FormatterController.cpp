#ifndef ___FormatterController_cpp__
#define ___FormatterController_cpp__
#define FormatList std::vector<FormatterController*>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include "FileIOManager.cpp"

class FormatterController{
public:
    class VirtualCreator{ //�קK�bconstrutor�ɵL�k���T�ϥ�virtual�禡�����D
	public:
		virtual char *trans_format(const char *_format)=0;
	};
	char *format;
	bool initialized;
    FormatterController(const char *_format, VirtualCreator *v){
        format = v->trans_format(_format);
        initialized = false;
        delete v;
    }
    virtual ~FormatterController(){
        free(format);
    }
    virtual char *get_prev_result(){    //Will be called by FormatterIFStatement.
        printf("Error: fails to get_prev_result()");
        exit(1);
    }
    virtual int get_prev_int(){         //Will be called by FormatterIFStatement.
        printf("Error: fails to get_prev_int()");
        exit(1);
    }
    virtual int execute(OutputManager *outputer, const char **inputStream)=0; //�^�ǭnskip�������O��
};


#include "FormatterDate.cpp"
#include "FormatterString.cpp"
#include "FormatterInteger.cpp"
#include "FormatterDiscard.cpp"
#include "FormatterDebug.cpp"
#include "FormatterIPaddr.cpp"
#include "FormatterIFStatement.cpp"



#endif


