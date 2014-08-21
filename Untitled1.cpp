#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>
#include<vector>

#define MAX_STRING_SIZE 511
#define MAX_CONFIG_SIZE 1024
#define MAX_LOG_SIZE 8192
#include "windows.cpp"
#include "FormatterController.cpp"
#include "RMap.cpp"
#include "RubyInterpreter.cpp"
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
    void execute(const char *_input){
        inputStream = _input;
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute(&inputStream);
    }
};
FILE *fopen2(const char *filename, const char *mode){
    FILE *f = fopen(filename,mode);
    if (f == NULL){
        printf("Cannot open %s.",filename);
        exit(1);
    }
    return f;
}
#include "testing.cpp"
InputFormatter* CreateFormatter(){
    InputFormatter *formatter = new InputFormatter();
    RubyInterpreter ruby;
    ruby.execute_code("$IN_C_CODE = true");
    ruby.execute_file("./test.rb");
    const char *types[9] = {"INVALID", "Date", "String", "Int", "IPv4", "DROP", "#if", "#elsif", "#end"};
    for (int i = 0; i < 9; ++i) rb_funcall(rb_gv_get("$!"), rb_intern("register_hash"),  2, rb_str_new2(types[i]), INT2FIX(i));
    rb_funcall(rb_gv_get("$!"), rb_intern("read_config"),  1, rb_str_new2("test_config"));

    VALUE array, type, format, extra;
    while((array  = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0)) != Qnil){
        type   = rb_ary_entry(array,0);
        format = rb_ary_entry(array,1);
        extra  = rb_ary_entry(array,3);
        if (type == Qnil){
            puts("Unown type in CreateFormatter");
            continue;
        }
        switch(FIX2INT(type)){
        case 1:{ //Date
            formatter->formatList.push_back(new FormatterDate(StringValuePtr(format)));
            break;}
        case 2:{ //String
            formatter->formatList.push_back(new FormatterString(StringValuePtr(format), FIX2INT(extra)));
            break;}
        case 3:{ //Int
            formatter->formatList.push_back(new FormatterInteger(StringValuePtr(format)));
            break;}
        case 4:{ //IPv4
            formatter->formatList.push_back(new FormatterIPaddr(NULL));
            break;}
        case 5:{ //DROP
            formatter->formatList.push_back(new FormatterDiscard(StringValuePtr(format)));
            break;}
        }
    }
    formatter->execute("Dec  3 04:00:01 iisfw 1,2013/12/03 04:00:01,0011C101825,TRAFFIC,end,1,2013/12/03 04:00:00,140.109.23.120,140.109.254.5,0.0.0.0,0.0.0.0");
    return formatter;
/*
    VALUE array = rb_funcall(rb_gv_get("$!"), rb_intern("return_string"), 0);
    VALUE test1 = rb_ary_entry(array,0);
    printf("%s\n",StringValuePtr(test1));
    VALUE test2 = rb_ary_entry(array,1);
    printf("%s\n",StringValuePtr(test2));*/
}
int main(){

    //InputFormatter *formatter = CreateFormatter();
    //delete formatter;

    test_InputFormatter();
    //test_FormatterDate();



/*
    MapChar(int) BlahBlah;
    char key[] = "123";
    printf("%d",BlahBlah[key]);
    BlahBlah[key] = 3;
    key[0] = 'A';
    key[1] = 'B';
    key[2] = 'C';
    printf("[%s]\n",key);
    printf("%d",BlahBlah[key]);
    printf("%d",BlahBlah["123"]);
    return 0;*/


return 0;
    /*
    char string[] = "a string,of ,,tokens";
    char *token = strtok(string,"o")
    while (token != NULL){
        printf("%s\n", token);
        token = strtok((char *) NULL,"[^ ]");
    }*/
    return 0;
    /*
    FILE *f1 = fopen2("config","r");
    char config[MAX_CONFIG_SIZE];
    fclose(f1);*/
    const char *filename = "test";
    char buffer[MAX_LOG_SIZE];
    FILE *f2 = fopen2(filename,"r");

    while(fgets(buffer, MAX_LOG_SIZE, f2) != NULL){



    }
    fclose(f2);
    printf("%d",'\n');
    return 0;
}
