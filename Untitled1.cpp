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
class InputFormatter{
public:
    FormatList formatList;
    const char *inputStream;
    InputFormatter(){
        formatList.push_back(new FormatterDate("MMM d HH:mm:ss "));
        formatList.push_back(new FormatterString<MAX_STRING_SIZE>(" "));
        formatList.push_back(new FormatterInteger("10"));
        formatList.push_back(new FormatterDiscard(","));
        formatList.push_back(new FormatterDate("yyyy/MM/dd HH:mm:ss,"));
        formatList.push_back(new FormatterString<MAX_STRING_SIZE>(","));
        formatList.push_back(new FormatterString<MAX_STRING_SIZE>(","));
        formatList.push_back(new FormatterString<MAX_STRING_SIZE>(","));
        formatList.push_back(new FormatterInteger("10"));
        formatList.push_back(new FormatterDiscard(","));
        formatList.push_back(new FormatterDate("yyyy/MM/dd HH:mm:ss,"));
        formatList.push_back(new FormatterIPaddr(NULL));
        formatList.push_back(new FormatterDiscard(","));
        formatList.push_back(new FormatterIPaddr(NULL));
        formatList.push_back(new FormatterDiscard(","));
        formatList.push_back(new FormatterIPaddr(NULL));
        formatList.push_back(new FormatterDiscard(","));
        formatList.push_back(new FormatterIPaddr(NULL));
        formatList.push_back(new FormatterDiscard(","));
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
int main(){


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
    InputFormatter formatter2;
    formatter2.execute("Dec  3 04:00:01 iisfw 1,2013/12/03 04:00:01,0011C101825,TRAFFIC,end,1,2013/12/03 04:00:00,140.109.23.120,140.109.254.5,");
return 0;
    test_date_formatter();
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
