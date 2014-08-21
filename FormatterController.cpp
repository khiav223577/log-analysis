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
    class VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
	public:
		virtual char *trans_format(const char *_format)=0;
	};
	OutputManager *outputer;
	InputManager *inputer;
	char *format;
	bool initialized;
    FormatterController(const char *_format, VirtualCreator *v) : outputer(NULL), inputer(NULL){
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
    virtual int execute1(const char **inputStream)=0; //回傳要skip掉的指令數
    virtual int execute2()=0;    //回傳要skip掉的指令數
    virtual int execute3()=0;    //回傳要skip掉的指令數
    virtual void save_config1(FILE *file)=0;
    virtual void save_config2(FILE *file)=0;
    virtual void load_config1(FILE *file)=0;
    virtual void load_config2(FILE *file)=0;
};

class SizeFlagManager{
private:
    unsigned char byte_num;
    int Size2FlagAt;
    int Size3FlagAt;
    int Size4FlagAt;
public:
    inline void setSize2FlagAt(int val){ Size2FlagAt = val; }
    inline void setSize3FlagAt(int val){ Size3FlagAt = val; }
    inline void setSize4FlagAt(int val){ Size4FlagAt = val; }
public:
    SizeFlagManager(){
        byte_num = 1;
        Size2FlagAt = -1;
        Size3FlagAt = -1;
        Size4FlagAt = -1;
    }
    inline unsigned char get_write_byte(int output, unsigned int executeCounter){
        if (output >= (1 << 23) || output < -(1 << 23)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
            if (Size4FlagAt == -1) Size4FlagAt = executeCounter;
        }else if (output >= (1 << 15) || output < -(1 << 15)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
        }else if (output >= (1 <<  7) || output < -(1 <<  7)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
        }
        if (Size2FlagAt == -1) return 1;
        if (Size3FlagAt == -1) return 2;
        if (Size4FlagAt == -1) return 3;
        return 4;
    }
    inline unsigned char get_write_byte(unsigned int output, unsigned int executeCounter){
        if (output >= (1 << 24)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
            if (Size4FlagAt == -1) Size4FlagAt = executeCounter;
        }else if (output >= (1 << 16)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
            if (Size3FlagAt == -1) Size3FlagAt = executeCounter;
        }else if (output >= (1 <<  8)){
            if (Size2FlagAt == -1) Size2FlagAt = executeCounter;
        }
        if (Size2FlagAt == -1) return 1;
        if (Size3FlagAt == -1) return 2;
        if (Size4FlagAt == -1) return 3;
        return 4;
    }
    inline unsigned char get_read_byte(unsigned int executeCounter){
        if (Size2FlagAt == -1 || executeCounter < (unsigned int) Size2FlagAt) return 1;
        if (Size3FlagAt == -1 || executeCounter < (unsigned int) Size3FlagAt) return 2;
        if (Size4FlagAt == -1 || executeCounter < (unsigned int) Size4FlagAt) return 3;
        return 4;
    }
    inline void save(FILE *file){
        fprintf(file, " %d %d %d", Size2FlagAt, Size3FlagAt, Size4FlagAt);
    }
    inline void load(FILE *file){
        fscanf(file, " %d %d %d", &Size2FlagAt, &Size3FlagAt, &Size4FlagAt);
    }
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
    void execute1(const char *_input){
        inputStream = _input;
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute1(&inputStream); //execute回傳要skip掉的指令數
    }
    void execute2(){
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute2(); //execute回傳要skip掉的指令數
    }
    void execute3(){
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute3(); //execute回傳要skip掉的指令數
    }
};

#include "ConfigInterfaceIN1.cpp"
#include "ConfigInterfaceOUT1.cpp"
#include "ConfigInterfaceIN2.cpp"
#include "ConfigInterfaceOUT2.cpp"
#include "ConfigInterfaceIN3.cpp"

#endif


