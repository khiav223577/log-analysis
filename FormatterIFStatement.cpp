
#ifndef ___FormatterIFStatement_cpp__
#define ___FormatterIFStatement_cpp__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

class FormatterIFStatement : public FormatterController{
public:
    typedef FormatterController super;
//-------------------------------------------------------------------------
//  transform config-format to appropriate format. (for speed up)
//-------------------------------------------------------------------------
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        virtual char *trans_format(const char *_format)=0;
	};
	char boolOperator;
	//<------------The following two should be set by hand.------------>
	FormatList formatList; //execute extra statement if check_condition is TRUE
	int skip;              //skip some statement if executed
	FormatterIFStatement(const char *_format, char op, VirtualCreator *v) : super(_format, v){
        skip = 0;
        boolOperator = op;
	}
    virtual ~FormatterIFStatement(){
        for(FormatList::iterator iter = formatList.begin(); iter != formatList.end(); ++iter) delete *iter;
        formatList.clear();
    }
    inline void Perror(char op){
        printf("Unknown operator in FormatterIFStatement: '%c'", op);
        exit(1);
    }
public:
    virtual void output_config1(FILE *file);
//--------------------------------------
//  execute
//--------------------------------------
    virtual bool check_condition()=0;
    inline int execute1(OutputManager *outputer, const char **inputStream){
        if (check_condition() == false) return 0;
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute1(outputer, inputStream); //execute回傳要skip掉的指令數
        return skip;
    }
};

//====================================================================
//  Compare string.
//====================================================================

class FormatterIF_CmpString : public FormatterIFStatement{
public:
    typedef FormatterIFStatement super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){ //_format = "\"string\""
            if (_format == NULL) return NULL;
            int len = strlen(_format);
            char *format = (char *) malloc(len - 1);
            memcpy(format, _format + 1, (len - 2) * sizeof(char));
            format[len - 2] = '\0';
            return format; //Ex: format = "iisfw"
        }
	};
	FormatterController *compareTarget;
//-------------------------------------------------------------------------
//  target == format  /  target != format
//-------------------------------------------------------------------------
    FormatterIF_CmpString(char op, FormatterController *_target, const char *_format) : super(_format, op,  new VirtualCreator()){
        if (op != '=' && op != '!') Perror(op);
        compareTarget = _target;
    }
    bool check_condition(){
        switch(boolOperator){
        case '=':{ return (strcmp(compareTarget->get_prev_string(), format) == 0);} //operator = "=="
        case '!':{ return (strcmp(compareTarget->get_prev_string(), format) != 0);} //operator = "!="
        default:{
            Perror(boolOperator);
            return false;}
        }
    }
};

//====================================================================
//  Compare Integer.
//====================================================================

class FormatterIF_CmpInt : public FormatterIFStatement{
public:
    typedef FormatterIFStatement super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            return NULL;
        }
	};
	FormatterController *compareTarget;
	int format_int;
//-------------------------------------------------------------------------
//  target == format  /  target != format
//-------------------------------------------------------------------------
    FormatterIF_CmpInt(char op, FormatterController *_target, const char *_format) : super(_format, op,  new VirtualCreator()){
        if (op != '=' && op != '!' && op != '>' && op != '<' && op != '.' && op != ',') Perror(op);
        compareTarget = _target;
        format_int = atoi(_format);
    }
    bool check_condition(){
        switch(boolOperator){
        case '=':{ return (compareTarget->get_prev_int() == format_int);}  //operator = "=="
        case '!':{ return (compareTarget->get_prev_int() != format_int);}  //operator = "!="
        case '>':{ return (compareTarget->get_prev_int() >  format_int);}  //operator = ">"
        case '<':{ return (compareTarget->get_prev_int() <  format_int);}  //operator = "<"
        case '.':{ return (compareTarget->get_prev_int() >= format_int);}  //operator = ">="
        case ',':{ return (compareTarget->get_prev_int() <= format_int);}  //operator = "<="
        default:{
            Perror(boolOperator);
            return false;}
        }
    }
};

//====================================================================
//  Compare boolean expression.
//====================================================================

class FormatterIF_CmpIF : public FormatterIFStatement{
public:
    typedef FormatterIFStatement super;
    class VirtualCreator : public super::VirtualCreator{ //避免在construtor時無法正確使用virtual函式的問題
        char *trans_format(const char *_format){
            return NULL;
        }
	};
	FormatterIFStatement *lExpr, *rExpr;
    //-------------------------------------------------------------------------
    //  result == lExpr && rExpr  /  result == lExpr || rExpr
    //-------------------------------------------------------------------------
    FormatterIF_CmpIF(char op, FormatterIFStatement *_lExpr, FormatterIFStatement *_rExpr) : super(NULL, op, new VirtualCreator()){
        if (op != '&' && op != '|') Perror(op);
        lExpr = _lExpr;
        rExpr = _rExpr;
    }
    ~FormatterIF_CmpIF(){
        delete lExpr;
        delete rExpr;
    }
    bool check_condition(){
        switch(boolOperator){
        case '|':{
            if (lExpr->check_condition() == true ) return true;
            if (rExpr->check_condition() == true ) return true;
            return false;}
        case '&':{
            if (lExpr->check_condition() == false) return false;
            if (rExpr->check_condition() == false) return false;
            return true;}
        default:{
            Perror(boolOperator);
            return false;}
        }
    }
};

#endif
