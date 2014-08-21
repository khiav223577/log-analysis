
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
            if (_format == NULL) return NULL;
            char *format = (char *) malloc(strlen(_format) + 1);
            strcpy(format, _format);
            return format; //Ex: format = "iisfw"
        }
	};
	FormatterIFStatement *lExpr, *rExpr;
	FormatterController *compareTarget;
	char boolOperator;
	//<------------The following two should be set by hand.------------>
	FormatList formatList; //execute extra statement if check_condition is TRUE
	int skip;              //skip some statement if executed
    //-------------------------------------------------------------------------
    //  target == format  /  target != format
    //-------------------------------------------------------------------------
    FormatterIFStatement(char op, FormatterController *_target, const char *_format) : super(_format, new VirtualCreator()){
        if (op != 'T' && op != 'F') Perror(op);
        skip = 0;
        boolOperator = op;
        compareTarget = _target;
        lExpr = NULL;
        rExpr = NULL;
    }
    //-------------------------------------------------------------------------
    //  result == lExpr && rExpr  /  result == lExpr || rExpr
    //-------------------------------------------------------------------------
    FormatterIFStatement(char op, FormatterIFStatement *_lExpr, FormatterIFStatement *_rExpr) : super(NULL, new VirtualCreator()){
        if (op != '&' && op != '|') Perror(op);
        skip = 0;
        boolOperator = op;
        compareTarget = NULL;
        lExpr = _lExpr;
        rExpr = _rExpr;
    }
    ~FormatterIFStatement(){
        for(FormatList::iterator iter = formatList.begin(); iter != formatList.end(); ++iter) delete *iter;
        formatList.clear();
        delete lExpr;
        delete rExpr;
    }
    inline void Perror(char op){
        printf("Unknown operator in FormatterIFStatement: %c(%d)", op, op);
        exit(1);
    }
public:

//--------------------------------------
//  execute
//--------------------------------------
    inline int execute(const char ** inputStream){
        if (check_condition() == false) return 0;
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute(inputStream);
        return skip;
    }
    inline bool check_condition(){
        switch(boolOperator){
        case '|':{
            if (lExpr->check_condition() == true ) return true;
            if (rExpr->check_condition() == true ) return true;
            return false;}
        case '&':{
            if (lExpr->check_condition() == false) return false;
            if (rExpr->check_condition() == false) return false;
            return true;}
        case 'T':  case 'F':{
            char *prev_result = compareTarget->get_prev_result();
            if (prev_result == NULL) return false;
            int cmp_result = strcmp(format, prev_result);
            return (boolOperator == 'T') ? (cmp_result == 0) : (cmp_result != 0);
            Perror(boolOperator);}
        default:{
            Perror(boolOperator);
            return false;}
        }
    }
};

#endif
