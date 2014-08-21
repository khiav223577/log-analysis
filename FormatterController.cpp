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
	char *format, *prev_result;
    virtual int execute(const char ** inputStream)=0;
    FormatterController(const char *_format, VirtualCreator *v){
        prev_result = NULL;
        format = v->trans_format(_format);
        delete v;
    }
    virtual ~FormatterController(){
        free(prev_result);
        free(format);
    }
    virtual char *get_prev_result(){ //Will be call by FormatterIFStatement.
        return prev_result;
    }
};


#include "FormatterDate.cpp"
#include "FormatterString.cpp"
#include "FormatterInteger.cpp"
#include "FormatterDiscard.cpp"
#include "FormatterIPaddr.cpp"
#include "FormatterIFStatement.cpp"



#endif


