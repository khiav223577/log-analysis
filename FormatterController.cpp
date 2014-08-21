#ifndef ___FormatterController_cpp__
#define ___FormatterController_cpp__
#define FormatList std::vector<FormatterController*>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include "FileIOManager.cpp"
#include "lib/DeltaEncoding.cpp"

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
    virtual char *get_prev_string(){    //Will be called by FormatterIFStatement.
        PERROR(true, printf("Error: fails to get_prev_string()"););
    }
    virtual int get_prev_int(){         //Will be called by FormatterIFStatement.
        PERROR(true, printf("Error: fails to get_prev_int()"););
    }
    virtual int execute1(OutputManager *outputer, const char **inputStream)=0; //�^�ǭnskip�������O��
    virtual int execute2(InputManager *inputer)=0; //�^�ǭnskip�������O��
    virtual void save_config1(FILE *file)=0;
    virtual void load_config1(FILE *file)=0;
};


#include "FormatterDate.cpp"
#include "FormatterString.cpp"
#include "FormatterInteger.cpp"
#include "FormatterDiscard.cpp"
#include "FormatterDebug.cpp"
#include "FormatterIPaddr.cpp"
#include "FormatterIFStatement.cpp"

class InputFormatter{
public:
    FormatList formatList;
    const char *inputStream;
    InputFormatter(){
    }
    ~InputFormatter(){
        for(FormatList::iterator iter = formatList.begin(); iter != formatList.end(); ++iter) delete *iter;
        formatList.clear();
    }
//--------------------------------------
//  execute
//--------------------------------------
    void execute1(OutputManager *outputer, const char *_input){
        inputStream = _input;
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute1(outputer, &inputStream); //execute�^�ǭnskip�������O��
    }
    void execute2(InputManager *inputer){
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute2(inputer); //execute�^�ǭnskip�������O��
    }
};

#include "ConfigInterfaceIN1.cpp"
#include "ConfigInterfaceOUT1.cpp"
#include "ConfigInterfaceIN2.cpp"

#endif


