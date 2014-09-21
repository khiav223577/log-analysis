

#ifndef ___ConfigInterfaceIN1_cpp__
#define ___ConfigInterfaceIN1_cpp__

#include "FormatterController.cpp"
#include "lib/BlockIOManager.cpp"
#include "RStack.cpp"
#include "RubyInterpreter.cpp"
#define FormatStack RStack<FormatterIFStatement>
#define IFList std::vector<FormatterIFStatement*>
class ConfigInterfaceIN1{
public:
    InputFormatter *formatter;
    FormatList glist; //global_formatList
    RubyInterpreter *ruby;
    bool ignore_drop_node;
public:
    ConfigInterfaceIN1(RubyInterpreter *_ruby) : ruby(_ruby){
        ruby->execute_code("$IN_C_CODE = true");
        ruby->execute_file("./test.rb");
        VALUE array = rb_ary_new();
        rb_ary_push(array, rb_str_new2("INVALID"));
        rb_ary_push(array, rb_str_new2("#if"));
        rb_ary_push(array, rb_str_new2("#elsif"));
        rb_ary_push(array, rb_str_new2("#else"));
        rb_ary_push(array, rb_str_new2("#end"));
        rb_ary_push(array, rb_str_new2("EXIT_BLOCK"));
        rb_ary_push(array, rb_str_new2("#DEBUG"));
        rb_ary_push(array, rb_str_new2("Date"));
        rb_ary_push(array, rb_str_new2("String"));
        rb_ary_push(array, rb_str_new2("Int"));
        rb_ary_push(array, rb_str_new2("IPv4"));
        rb_ary_push(array, rb_str_new2("DROP"));
        rb_ary_push(array, rb_str_new2("Char"));
        rb_funcall(rb_const("ConfigReaderInterface"), rb_intern("register_hash"),  1, array);
    }
//-------------------------------------------------------------------------
//  CreateFormatter
//-------------------------------------------------------------------------
    InputFormatter *CreateFormatters(const char *filename, bool flag){
        formatter = new InputFormatter();
        glist.clear();
        ignore_drop_node = flag;
        rb_funcall(rb_const("ConfigReaderInterface"), rb_intern("read_config"),  1, rb_str_new2(filename));
        inner_retrieve_format(&formatter->formatList);
        return formatter;
    }
    void push_node(FormatterController *node){
        node->formatter = formatter;
        glist.push_back(node);
    }
//-------------------------------------------------------------------------
//  mapping Ruby data to C classes
//-------------------------------------------------------------------------
    void inner_retrieve_format(FormatList *formatList){
        IFList ifList;
        int skip_base = 0;
        FormatterController *node = NULL;
        VALUE array, type, format, hash;
        while((array  = rb_funcall(rb_const("ConfigReaderInterface"), rb_intern("get_next_result"), 0)) != Qnil){
            type   = rb_ary_entry(array,0); //[type, format, variable_name, hash]
            format = rb_ary_entry(array,1);
            hash   = rb_ary_entry(array,3);
            if (type == Qnil){
                puts("Unown type in CreateFormatter");
                continue;
            }
            switch(FIX2INT(type)){
            case  0:{ PERROR(true, puts("Unknown type! in ruby get_next_result"); );         break;} //INVALID
            case  1: case  2:{ //#if, #elsif
                FormatterIFStatement *ifnode = parse_bool_statement(format);
                ifList.push_back(ifnode); //記錄elsif數量。(elsif也是一個node，每多一個elsif，skip要再加1)。
                push_node(ifnode);
                formatList->push_back(ifnode);
                inner_retrieve_format(&ifnode->formatList);
                continue;}
            case  3:{  //#else
                int size = formatList->size();
                inner_retrieve_format(formatList);
                skip_base = formatList->size() - size;
                continue;}
            case  4:{  //#end
                for(IFList::reverse_iterator it = ifList.rbegin(); it != ifList.rend(); ++it) (*it)->skip = skip_base++;
                ifList.clear(); //elsif, else也都是node，需要被skip。例如(if, elsif, else)的skip量分別為(skip+2, skip+1, skip+0)
                skip_base = 0;
                continue;}
            case  5:{  //EXIT_BLOCK
                return;}
            //-----------------------------------
            //  Formatter
            //-----------------------------------
            case  6:{ node = new FormatterDebug  (StringValuePtr(format));                                     break;} //#DEBUG
            case  7:{ node = new FormatterDate   (StringValuePtr(format));                                     break;} //Date
            case  8:{ node = new FormatterString (StringValuePtr(format), FIX2INT(rb_hash(hash, "max_size"))); break;} //String
            case  9:{ node = new FormatterInteger(StringValuePtr(format));                                     break;} //Int
            case 10:{ node = new FormatterIPaddr (StringValuePtr(format));                                     break;} //IPv4
            case 11:{ node = new FormatterDiscard(StringValuePtr(format));                                     break;} //DROP
            case 12:{ node = new FormatterChar   (StringValuePtr(format));                                     break;} //Char
            }
            if (hash != Qnil){
                node->attr_drop  = RTEST(rb_hash(hash, "drop"));
                node->attr_peek  = RTEST(rb_hash(hash, "peek"));
                node->attr_index = RTEST(rb_hash(hash, "index"));
            }
            push_node(node);
            if (ignore_drop_node == false || node->attr_drop == false) formatList->push_back(node);
        }
    }
//-------------------------------------------------------------------------
//  parse ruby postfix data to C classes
//-------------------------------------------------------------------------
    FormatterIFStatement *parse_bool_statement(VALUE format){ //format = [[XXX],[XXX],OP,...] #postfix
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
                FormatterController *target = glist[FIX2INT(a0)];
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
    void save_config1(const char *filename, BlockConfig *config){
        FILE *file = fopen2(filename, "wb");
        fprintf(file, "%u %u\n", config->line_count, config->block_size);
        for(unsigned int i = 0, size = glist.size(); i < size; ++i) glist[i]->save_config1(file);
        fclose(file);
    }
    void save_config2(const char *filename, BlockConfig *config){
        FILE *file = fopen2(filename, "wb");
        fprintf(file, "%u %u\n", config->line_count, config->block_size);
        for(unsigned int i = 0, size = glist.size(); i < size; ++i) glist[i]->save_config2(file);
        fclose(file);
    }
    BlockConfig *load_config1(const char *filename){
        FILE *file = fopen2(filename, "rb");
        unsigned int line_count, block_size;
        fscanf(file, "%u %u\n", &line_count, &block_size);
        for(unsigned int i = 0, size = glist.size(); i < size; ++i) glist[i]->load_config1(file);
        fclose(file);
        return new BlockConfig(line_count, block_size);
    }
    BlockConfig *load_config2(const char *filename){
        FILE *file = fopen2(filename, "rb");
        unsigned int line_count, block_size;
        fscanf(file, "%u %u\n", &line_count, &block_size);
        for(unsigned int i = 0, size = glist.size(); i < size; ++i) glist[i]->load_config2(file);
        fclose(file);
        return new BlockConfig(line_count, block_size);
    }
//-------------------------------------------------------------------------
//  Index
//-------------------------------------------------------------------------
    InputIndexer *CreateIndexers(InputManager *inputer){
        InputIndexer *list = new InputIndexer();
        for(unsigned int i = 0, size = glist.size(); i < size; ++i) list->push(glist[i]->create_indexer());
        list->load(inputer);
        return list;
    }
//-------------------------------------------------------------------------
//  ACCESS
//-------------------------------------------------------------------------
    inline FormatterController *getFormatterByName(const char *variable_name){
        VALUE rb_gidx = rb_funcall(rb_const("ConfigReaderInterface"), rb_intern("get_buffer_index"), 1, rb_str_new2(variable_name));
        PERROR(rb_gidx == Qnil, printf("Cannot get_buffer_index of %s.\n", variable_name));
        unsigned int gidx = FIX2INT(rb_gidx);
        return glist[gidx];
    }
};
#undef glist
#endif
