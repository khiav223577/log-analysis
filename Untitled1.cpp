#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>

#define MAX_STRING_SIZE 511
#define MAX_CONFIG_SIZE 1024
#define MAX_LOG_SIZE 8192
#include "windows.cpp"
#include "FormatterController.cpp"
#include "RMap.cpp"
#include <vector>

class InputFormatter{
public:
#define FormatList std::vector<FormatterController*>
    FormatList formatList;
    const char *inputStream;
    InputFormatter(){
        formatList.push_back(new FormatterController(FormatterController::DATE,"MMM d HH:mm:ss "));
        formatList.push_back(new FormatterController(FormatterController::STRING," "));
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
        for(int i = 0, size = formatList.size(); i < size; ++i) formatList[i]->execute(&inputStream);
    }

#undef FormatList
};
FILE *fopen2(const char *filename, const char *mode){
    FILE *f = fopen(filename,mode);
    if (f == NULL){
        printf("Cannot open %s.",filename);
        exit(1);
    }
    return f;
}
void SetColor2(){
    static int i = 2;
    SetColor(i);
    i = (i == 8 ? 2 : i + 1);
}
void test_date_formatter(){
    InputFormatter formatter;
    srand(time(NULL));
    for(int i = 0; i < 1000; ++i){
        int year = 2000 - rand() % 61 + 30;
        int month = rand() % 12 + 1;
        int dates[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if (RDate::is_leapyear(year)) dates[1] = 29;
        int day = rand() % dates[month - 1] + 1;
        int hour = rand() % 24;
        int min = rand() % 60;
        int sec = rand() % 60;
        char data[999];
        const char *inputStream = data;
        sprintf(data,"%d/%02d/%02d %02d:%02d:%02d",year,month,day,hour,min,sec);
        SetColor2();FormatterDate::retrieve(&inputStream,"yyyy/MM/dd HH:mm:ss");
    }
    SetColor(7);
}
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
    formatter2.execute("Dec  3 04:00:01 iisfw");
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
