
#include<stdio.h>
#include<iostream>
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

#define MAX_LOG_SIZE 8192
RubyInterpreter *ruby;
ConfigInterfaceIN1 *ruby_interface;
InputFormatter *formatter;
inline void first_pass(const char *input_path, const char *output_path, const char *config_path){
    OutputManager *outputer = new OutputManager(output_path);
    FILE *file1 = fopen2(input_path,"r");
    int i = 0;
    char buffer[MAX_LOG_SIZE];
    while(fgets(buffer, MAX_LOG_SIZE, file1) != NULL){
        if (buffer[0] == '\0' || buffer[0] == '\n') continue;
        printf("%02d: ", ++i);
        formatter->execute1(outputer, buffer);
        puts("");
    }
    ruby_interface->output_config(config_path);
    fclose(file1);
    delete outputer;
}
inline void second_pass(const char *input_path, const char *output_path, const char *config_path){

}
int main(int argc, char **argv){

    /*
    PERROR(1 == 1, printf("read BigInt fail, format = %s, input = %s", "000", ":???"););
    return 0;
    std::cout << bigIntegerToString(BigInteger(BigUnsignedInABase("01Aa", 16)));
    return 0;
    */
    const char *ConfigPath = "data/test_config2";
    const char *InputPath  = "data/test_input2";
    const char *OutputPath = "data/test_output2";
    ruby = new RubyInterpreter();

    {
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath);
        char *input_path  = (char *) malloc(sizeof(char) * strlen( InputPath) + 1 + 0);
        char *output_path = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *config_path = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        sprintf( input_path, "%s"        ,  InputPath);
        sprintf(output_path, "%s.temp1"  , OutputPath);
        sprintf(config_path, "%s.config1", OutputPath);
        first_pass(input_path, output_path, config_path);
        free( input_path);
        free(output_path);
        free(config_path);
        delete formatter;
        delete ruby_interface;
    }
    {
        ruby_interface = new ConfigInterfaceIN1(ruby);
        formatter = ruby_interface->CreateFormatters(ConfigPath);
        char *input_path  = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *output_path = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 5);
        char *config_path = (char *) malloc(sizeof(char) * strlen(OutputPath) + 1 + 7);
        sprintf(input_path , "%s.temp1"  , OutputPath);
        sprintf(output_path, "%s.temp2"  , OutputPath);
        sprintf(config_path, "%s.config2", OutputPath);
        second_pass(input_path, output_path, config_path);
        free( input_path);
        free(output_path);
        free(config_path);
        delete formatter;
        delete ruby_interface;
    }

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

