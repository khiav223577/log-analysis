#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>
#include<vector>
#include<iostream>


#define MAX_CONFIG_SIZE 1024
#define MAX_LOG_SIZE 8192
#include "windows.cpp"
#include "FormatterController.cpp"
//#include "RMap.cpp"

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
    void execute1(OutputManager *outputer, const char *_input){
        inputStream = _input;
        for(int i = 0, size = formatList.size(); i < size; ++i) i += formatList[i]->execute1(outputer, &inputStream); //execute回傳要skip掉的指令數
    }
};
#include "ConfigInterfaceIN1.cpp"
#include "testing.cpp"
#include "FlexibleInt.cpp"
int main(){
    /*
    if (1 >= BigInteger(1)){ return 1;}

    FlexibleInt a(new BigInteger(1)), b(new BigInteger(3));
    a = b + b + FlexibleInt(new BigInteger(12));
    Sleep(500);
    printf("!");

    FlexibleInt c(new BigInteger(100)), d(new BigInteger(300));
    FlexibleInt e = c + d;
    Sleep(500);
    printf("!");

    FlexibleInt f = FlexibleInt(new BigInteger(2000));
    Sleep(500);
    printf("!");
    return 0;*/
    const char *ConfigPath = "data/test_config2";
    const char *InputPath  = "data/test_input2";
    const char *OutputPath = "data/test_output2";
    ConfigInterfaceIN1 *ruby_interface = new ConfigInterfaceIN1();
    InputFormatter *formatter = ruby_interface->CreateFormatters(ConfigPath);
    OutputManager *outputer = new OutputManager(OutputPath);
    FILE *file1 = fopen2(InputPath,"r");
    int i = 0;
    char buffer[MAX_LOG_SIZE];
    while(fgets(buffer, MAX_LOG_SIZE, file1) != NULL){
        if (buffer[0] == '\0' || buffer[0] == '\n') continue;
        printf("%02d: ", ++i);
        formatter->execute1(outputer, buffer);
        puts("");
    }
    ruby_interface->output_config(OutputPath);
    fclose(file1);
    delete outputer;
    delete formatter;
    delete ruby_interface;

    //test_InputFormatter();
    //test_FormatterDate();



/*
    MapChar(int) BlahBlah;
    char key[] = "123";
    printf("%s => %d\n",key, BlahBlah[key]);
    printf("set %s = %d\n",key, 3);
    BlahBlah[key] = 3;
    key[0] = 'A';
    key[1] = 'B';
    key[2] = 'C';
    printf("%s => %d\n",key, BlahBlah[key]); //store reference
    printf("%s => %d\n","123", BlahBlah["123"]);
    return 0;
*/


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
    /*
    const char *filename = "test";
    char buffer[MAX_LOG_SIZE];
    FILE *f2 = fopen2(filename,"r");

    while(fgets(buffer, MAX_LOG_SIZE, f2) != NULL){



    }
    fclose(f2);
    printf("%d",'\n');*/
    return 0;
}

