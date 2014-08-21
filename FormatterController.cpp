#ifndef ___FormatterController_cpp__
#define ___FormatterController_cpp__
#define FormatList std::vector<FormatterController*>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include "FileIOManager.cpp"
#include "lib/DeltaEncoding.cpp"
#include "SizeFlagManager.cpp"
#include "HashCompressor.cpp"

class InputFormatter;
class FormatterController{
public:
    class VirtualCreator{ //�קK�bconstrutor�ɵL�k���T�ϥ�virtual�禡�����D
	public:
		virtual char *trans_format(const char *_format)=0;
	};
	InputFormatter *formatter;
	OutputManager *outputer;
	InputManager *inputer;
	char *format;
	bool initialized, attr_drop, attr_peek, attr_index;
    FormatterController(const char *_format, VirtualCreator *v) : outputer(NULL), inputer(NULL){
        format = v->trans_format(_format);
        initialized = false;
        attr_drop   = false;
        attr_peek   = false;
        attr_index  = false;
        delete v;
    }
    virtual ~FormatterController(){
        free(format);
    }
    virtual char *get_prev_string(){    //Will be called by FormatterIFStatement.
        PERROR(true, printf("Error: fails to get_prev_string() in %s", typeid(*this).name()););
    }
    virtual int get_prev_int(){         //Will be called by FormatterIFStatement.
        PERROR(true, printf("Error: fails to get_prev_int() in %s", typeid(*this).name()););
    }
    virtual int execute1(const char **inputStream)=0; //�^�ǭnskip�������O��
    virtual int execute2()=0;    //�^�ǭnskip�������O��
    virtual int execute3()=0;    //�^�ǭnskip�������O��
    virtual void save_config1(FILE *file)=0;
    virtual void save_config2(FILE *file)=0;
    virtual void load_config1(FILE *file)=0;
    virtual void load_config2(FILE *file)=0;
};

#include "FormatterDate.cpp"
#include "FormatterString.cpp"
#include "FormatterChar.cpp"
#include "FormatterInteger.cpp"
#include "FormatterDiscard.cpp"
#include "FormatterDebug.cpp"
#include "FormatterIPaddr.cpp"
#include "FormatterIFStatement.cpp"

class InputFormatter{
public:
    FormatList formatList;
    InputFormatter(){
    }
    ~InputFormatter(){
        for(FormatList::iterator iter = formatList.begin(); iter != formatList.end(); ++iter) delete *iter;
        formatList.clear();
    }
//--------------------------------------
//  execute
//--------------------------------------
    void execute1(const char *_input){
        const char *inputStream = _input;
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute1(&inputStream); //execute�^�ǭnskip�������O��
    }
    void execute2(){
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute2(); //execute�^�ǭnskip�������O��
    }
    void execute3(){
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute3(); //execute�^�ǭnskip�������O��
    }
};

#include "ConfigInterfaceIN1.cpp"
#include "ConfigInterfaceOUT1.cpp"
#include "ConfigInterfaceIN2.cpp"
#include "ConfigInterfaceOUT2.cpp"
#include "ConfigInterfaceIN3.cpp"

#endif


