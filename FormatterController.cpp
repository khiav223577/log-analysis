#ifndef ___FormatterController_cpp__
#define ___FormatterController_cpp__
#include<stdlib.h>
#include "FormatterDate.cpp"
#include "FormatterString.cpp"
#include "FormatterInteger.cpp"
class FormatterController{
public:
    enum TYPE{ DATE,STRING,INTEGER };
    TYPE type;
    char *format;
    FormatterController(TYPE _type, const char *_format) : type(_type){
        format = trans_format(_format);
    }
    ~FormatterController(){
        free(format);
    }
//--------------------------------------
//  transform config-format to appropriate format. (for speed up)
//--------------------------------------
    char *trans_format(const char *_format){
        switch(type){
        case DATE:{    return FormatterDate::trans_format(_format);}
        case STRING:{  return FormatterString<MAX_STRING_SIZE>::trans_format(_format);}
        case INTEGER:{ return FormatterInteger::trans_format(_format);}
        default:
            printf("Unknown type:%d",type);
            exit(1);
        }
    }
//--------------------------------------
//  execute
//--------------------------------------
    void execute(const char ** inputStream){
        switch(type){
        case DATE:{
            int date = FormatterDate::retrieve(inputStream, format);
            RDate::show(date); //DEBUG
            break;}
        case STRING:{
            char *str = FormatterString<MAX_STRING_SIZE>::retrieve(inputStream, format);
            printf("[%s]\n",str); //DEBUG
            free(str); //TODO
            break;}
        case INTEGER:{

            break;}
        }
    }
};




#endif


