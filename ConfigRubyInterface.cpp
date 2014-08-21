

#ifndef ___ConfigRubyInterface_cpp__
#define ___ConfigRubyInterface_cpp__

#include "FormatterController.cpp"
#include "RStack.cpp"
#include "RubyInterpreter.cpp"
#define FormatStack RStack<FormatterIFStatement>
class ConfigRubyInterface{
private:
    FormatList global_formatList;
public:
//-------------------------------------------------------------------------
//  CreateFormatter
//-------------------------------------------------------------------------
    InputFormatter* CreateFormatter(){
        InputFormatter *formatter = new InputFormatter();
        RubyInterpreter ruby;
        ruby.execute_code("$IN_C_CODE = true");
        ruby.execute_file("./test.rb");
        const char *types[9] = {"INVALID", "Date", "String", "Int", "IPv4", "DROP", "#if", "#elsif", "#end"};
        for (int i = 0; i < 9; ++i) rb_funcall(rb_gv_get("$!"), rb_intern("register_hash"),  2, rb_str_new2(types[i]), INT2FIX(i));
        rb_funcall(rb_gv_get("$!"), rb_intern("read_config"),  1, rb_str_new2("test_config"));


        inner_retrieve_format(&formatter->formatList);
        return formatter;
    /*
        VALUE array = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0);
        VALUE test1 = rb_ary_entry(array,0);
        printf("%s\n",StringValuePtr(test1));
        VALUE test2 = rb_ary_entry(array,1);
        printf("%s\n",StringValuePtr(test2));*/
    }
//-------------------------------------------------------------------------
//  mapping Ruby data to C classes
//-------------------------------------------------------------------------
    void inner_retrieve_format(FormatList *formatList){
        FormatterController *node;
        VALUE array, type, format, extra;
        while((array  = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0)) != Qnil){
            type   = rb_ary_entry(array,0); //[type, format, variable_name, extra]
            format = rb_ary_entry(array,1);
            extra  = rb_ary_entry(array,3);
            if (type == Qnil){
                puts("Unown type in CreateFormatter");
                continue;
            }

            switch(FIX2INT(type)){
            case 1:{ node = new FormatterDate   (StringValuePtr(format));                 break;} //Date
            case 2:{ node = new FormatterString (StringValuePtr(format), FIX2INT(extra)); break;} //String
            case 3:{ node = new FormatterInteger(StringValuePtr(format));                 break;} //Int
            case 4:{ node = new FormatterIPaddr (NULL);                                   break;} //IPv4
            case 5:{ node = new FormatterDiscard(StringValuePtr(format));                 break;} //DROP
            case 6:{ //IF
                node = parse_bool_statement(format);
                //node = new FormatterIFStatement('T', target, "string");

                //formatter->formatList.push_back(new FormatterDiscard(StringValuePtr(format)));
                break;}
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
        puts("=================================");
        printf("%d",len);
        for (int i = 0; i < len; ++i){
            VALUE element = rb_ary_entry(format,i);
            switch (TYPE(element)) {
            case T_FIXNUM:{ //'|', '&'
                printf("[%c]\n",FIX2INT(element));
                FormatterIFStatement *rExpr = FormatStack::pop(&stack);
                FormatterIFStatement *lExpr = FormatStack::pop(&stack);
                FormatStack::push(&stack, new FormatterIFStatement(FIX2INT(element), rExpr, lExpr));
                break;}
            case T_ARRAY:{  //[idx, "==","string"]
                VALUE a0 = rb_ary_entry(element,0);
                VALUE a1 = rb_ary_entry(element,1);
                VALUE a2 = rb_ary_entry(element,2);
                FormatterController *target = global_formatList[FIX2INT(a0)];
                char type = (FIX2INT(a1) == '!' ? 'F' : 'T');
                char *string = StringValuePtr(a2);
                FormatStack::push(&stack, new FormatterIFStatement(type, target, string));
                break;}
            default:{
                rb_raise(rb_eTypeError, "not valid value");
                break;}
            }
        }
        FormatterIFStatement *result = FormatStack::pop(&stack);
        FormatStack::clear(&stack);
        puts("=================================");
        return result;
    }
};

#endif
