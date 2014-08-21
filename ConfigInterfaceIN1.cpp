

#ifndef ___ConfigInterfaceIN1_cpp__
#define ___ConfigInterfaceIN1_cpp__

#include "FormatterController.cpp"
#include "RStack.cpp"
#include "RubyInterpreter.cpp"
#define FormatStack RStack<FormatterIFStatement>
#define IFList std::vector<FormatterIFStatement*>
class ConfigInterfaceIN1{
public:
    FormatList global_formatList;
    RubyInterpreter *ruby;
public:
    ConfigInterfaceIN1(RubyInterpreter *_ruby) : ruby(_ruby){
    }
//-------------------------------------------------------------------------
//  CreateFormatter
//-------------------------------------------------------------------------
    InputFormatter* CreateFormatters(const char *filename){
        InputFormatter *formatter = new InputFormatter();

        ruby->execute_code("$IN_C_CODE = true");
        ruby->execute_file("./test.rb");

        rb_eval_string("register_hash(['INVALID', '#if', '#elsif', '#else', '#end', 'EXIT_BLOCK', '#DEBUG', 'Date', 'String', 'Int', 'IPv4', 'DROP', 'Char'])");
        rb_funcall(rb_gv_get("$!"), rb_intern("read_config"),  1, rb_str_new2(filename));

        inner_retrieve_format(&formatter->formatList);
        return formatter;
    }
//-------------------------------------------------------------------------
//  mapping Ruby data to C classes
//-------------------------------------------------------------------------
    void inner_retrieve_format(FormatList *formatList){
        IFList ifList;
        int skip_base = 0;
        FormatterController *node;
        VALUE array, type, format, hash;
        while((array  = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0)) != Qnil){
            type   = rb_ary_entry(array,0); //[type, format, variable_name, hash]
            format = rb_ary_entry(array,1);
            hash   = rb_ary_entry(array,3);

            if (type == Qnil){
                puts("Unown type in CreateFormatter");
                continue;
            }
            switch(FIX2INT(type)){
            case  0:{ PERROR(true, puts("Unknown type! in ruby return_string"); );         break;} //INVALID
            case  1: case  2:{ //#if, #elsif
                FormatterIFStatement *ifnode = parse_bool_statement(format);
                ifList.push_back(ifnode); //記錄elsif數量。(elsif也是一個node，每多一個elsif，skip要再加1)。
                global_formatList.push_back(ifnode);
                formatList->push_back(ifnode);
                inner_retrieve_format(&ifnode->formatList);
                continue;
                break;}
            case  3:{  //#else
                int size = formatList->size();
                inner_retrieve_format(formatList);
                skip_base = formatList->size() - size;
                continue;
                break;}
            case  4:{  //#end
                for(IFList::reverse_iterator it = ifList.rbegin(); it != ifList.rend(); ++it) (*it)->skip = skip_base++;
                ifList.clear(); //elsif, else也都是node，需要被skip。例如(if, elsif, else)的skip量分別為(skip+2, skip+1, skip+0)
                skip_base = 0;
                continue;
                break;}
            case  5:{ //EXIT_BLOCK
                return;
                break;}
            //------------------------------
            //  Formatter
            //------------------------------
            case  6:{ node = new FormatterDebug  (StringValuePtr(format));                 break;} //#DEBUG
            case  7:{ node = new FormatterDate   (StringValuePtr(format));                 break;} //Date
            case  8:{ //String
                int max_size = FIX2INT(rb_hash(hash, "max_size"));
                node = new FormatterString(StringValuePtr(format), max_size);
                break;}
            case  9:{ node = new FormatterInteger(StringValuePtr(format));                 break;} //Int
            case 10:{ node = new FormatterIPaddr (StringValuePtr(format));                 break;} //IPv4
            case 11:{ node = new FormatterDiscard(StringValuePtr(format));                 break;} //DROP
            case 12:{ node = new FormatterChar   (StringValuePtr(format));                 break;} //Char
            }
            global_formatList.push_back(node);
            formatList->push_back(node);
        }
    }
//-------------------------------------------------------------------------
//  parse ruby postfix data to C classes
//-------------------------------------------------------------------------
    FormatterIFStatement* parse_bool_statement(VALUE format){ //format = [[XXX],[XXX],OP,...] #postfix
        long len = RARRAY_LEN(format);
        FormatStack *stack = NULL;
        for (int i = 0; i < len; ++i){
            VALUE element = rb_ary_entry(format,i);
            switch (TYPE(element)) {
            case T_FIXNUM:{ //element = '|' or '&'
                FormatterIFStatement *rExpr = FormatStack::pop(&stack);
                FormatterIFStatement *lExpr = FormatStack::pop(&stack);
                FormatStack::push(&stack, new FormatterIF_CmpIF(FIX2INT(element), rExpr, lExpr));
                break;}
            case T_ARRAY:{  //[idx, "==", "string"]
                VALUE a0 = rb_ary_entry(element,0);
                VALUE a1 = rb_ary_entry(element,1);
                VALUE a2 = rb_ary_entry(element,2);
                FormatterController *target = global_formatList[FIX2INT(a0)];
                char op = FIX2INT(a1);
                char *string = StringValuePtr(a2);
                switch(string[0]){
                case '"':{ FormatStack::push(&stack, new FormatterIF_CmpString(op, target, string)); break;} //string
                default:{  FormatStack::push(&stack, new FormatterIF_CmpInt   (op, target, string)); break;} //integer
                }
                break;}
            default:{
                puts("not valid value in 'parse_bool_statement'");
                //rb_raise(rb_eTypeError, "not valid value");
                break;}
            }
        }
        FormatterIFStatement *result = FormatStack::pop(&stack);
        FormatStack::clear(&stack);
        return result;
    }
//-------------------------------------------------------------------------
//  config
//-------------------------------------------------------------------------
    void save_config1(int line_count, const char *filename){
        FILE *file = fopen2(filename, "w");
        fprintf(file, "%d\n", line_count);
        int size = global_formatList.size();
        for(int i = 0; i < size; ++i) global_formatList[i]->save_config1(file);
        fclose(file);
    }
    void save_config2(int line_count, const char *filename){
        FILE *file = fopen2(filename, "w");
        fprintf(file, "%d\n", line_count);
        int size = global_formatList.size();
        for(int i = 0; i < size; ++i) global_formatList[i]->save_config2(file);
        fclose(file);
    }
    int load_config1(const char *filename){
        FILE *file = fopen2(filename, "r");
        int line_count, size = global_formatList.size();
        fscanf(file, "%d\n", &line_count);
        for(int i = 0; i < size; ++i) global_formatList[i]->load_config1(file);
        fclose(file);
        return line_count;
    }
    int load_config2(const char *filename){
        FILE *file = fopen2(filename, "r");
        int line_count, size = global_formatList.size();
        fscanf(file, "%d\n", &line_count);
        for(int i = 0; i < size; ++i) global_formatList[i]->load_config2(file);
        fclose(file);
        return line_count;
    }
};

#endif
