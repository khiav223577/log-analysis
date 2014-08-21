#ifndef ___FormatterController_cpp__
#define ___FormatterController_cpp__
#define FormatList std::vector<FormatterController*>
#include<stdlib.h>
#include <vector>
class FormatterController{
public:
    class VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
	public:
		virtual char *trans_format(const char *_format)=0;
	};
	char *format;
    virtual int execute(const char ** inputStream)=0;
    FormatterController(const char *_format, VirtualCreator *v){
        format = v->trans_format(_format);
        delete v;
    }
    virtual ~FormatterController(){
        free(format);
    }
    virtual char *get_prev_result(){ //Will be called by FormatterIFStatement.
        printf("Error: fails to get_prev_result()");
        exit(1);
    }
};


#include "FormatterDate.cpp"
#include "FormatterString.cpp"
#include "FormatterInteger.cpp"
#include "FormatterDiscard.cpp"
#include "FormatterIPaddr.cpp"
#include "FormatterIFStatement.cpp"



#endif


