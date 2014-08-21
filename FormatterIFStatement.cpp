
#ifndef ___FormatterIFStatement_cpp__
#define ___FormatterIFStatement_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
class FormatterIFStatement : public FormatterController{
public:
    typedef FormatterController super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
    public:
    //-------------------------------------------------------------------------
    //  transform config-format to appropriate format. (for speed up)
    //-------------------------------------------------------------------------
        char *trans_format(const char *_format){
            char *format = (char *) malloc(strlen(_format) + 1);
            strcpy(format, _format);
            return format; //Ex: format = "iisfw"
        }
	};
	FormatterController *target;
	FormatList formatList;
    FormatterIFStatement(const char *_format, FormatterController *_target) : super(_format, new VirtualCreator()){
        target = _target;
    }
    ~FormatterIFStatement(){
        for(FormatList::iterator iter = formatList.begin(); iter != formatList.end(); ++iter) delete *iter;
        formatList.clear();
    }
public:

//--------------------------------------
//  execute
//--------------------------------------
    void execute(const char ** inputStream){
        if (check_condition() == false) return;
        for(int i = 0, size = formatList.size(); i < size; ++i) formatList[i]->execute(inputStream);
    }
    bool check_condition(){
        char *prev_result = target->get_prev_result();
        if (prev_result == NULL) return false;
        return (strcmp(format, prev_result) == 0);
    }
};

#endif
